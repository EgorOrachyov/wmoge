/**********************************************************************************/
/* Wmoge game engine                                                              */
/* Available at github https://github.com/EgorOrachyov/wmoge                      */
/**********************************************************************************/
/* MIT License                                                                    */
/*                                                                                */
/* Copyright (c) 2023 Egor Orachyov                                               */
/*                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy   */
/* of this software and associated documentation files (the "Software"), to deal  */
/* in the Software without restriction, including without limitation the rights   */
/* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      */
/* copies of the Software, and to permit persons to whom the Software is          */
/* furnished to do so, subject to the following conditions:                       */
/*                                                                                */
/* The above copyright notice and this permission notice shall be included in all */
/* copies or substantial portions of the Software.                                */
/*                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    */
/* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  */
/* SOFTWARE.                                                                      */
/**********************************************************************************/

#include "shader.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "core/task.hpp"
#include "glsl/glsl_builder.hpp"
#include "glsl/glsl_include_processor.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_builder.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_compiler_task_manager.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "io/enum.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace wmoge {

    Shader::~Shader() {
        Config* config = IocContainer::iresolve_v<Config>();

        if (config->get_bool_or_default(SID("grc.shader.cache.save_cache"), true)) {
            save_cache(m_active_platform);
        }
    }

    Status Shader::init(ShaderReflection& reflection) {
        WG_AUTO_PROFILE_GRC("Shader::from_reflection");

        PsoCache*      pso_cache      = IocContainer::iresolve_v<PsoCache>();
        ShaderManager* shader_manager = IocContainer::iresolve_v<ShaderManager>();
        GfxDriver*     driver         = IocContainer::iresolve_v<GfxDriver>();
        Config*        config         = IocContainer::iresolve_v<Config>();

        m_reflection      = std::move(reflection);
        m_active_platform = driver->get_shader_platform();
        m_cache_path      = "cache://";

        config->get_string(SID("grc.shader.cache.path"), m_cache_path);

        for (std::int16_t space_idx = 0; space_idx < get_num_spaces(); space_idx++) {
            GfxDescSetLayoutDesc layout_desc;
            fill_layout_desc(layout_desc, space_idx);
            Ref<GfxDescSetLayout> layout = pso_cache->get_or_create_desc_layout(layout_desc);
            m_layouts.push_back(std::move(layout));
        }

        m_pso_layout = pso_cache->get_or_create_pso_layout(m_layouts);

        m_env = shader_manager->get_compiler_env();

        m_env.virtual_includes["generated/declarations.glsl"] = "";// virtual include with generated inperface of the shader
        m_env.virtual_includes["generated/input.glsl"]        = "";// virtual include with generated vertex input format

        for (const auto lang : m_reflection.languages) {
            WG_CHECKED(fill_declarations(lang, m_cached_declarations[lang]));
        }

        if (config->get_bool_or_default(SID("grc.shader.cache.load_cache"), true)) {
            WG_CHECKED(load_cache(m_active_platform, true));
        }

        WG_CHECKED(reanalyse_includes());

        return WG_OK;
    }

    Status Shader::fill_layout_desc(GfxDescSetLayoutDesc& desc, std::int16_t space) const {
        WG_AUTO_PROFILE_GRC("Shader::fill_layout_desc");

        assert(0 <= space && space < m_reflection.spaces.size());

        const ShaderSpace& shader_space = m_reflection.spaces[space];

        std::int16_t binding_id = 0;

        for (const ShaderBinding& binding : shader_space.bindings) {
            GfxDescBinging& gfx_binding = desc.emplace_back();
            gfx_binding.binding         = binding_id++;
            gfx_binding.count           = 1;
            gfx_binding.name            = binding.name;

            switch (binding.binding) {
                case ShaderBindingType::InlineUniformBuffer:
                case ShaderBindingType::UniformBuffer:
                    gfx_binding.type = GfxBindingType::UniformBuffer;
                    break;

                case ShaderBindingType::Sampler2d:
                case ShaderBindingType::Sampler2dArray:
                case ShaderBindingType::SamplerCube:
                    gfx_binding.type = GfxBindingType::SampledTexture;
                    break;

                case ShaderBindingType::StorageBuffer:
                    gfx_binding.type = GfxBindingType::StorageBuffer;
                    break;

                case ShaderBindingType::StorageImage2d:
                    gfx_binding.type = GfxBindingType::StorageImage;
                    break;

                default:
                    return StatusCode::InvalidState;
            }
        }

        return WG_OK;
    }

    Status Shader::fill_layouts(GfxDescSetLayouts& layouts) const {
        layouts = m_layouts;
        return WG_OK;
    }

    Status Shader::fill_compiler_env(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompilerEnv& compiler_env) {
        WG_AUTO_PROFILE_GRC("Shader::fill_compiler_env");

        compiler_env.merge(m_env);

        compiler_env.set_define(SID(GfxShaderPlatformGlslDefines[int(platform)]));
        compiler_env.set_define(SID("TECHNIQUE_IDX"), permutation.technique_idx);
        compiler_env.set_define(SID("PASS_IDX"), permutation.pass_idx);

        permutation.vert_attribs.for_each([&](int idx, GfxVertAttrib) {
            compiler_env.set_define(SID(GfxVertAttribGlslDefines[idx]));
        });

        for (std::int16_t i = 0; i < ShaderOptions::MAX_OPTIONS; i++) {
            if (permutation.options[i]) {
                Strid option, variant;
                WG_CHECKED(fill_option_info(permutation.technique_idx, i, option, variant));
                compiler_env.set_define(SID(option.str() + "_" + variant.str()));
            }
        }

        compiler_env.virtual_includes["generated/declarations.glsl"] = m_cached_declarations[lang];

        WG_CHECKED(fill_vertex_input(lang, permutation, compiler_env.virtual_includes["generated/input.glsl"]));

        return WG_OK;
    }

    Status Shader::fill_compiler_input(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompilerInput& compiler_input) {
        WG_AUTO_PROFILE_GRC("Shader::fill_compiler_input");

        std::string program_name;
        if (!fill_program_name(lang, platform, permutation, program_name)) {
            WG_LOG_ERROR("failed to get program name to compile " << get_shader_name());
            return StatusCode::Error;
        }

        if (!fill_compiler_env(lang, platform, permutation, compiler_input.env)) {
            WG_LOG_ERROR("failed to fill env to compile " << program_name);
            return StatusCode::Error;
        }

        for (ShaderSourceFile& source_file : m_reflection.sources) {
            if (source_file.lang == lang) {
                ShaderCompilerInputFile& f = compiler_input.files.emplace_back();
                f.name                     = source_file.file;
                f.file_path                = source_file.file;
                f.module_type              = source_file.module;
                f.entry_point              = "main";
            }
        }

        compiler_input.name     = SID(program_name);
        compiler_input.language = lang;
        compiler_input.options  = IocContainer::iresolve_v<ShaderManager>()->get_compiler_options();

        return WG_OK;
    }

    Status Shader::fill_program_name(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, std::string& name) {
        WG_AUTO_PROFILE_GRC("Shader::fill_program_name");

        std::stringstream stream;

        stream << "shader=" << get_shader_name() << " ";
        stream << "perm=" << permutation.hash() << " ";
        stream << "tech=" << permutation.technique_idx << " ";
        stream << "pass=" << permutation.pass_idx << " ";
        stream << "vf=" << permutation.vert_attribs.to_string() << " ";
        stream << "opts: ";

        for (std::int16_t i = 0; i < ShaderOptions::MAX_OPTIONS; i++) {
            if (permutation.options[i]) {
                Strid option, variant;
                WG_CHECKED(fill_option_info(permutation.technique_idx, i, option, variant));
                stream << option << "=" << variant << " ";
            }
        }

        name = stream.str();

        return WG_OK;
    }

    Status Shader::fill_option_info(std::int16_t technique_idx, std::int16_t permutation_bit, Strid& option, Strid& variant) {
        ShaderTechniqueInfo& technique = m_reflection.techniques[technique_idx];
        option                         = technique.options_remap[permutation_bit];
        variant                        = technique.variants_remap[permutation_bit];
        return WG_OK;
    }

    Status Shader::fill_declarations(GfxShaderLang lang, std::string& out_declarations) {
        WG_AUTO_PROFILE_GRC("Shader::fill_declarations");

        assert(lang == GfxShaderLang::GlslVk450);

        GlslBuilder                                             builder;
        std::function<void(const Ref<ShaderType>& struct_type)> visitor;
        flat_set<Ref<ShaderType>>                               visited;

        auto emit_struct = [&](const Ref<ShaderType>& struct_type) {
            for (const auto& field : struct_type->fields) {
                if (field.is_array && field.elem_count == 0) {
                    builder.add_field(field.type->name, field.name, std::nullopt);
                }
                if (field.is_array && field.elem_count > 0) {
                    builder.add_field(field.type->name, field.name, field.elem_count);
                }
                if (!field.is_array) {
                    builder.add_field(field.type->name, field.name);
                }
            }
        };

        visitor = [&](const Ref<ShaderType>& struct_type) {
            if (visited.find(struct_type) != visited.end()) {
                return;
            }

            visited.insert(struct_type);

            for (const auto& filed : struct_type->fields) {
                if (filed.type->type == ShaderBaseType::Struct) {
                    visitor(filed.type);
                }
            }

            builder.begin_struct(struct_type->name);
            emit_struct(struct_type);
            builder.end_struct();
        };

        // Collect all declarations and emit them first
        for (std::int16_t space_idx = 0; space_idx < get_num_spaces(); space_idx++) {
            const ShaderSpace& space = m_reflection.spaces[space_idx];

            for (std::int16_t binding_idx = 0; binding_idx < std::int16_t(space.bindings.size()); binding_idx++) {
                const ShaderBinding& binding = space.bindings[binding_idx];

                switch (binding.binding) {
                    case ShaderBindingType::InlineUniformBuffer:
                    case ShaderBindingType::UniformBuffer:
                    case ShaderBindingType::StorageBuffer:
                        for (const auto& filed : binding.type->fields) {
                            if (filed.type->type == ShaderBaseType::Struct) {
                                visitor(filed.type);
                            }
                        }
                        break;

                    default:
                        break;
                }
            }
        }

        for (std::int16_t space_idx = 0; space_idx < get_num_spaces(); space_idx++) {
            const ShaderSpace& space = m_reflection.spaces[space_idx];

            for (std::int16_t binding_idx = 0; binding_idx < std::int16_t(space.bindings.size()); binding_idx++) {
                const ShaderBinding& binding = space.bindings[binding_idx];

                switch (binding.binding) {
                    case ShaderBindingType::InlineUniformBuffer:
                    case ShaderBindingType::UniformBuffer:
                        builder.begin_uniform_binding(space_idx, binding_idx, binding.name, binding.qualifiers);
                        emit_struct(binding.type);
                        builder.end_uniform_binding();
                        break;

                    case ShaderBindingType::Sampler2d:
                        builder.add_sampler2d_binding(space_idx, binding_idx, binding.name);
                        break;

                    case ShaderBindingType::Sampler2dArray:
                        builder.add_sampler2dArray_binding(space_idx, binding_idx, binding.name);
                        break;

                    case ShaderBindingType::SamplerCube:
                        builder.add_samplerCube_binding(space_idx, binding_idx, binding.name);
                        break;

                    case ShaderBindingType::StorageImage2d:
                        builder.add_image_binding(space_idx, binding_idx, binding.name, binding.qualifiers);
                        break;

                    case ShaderBindingType::StorageBuffer:
                        builder.begin_storage_binding(space_idx, binding_idx, binding.name, binding.qualifiers);
                        emit_struct(binding.type);
                        builder.end_storage_binding();
                        break;

                    default:
                        return StatusCode::InvalidState;
                }
            }
        }

        out_declarations = builder.emit();

        return WG_OK;
    }

    Status Shader::fill_vertex_input(GfxShaderLang lang, const ShaderPermutation& permutation, std::string& out_input) {
        WG_AUTO_PROFILE_GRC("Shader::fill_vertex_input");

        assert(lang == GfxShaderLang::GlslVk450);

        GlslBuilder builder;

        int next_location = 0;

        permutation.vert_attribs.for_each([&](int idx, GfxVertAttrib attrib) {
            builder.add_vertex_input(next_location, GfxVertAttribGlslTypes[idx], std::string("in") + Enum::to_str(attrib));
            next_location += 1;
        });

        out_input = builder.emit();

        return WG_OK;
    }

    Status Shader::reanalyse_includes() {
        WG_AUTO_PROFILE_GRC("Shader::reanalyse_includes");

        FileSystem* fs = IocContainer::iresolve_v<FileSystem>();

        std::vector<ShaderInclude> new_includes;
        flat_set<Strid>            new_dependencies;

        for (ShaderSourceFile& source_file : m_reflection.sources) {
            GlslIncludeProcessor include_processor(m_env, *fs, false);

            if (!include_processor.parse_file(source_file.file)) {
                WG_LOG_ERROR("failed parse file " << source_file.file);
                return StatusCode::FailedParse;
            }

            for (const Strid& include : include_processor.get_includes()) {
                ShaderInclude& new_include = new_includes.emplace_back();
                new_include.module         = source_file.module;
                new_include.file           = include;
            }
        }

        for (ShaderInclude& new_include_file : new_includes) {
            new_dependencies.insert(new_include_file.file);
        }

        for (ShaderSourceFile& source_file : m_reflection.sources) {
            new_dependencies.insert(source_file.file);
        }

        std::swap(m_reflection.includes, new_includes);
        std::swap(m_reflection.dependencies, new_dependencies);

        return WG_OK;
    }

    Status Shader::load(const ShaderFile& file) {
        WG_AUTO_PROFILE_GRC("Shader::load");

        ShaderManager*  shader_manager  = IocContainer::iresolve_v<ShaderManager>();
        TextureManager* texture_manager = IocContainer::iresolve_v<TextureManager>();
        FileSystem*     file_system     = IocContainer::iresolve_v<FileSystem>();
        ShaderBuilder   builder;

        builder.set_name(file.name);
        builder.set_domain(file.domain);
        builder.add_ui_info(file.ui_name.empty() ? file.name.str() : file.ui_name, file.ui_hint);

        for (const auto& source : file.sources) {
            builder.add_source(SID(source.file), source.module, source.lang);
        }

        std::int16_t next_pad_idx   = 0;
        std::int16_t next_block_idx = 0;

        buffered_vector<std::int16_t> inline_params_idx;
        buffered_vector<Strid>        inline_params_names;
        buffered_vector<std::int16_t> inline_params_sizes;

        for (const auto& param_block : file.param_blocks) {
            bool         block_has_inline_params = false;
            std::int16_t block_size              = 0;

            for (const auto& param : param_block.params) {
                auto qtype = shader_manager->find_global_type(param.type);

                if (!qtype) {
                    WG_LOG_ERROR("failed to find param type " << param.type);
                    return StatusCode::InvalidData;
                }

                auto type = qtype.value();
                if (type->type == ShaderBaseType::Struct) {
                    builder.add_struct(type);
                }

                if (type->is_primitive) {
                    block_has_inline_params = true;

                    assert(param.elements == 1);
                    block_size += std::int16_t(Math::align(type->byte_size, sizeof(Vec4f)));
                }
            }

            inline_params_idx.push_back(block_has_inline_params ? next_block_idx : -1);
            inline_params_names.push_back(block_has_inline_params ? SID("InlineParamBlock" + StringUtils::from_int(next_block_idx)) : Strid());
            inline_params_sizes.push_back(block_size);
            next_block_idx++;
        }

        for (std::int16_t i = 0; i < std::int16_t(file.param_blocks.size()); i++) {
            const auto& param_block = file.param_blocks[i];

            if (inline_params_idx[i] == -1) {
                continue;
            }

            auto struct_bilder = builder.add_struct(inline_params_names[i], inline_params_sizes[i]);

            for (const auto& param : param_block.params) {
                auto type = shader_manager->find_global_type(param.type).value();
                if (!type->is_primitive) {
                    continue;
                }

                std::size_t field_size         = type->byte_size;
                std::size_t field_size_aligned = Math::align(type->byte_size, sizeof(Vec4f));

                struct_bilder.add_field(param.name, type, param.value.empty() ? Var() : Var(param.value));

                while (field_size < field_size_aligned) {
                    struct_bilder.add_field(SID("Padding" + StringUtils::from_int(next_pad_idx++)), type);
                    field_size += type->byte_size;
                }
            }

            struct_bilder.end_struct();
        }

        for (std::int16_t i = 0; i < std::int16_t(file.param_blocks.size()); i++) {
            const auto& param_block      = file.param_blocks[i];
            const bool  has_inline_block = inline_params_idx[i] != -1;

            auto space_builder = builder.add_space(param_block.name, param_block.type);

            if (has_inline_block) {
                space_builder.add_inline_uniform_buffer(inline_params_names[i], inline_params_names[i]);
            }

            for (const auto& param : param_block.params) {
                auto type = shader_manager->find_global_type(param.type).value();
                if (type->is_primitive) {
                    continue;
                }

                if (param.binding == ShaderBindingType::UniformBuffer) {
                    space_builder.add_uniform_buffer(param.name, param.type);
                    continue;
                }
                if (param.binding == ShaderBindingType::StorageBuffer) {
                    space_builder.add_storage_buffer(param.name, param.type);
                    continue;
                }
                if (type == ShaderTypes::SAMPLER2D) {
                    auto texture = Enum::parse<DefaultTexture>((std::string) param.value);
                    auto sampler = DefaultSampler::Default;
                    space_builder.add_texture_2d(param.name, texture_manager->get_texture(texture), texture_manager->get_sampler(sampler));
                    continue;
                }
                if (type == ShaderTypes::SAMPLER2D_ARRAY) {
                    space_builder.add_texture_2d_array(param.name, {}, {});
                    continue;
                }
                if (type == ShaderTypes::SAMPLER_CUBE) {
                    space_builder.add_texture_cube(param.name, {}, {});
                    continue;
                }
                if (type == ShaderTypes::IMAGE2D) {
                    space_builder.add_storage_image_2d(param.name);
                    continue;
                }

                WG_LOG_ERROR("unsupported type for shader script " << param.type);
                return StatusCode::InvalidData;
            }

            space_builder.end_space();
        }

        for (const auto& techique : file.techniques) {
            auto technique_builder = builder.add_technique(techique.name);

            technique_builder.add_ui_info(techique.ui_name.empty() ? techique.name.str() : techique.ui_name, techique.ui_hint);

            for (const auto& option : techique.options) {
                technique_builder.add_option(option.name, option.variants);
            }

            for (const auto& tag : techique.tags) {
                technique_builder.add_tag(tag.first, tag.second);
            }

            for (const auto& pass : techique.passes) {
                auto pass_builder = technique_builder.add_pass(pass.name);

                pass_builder.add_ui_info(pass.ui_name.empty() ? pass.name.str() : pass.ui_name, pass.ui_hint);

                for (const auto& option : pass.options) {
                    pass_builder.add_option(option.name, option.variants);
                }

                for (const auto& tag : pass.tags) {
                    pass_builder.add_tag(tag.first, tag.second);
                }

                pass_builder.add_state(pass.state);
                pass_builder.end_pass();
            }

            technique_builder.end_technique();
        }

        WG_CHECKED(builder.finish());
        WG_CHECKED(init(builder.get_reflection()));

        for (const auto& param_block : file.param_blocks) {
            for (const auto& param : param_block.params) {
                ShaderParamInfo& param_info = m_reflection.params_info[m_reflection.params_id[param.name]];
                param_info.ui_name          = param.ui_name.empty() ? param.name.str() : param.ui_name;
                param_info.ui_hint          = param.ui_hint;
            }
        }

        return WG_OK;
    }

    Ref<GfxShaderProgram> Shader::get_or_create_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        Ref<GfxShaderProgram> fast_lookup = find_program(platform, permutation);
        if (fast_lookup) {
            return fast_lookup;
        }

        std::unique_lock lock(m_mutex);
        ShaderProgram&   entry = m_cache.get_or_add_entry(platform, permutation);

        if (entry.status == ShaderStatus::InCompilation) {
            return entry.program;
        }
        if (entry.status == ShaderStatus::Failed) {
            return entry.program;
        }
        if (entry.status == ShaderStatus::InBytecode) {
            GfxShaderProgramHeader program_header;

            for (std::size_t i = 0; i < m_reflection.sources.size(); i++) {
                GfxShaderHeader& header = program_header.emplace_back();
                header.module_type      = m_reflection.sources[i].module;
                header.shader_hash      = entry.modules[i];
            }

            PsoCache* pso_cache = IocContainer::iresolve_v<PsoCache>();
            entry.program       = pso_cache->get_or_create_program(program_header, entry.name);
            if (entry.program) {
                entry.status = ShaderStatus::Compiled;
                WG_LOG_INFO("create from cache " << entry.name);
                return entry.program;
            }

            ShaderManager* shader_manager = IocContainer::iresolve_v<ShaderManager>();
            if (!shader_manager->is_compilation_enabled()) {
                entry.status = ShaderStatus::Failed;
                WG_LOG_INFO("failed to create " << entry.name);
            }

            entry.status = ShaderStatus::None;
            entry.modules.clear();
        }
        if (entry.status == ShaderStatus::None) {
            Ref<ShaderCompilerRequest> request;

            Async compilation_task = compile_program(platform, permutation, request);
            if (compilation_task.is_completed() && compilation_task.is_failed()) {
                entry.status = ShaderStatus::Failed;
                return entry.program;
            }

            Task cache_task(request->input.name, [permutation, platform, request, weak_self = WeakRef<Shader>(this)](TaskContext&) {
                WG_AUTO_PROFILE_GRC("Shader::cache_compiled_program");

                Ref<Shader> self = weak_self.acquire();
                if (!self) {
                    return 0;
                }

                std::unique_lock lock(self->m_mutex);
                ShaderProgram&   entry = self->m_cache.get_or_add_entry(platform, permutation);

                if (!request->output.status) {
                    return 1;
                }

                ShaderLibrary* shader_library = IocContainer::iresolve_v<ShaderLibrary>();
                std::size_t    num_modules    = request->output.bytecode.size();

                for (std::size_t i = 0; i < num_modules; i++) {
                    ShaderModule module;
                    module.name          = request->input.files[i].name;
                    module.module_type   = request->input.files[i].module_type;
                    module.bytecode      = request->output.bytecode[i];
                    module.bytecode_hash = request->output.bytecode_hashes[i];
                    module.source_hash   = request->output.source_hashes[i];

                    shader_library->fit_module(platform, module);
                    entry.modules.push_back(module.bytecode_hash);
                }

                return 0;
            });

            cache_task.set_task_manager(*IocContainer::iresolve_v<ShaderCompilerTaskManager>());
            auto cache_task_hnd = cache_task.schedule(compilation_task);

            cache_task_hnd.add_on_completion([platform, permutation, weak_self = WeakRef<Shader>(this)](AsyncStatus status, std::optional<int>&) {
                Ref<Shader> self = weak_self.acquire();
                if (!self) {
                    return;
                }

                std::unique_lock lock(self->m_mutex);
                ShaderProgram&   entry = self->m_cache.get_or_add_entry(platform, permutation);

                entry.compilation_task.reset();
                entry.status = status == AsyncStatus::Ok ? ShaderStatus::InBytecode : ShaderStatus::Failed;

                WG_LOG_INFO("finish compilation of entry " << entry.name << " status=" << Enum::to_str(entry.status));
            });

            entry.status           = ShaderStatus::InCompilation;
            entry.permutation      = permutation;
            entry.name             = request->input.name;
            entry.compilation_task = cache_task_hnd.as_async();
        }

        return entry.program;
    }

    Ref<GfxShaderProgram> Shader::find_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        std::shared_lock lock(m_mutex);

        auto query = m_cache.find_program(platform, permutation);
        if (!query) {
            return Ref<GfxShaderProgram>();
        }

        const ShaderProgram* program = *query;
        return program->program;
    }

    Async Shader::precache_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        WG_AUTO_PROFILE_GRC("Shader::precache_program");

        return Async();
    }

    Async Shader::compile_program(GfxShaderPlatform platform, const ShaderPermutation& permutation, Ref<ShaderCompilerRequest>& request, Async depends_on) {
        WG_AUTO_PROFILE_GRC("Shader::compile_program");

        Async result = Async::failed();

        if (platform == GfxShaderPlatform::None || platform == GfxShaderPlatform::Max) {
            WG_LOG_ERROR("cannot compile program for platfrom " << Enum::to_str(platform));
            return result;
        }

        ShaderManager*  shader_manager  = IocContainer::iresolve_v<ShaderManager>();
        ShaderCompiler* shader_compiler = shader_manager->find_compiler(platform);
        if (!shader_compiler) {
            WG_LOG_ERROR("no compiler found for platfrom " << Enum::to_str(platform));
            return result;
        }

        request = make_ref<ShaderCompilerRequest>();

        if (!fill_compiler_input(shader_compiler->get_lang(), platform, permutation, request->input)) {
            WG_LOG_ERROR("failed to get compiler input for " << request->input.name);
            return result;
        }

        return shader_compiler->compile(request, depends_on);
    }

    Status Shader::load_cache(GfxShaderPlatform platform, bool allow_missing) {
        WG_AUTO_PROFILE_GRC("Shader::load_cache");

        std::unique_lock lock(m_mutex);

        const std::string path = m_cache.make_cache_file_name(m_cache_path, get_shader_name().str(), platform);
        return m_cache.load_cache(path, platform, allow_missing);
    }

    Status Shader::save_cache(GfxShaderPlatform platform) {
        WG_AUTO_PROFILE_GRC("Shader::save_cache");

        std::unique_lock lock(m_mutex);

        const std::string path = m_cache.make_cache_file_name(m_cache_path, get_shader_name().str(), platform);
        return m_cache.save_cache(path, platform);
    }

    std::optional<ShaderStatus> Shader::find_program_status(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        std::shared_lock lock(m_mutex);

        auto query = m_cache.find_program(platform, permutation);
        if (!query) {
            return std::nullopt;
        }

        const ShaderProgram* program = *query;
        return program->status;
    }

    std::optional<std::int16_t> Shader::find_technique(Strid name) {
        auto r = m_reflection.techniques_map.find(name);
        return r != m_reflection.techniques_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    std::optional<std::int16_t> Shader::find_pass(std::int16_t technique, Strid name) {
        auto r = m_reflection.techniques[technique].passes_map.find(name);
        return r != m_reflection.techniques[technique].passes_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    ShaderParamId Shader::find_param_id(Strid name) {
        auto query = m_reflection.params_id.find(name);

        if (query != m_reflection.params_id.end()) {
            return ShaderParamId(query->second);
        }

        return ShaderParamId();
    }

    std::optional<ShaderParamInfo*> Shader::find_param(ShaderParamId id) {
        if (id.is_invalid()) {
            return std::nullopt;
        }
        if (id.index >= m_reflection.params_info.size()) {
            return std::nullopt;
        }

        return &m_reflection.params_info[id.index];
    }

    bool Shader::has_dependency(const Strid& dependency) const {
        return m_reflection.dependencies.find(dependency) != m_reflection.dependencies.end();
    }

    bool Shader::has_space(ShaderSpaceType space_type) const {
        for (const ShaderSpace& space : m_reflection.spaces) {
            if (space.type == space_type) {
                return true;
            }
        }
        return false;
    }

    bool Shader::has_option(std::int16_t technique, Strid name, Strid variant) const {
        if (m_reflection.techniques.size() <= technique) {
            return false;
        }

        const ShaderOptions& options = m_reflection.techniques[technique].options;

        auto option_query = options.options_map.find(name);
        if (option_query == options.options_map.end()) {
            return false;
        }

        auto variant_query = options.options[option_query->second].variants.find(variant);
        return variant_query != options.options[option_query->second].variants.end();
    }

    bool Shader::has_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) const {
        if (m_reflection.techniques.size() <= technique) {
            return false;
        }
        if (m_reflection.techniques[technique].passes.size() <= pass) {
            return false;
        }

        const ShaderOptions& options = m_reflection.techniques[technique].passes[pass].options;

        auto option_query = options.options_map.find(name);
        if (option_query == options.options_map.end()) {
            return false;
        }

        auto variant_query = options.options[option_query->second].variants.find(variant);
        return variant_query != options.options[option_query->second].variants.end();
    }

    const std::int16_t Shader::get_num_spaces() const {
        return std::int16_t(m_reflection.spaces.size());
    }

}// namespace wmoge