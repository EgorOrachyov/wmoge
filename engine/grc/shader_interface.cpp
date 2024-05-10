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

#include "shader_interface.hpp"

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
#include "system/ioc_container.hpp"

#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace wmoge {

    Status ShaderInterface::init(ShaderReflection& reflection) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::from_reflection");

        m_reflection = std::move(reflection);

        PsoCache*      pso_cache      = IocContainer::iresolve_v<PsoCache>();
        ShaderManager* shader_manager = IocContainer::iresolve_v<ShaderManager>();

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

        return reanalyse_includes();
    }

    Status ShaderInterface::fill_layout_desc(GfxDescSetLayoutDesc& desc, std::int16_t space) const {
        WG_AUTO_PROFILE_GRC("ShaderInterface::fill_layout_desc");

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

    Status ShaderInterface::fill_layouts(GfxDescSetLayouts& layouts) const {
        layouts = m_layouts;
        return WG_OK;
    }

    Status ShaderInterface::fill_compiler_env(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompilerEnv& compiler_env) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::fill_compiler_env");

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

    Status ShaderInterface::fill_compiler_input(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, ShaderCompilerInput& compiler_input) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::fill_compiler_input");

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

    Status ShaderInterface::fill_program_name(GfxShaderLang lang, GfxShaderPlatform platform, const ShaderPermutation& permutation, std::string& name) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::fill_program_name");

        std::stringstream stream;

        stream << "shader=" << get_shader_name() << " ";
        stream << "permutation=" << permutation.hash() << " ";
        stream << "technique=" << permutation.technique_idx << " ";
        stream << "pass=" << permutation.pass_idx << " ";
        stream << "vf=" << permutation.vert_attribs.to_string() << " ";
        stream << "options: ";

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

    Status ShaderInterface::fill_option_info(std::int16_t technique_idx, std::int16_t permutation_bit, Strid& option, Strid& variant) {
        ShaderTechniqueInfo& technique = m_reflection.techniques[technique_idx];
        option                         = technique.options_remap[permutation_bit];
        variant                        = technique.variants_remap[permutation_bit];
        return WG_OK;
    }

    Status ShaderInterface::fill_declarations(GfxShaderLang lang, std::string& out_declarations) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::fill_declarations");

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

    Status ShaderInterface::fill_vertex_input(GfxShaderLang lang, const ShaderPermutation& permutation, std::string& out_input) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::fill_vertex_input");

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

    Status ShaderInterface::reanalyse_includes() {
        WG_AUTO_PROFILE_GRC("ShaderInterface::reanalyse_includes");

        FileSystem* fs = IocContainer::iresolve_v<FileSystem>();

        buffered_vector<ShaderInclude> new_includes;
        flat_set<Strid>                new_dependencies;

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

    Ref<GfxShaderProgram> ShaderInterface::get_or_create_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        Ref<GfxShaderProgram> fast_lookup = find_program(platform, permutation);
        if (fast_lookup) {
            return fast_lookup;
        }

        std::unique_lock lock(m_mutex);
        ShaderProgram&   entry = m_cache.get_or_add_entry(platform, permutation);

        if (entry.status == ShaderStatus::InBytecode) {
            GfxShaderProgramHeader program_header;

            for (std::size_t i = 0; i < m_reflection.sources.size(); i++) {
                GfxShaderHeader& header = program_header.emplace_back();
                header.module_type      = m_reflection.sources[i].module;
                header.shader_hash      = entry.modules[i];
            }

            PsoCache* pso_cache = IocContainer::iresolve_v<PsoCache>();
            entry.program       = pso_cache->get_or_create_program(program_header, entry.name);
            if (!entry.program) {
                entry.status = ShaderStatus::Failed;
                return entry.program;
            }

            entry.status = ShaderStatus::Compiled;
        }

        if (entry.status == ShaderStatus::None) {
            Ref<ShaderCompilerRequest> request;

            Async compilation_task = compile_program(platform, permutation, request);
            if (compilation_task.is_completed() && compilation_task.is_failed()) {
                entry.status = ShaderStatus::Failed;
                return entry.program;
            }

            Task cache_task(request->input.name, [permutation, platform, request, self = Ref<ShaderInterface>(this)](TaskContext&) {
                WG_AUTO_PROFILE_GRC("ShaderInterface::cache_compiled_program");

                std::unique_lock lock(self->m_mutex);
                ShaderProgram&   entry = self->m_cache.get_or_add_entry(platform, permutation);

                if (!request->output.status) {
                    entry.status = ShaderStatus::Failed;
                    return 0;
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

            cache_task_hnd.add_on_completion([platform, permutation, self = Ref<ShaderInterface>(this)](AsyncStatus status, std::optional<int>&) {
                std::unique_lock lock(self->m_mutex);
                ShaderProgram&   entry = self->m_cache.get_or_add_entry(platform, permutation);

                entry.compilation_task.reset();
                entry.status = status == AsyncStatus::Ok ? ShaderStatus::InBytecode : ShaderStatus::Failed;
            });

            entry.status           = ShaderStatus::InCompilation;
            entry.permutation      = permutation;
            entry.name             = request->input.name;
            entry.compilation_task = cache_task_hnd.as_async();
        }

        return entry.program;
    }

    Ref<GfxShaderProgram> ShaderInterface::find_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        std::shared_lock lock(m_mutex);

        auto query = m_cache.find_program(platform, permutation);
        if (!query) {
            return Ref<GfxShaderProgram>();
        }

        const ShaderProgram* program = *query;
        return program->program;
    }

    Async ShaderInterface::precache_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::precache_program");

        return Async();
    }

    Async ShaderInterface::compile_program(GfxShaderPlatform platform, const ShaderPermutation& permutation, Ref<ShaderCompilerRequest>& request, Async depends_on) {
        WG_AUTO_PROFILE_GRC("ShaderInterface::compile_program");

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

    std::optional<ShaderStatus> ShaderInterface::find_program_status(GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        std::shared_lock lock(m_mutex);

        auto query = m_cache.find_program(platform, permutation);
        if (!query) {
            return std::nullopt;
        }

        const ShaderProgram* program = *query;
        return program->status;
    }

    std::optional<std::int16_t> ShaderInterface::find_technique(Strid name) {
        auto r = m_reflection.techniques_map.find(name);
        return r != m_reflection.techniques_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    std::optional<std::int16_t> ShaderInterface::find_pass(std::int16_t technique, Strid name) {
        auto r = m_reflection.techniques[technique].passes_map.find(name);
        return r != m_reflection.techniques[technique].passes_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    ShaderParamId ShaderInterface::find_param_id(Strid name) {
        auto query = m_reflection.params_id.find(name);

        if (query != m_reflection.params_id.end()) {
            return ShaderParamId(query->second);
        }

        return ShaderParamId();
    }

    std::optional<ShaderParamInfo*> ShaderInterface::find_param(ShaderParamId id) {
        if (id.is_invalid()) {
            return std::nullopt;
        }
        if (id.index >= m_reflection.params_info.size()) {
            return std::nullopt;
        }

        return &m_reflection.params_info[id.index];
    }

    bool ShaderInterface::has_dependency(const Strid& dependency) const {
        return m_reflection.dependencies.find(dependency) != m_reflection.dependencies.end();
    }

    bool ShaderInterface::has_space(ShaderSpaceType space_type) const {
        for (const ShaderSpace& space : m_reflection.spaces) {
            if (space.type == space_type) {
                return true;
            }
        }
        return false;
    }

    bool ShaderInterface::has_option(std::int16_t technique, Strid name, Strid variant) const {
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

    bool ShaderInterface::has_option(std::int16_t technique, std::int16_t pass, Strid name, Strid variant) const {
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

    const std::int16_t ShaderInterface::get_num_spaces() const {
        return std::int16_t(m_reflection.spaces.size());
    }

}// namespace wmoge