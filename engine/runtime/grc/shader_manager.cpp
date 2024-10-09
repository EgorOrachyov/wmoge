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

#include "shader_manager.hpp"

#include "core/ioc_container.hpp"
#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "core/task.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_builder.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "io/enum.hpp"
#include "math/math_utils.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "rtti/type_storage.hpp"
#include "system/console.hpp"

#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <sstream>

namespace wmoge {

    ShaderManager::ShaderManager(IocContainer* ioc) {
        WG_AUTO_PROFILE_GRC("ShaderManager::ShaderManager");

        m_task_manager    = ioc->resolve_value<ShaderTaskManager>();
        m_file_system     = ioc->resolve_value<FileSystem>();
        m_gfx_driver      = ioc->resolve_value<GfxDriver>();
        m_texture_manager = ioc->resolve_value<TextureManager>();
        m_shader_library  = ioc->resolve_value<ShaderLibrary>();
        m_pso_cache       = ioc->resolve_value<PsoCache>();
        m_console         = ioc->resolve_value<Console>();

        m_shaders_folder          = "engine/shaders";
        m_shaders_cache_path      = "cache/";
        m_active_platform         = m_gfx_driver->get_shader_platform();
        m_compilation_enable      = true;
        m_load_cache              = true;
        m_save_cache              = true;
        m_hot_reload_enable       = true;
        m_hot_reload_on_change    = true;
        m_hot_reload_on_trigger   = true;
        m_hot_reload_interval_sec = 5.0f;
        m_callback                = std::make_shared<Shader::Callback>([this](Shader* shader) { remove_shader(shader); });

        auto builtin_types = ShaderTypes::builtin();
        for (auto& type : builtin_types) {
            add_global_type(type);
        }

        m_compiler_options = ShaderCompilerOptions();// debug options
        m_compiler_env.path_includes.insert(m_shaders_folder);
    }

    Status ShaderManager::load_shader_reflection(const ShaderFile& file, ShaderReflection& reflection) {
        WG_AUTO_PROFILE_GRC("ShaderManager::load_shader");

        ShaderBuilder builder;

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
                auto qtype = find_global_type(param.type);

                if (!qtype) {
                    WG_LOG_ERROR("failed to find param type " << param.type);
                    return StatusCode::InvalidData;
                }

                auto type = qtype.value();
                if (type->type == ShaderBaseType::Struct) {
                    builder.add_struct(type.cast<ShaderTypeStruct>());
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
                auto type = find_global_type(param.type).value();
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
                auto type = find_global_type(param.type).value();
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
                    space_builder.add_texture_2d(param.name, m_texture_manager->get_texture(texture), m_texture_manager->get_sampler(sampler));
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

        reflection = std::move(builder.get_reflection());

        for (const auto& param_block : file.param_blocks) {
            for (const auto& param : param_block.params) {
                ShaderParamInfo& param_info = reflection.params_info[reflection.params_id[param.name]];
                param_info.ui_name          = param.ui_name.empty() ? param.name.str() : param.ui_name;
                param_info.ui_hint          = param.ui_hint;
            }
        }

        return WG_OK;
    }

    void ShaderManager::add_shader(const Ref<Shader>& shader) {
        assert(!has_shader(shader.get()));

        shader->set_shader_callback(m_callback);

        auto& entry = add_entry(shader);
        {
            std::unique_lock lock(entry.mutex);

            for (std::int16_t space_idx = 0; space_idx < shader->get_num_spaces(); space_idx++) {
                GfxDescSetLayoutDesc layout_desc;
                shader->fill_layout_desc(layout_desc, space_idx);
                Ref<GfxDescSetLayout> layout = m_pso_cache->get_or_create_desc_layout(layout_desc);
                entry.layouts.push_back(std::move(layout));
            }

            entry.pso_layout = m_pso_cache->get_or_create_pso_layout(entry.layouts);
        }

        if (m_load_cache) {
            load_cache(shader.get(), m_active_platform, true);
        }
    }

    void ShaderManager::remove_shader(Shader* shader) {
        assert(has_shader(shader));

        if (m_save_cache) {
            save_cache(shader, m_active_platform);
        }

        remove_entry(shader);
    }

    bool ShaderManager::has_shader(Shader* shader) {
        return get_entry(shader) != nullptr;
    }

    const GfxDescSetLayoutRef& ShaderManager::get_shader_layout(Shader* shader, std::int16_t space) {
        auto& entry = get_entry_ref(shader);
        return entry.layouts[space];
    }

    const GfxPsoLayoutRef& ShaderManager::get_shader_pso_layout(Shader* shader) {
        auto& entry = get_entry_ref(shader);
        return entry.pso_layout;
    }

    Ref<GfxShaderProgram> ShaderManager::get_or_create_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        WG_AUTO_PROFILE_GRC("ShaderManager::get_or_create_program");

        Ref<GfxShaderProgram> fast_lookup = find_program(shader, platform, permutation);
        if (fast_lookup) {
            return fast_lookup;
        }

        auto&            shader_entry = get_entry_ref(shader);
        std::unique_lock lock(shader_entry.mutex);

        ShaderProgram& entry = shader_entry.cache.get_or_add_entry(platform, permutation);

        if (entry.status == ShaderStatus::InCompilation) {
            return entry.program;
        }
        if (entry.status == ShaderStatus::Failed) {
            return entry.program;
        }
        if (entry.status == ShaderStatus::InBytecode) {
            const ShaderReflection& reflection = shader->get_reflection();

            GfxShaderProgramHeader program_header;
            for (std::size_t i = 0; i < reflection.sources.size(); i++) {
                GfxShaderHeader& header = program_header.emplace_back();
                header.module_type      = reflection.sources[i].module;
                header.shader_hash      = entry.modules[i];
            }

            entry.program = m_pso_cache->get_or_create_program(program_header, entry.name);
            if (entry.program) {
                entry.status = ShaderStatus::Compiled;
                WG_LOG_INFO("create from cache " << entry.name);
                return entry.program;
            }

            if (!is_compilation_enabled()) {
                entry.status = ShaderStatus::Failed;
                WG_LOG_INFO("failed to create " << entry.name << ", compilation disabled");
            }

            entry.status = ShaderStatus::None;
            entry.modules.clear();
        }
        if (entry.status == ShaderStatus::None) {
            Ref<ShaderCompilerRequest> request;

            Async compilation_task = compile_program(shader, platform, permutation, request);
            if (compilation_task.is_completed() && compilation_task.is_failed()) {
                entry.status = ShaderStatus::Failed;
                return entry.program;
            }

            Task cache_task(request->input.name, [shader_library = m_shader_library, permutation, platform, request, weak_shader_entry = WeakRef<Entry>(&shader_entry)](TaskContext&) {
                WG_AUTO_PROFILE_GRC("Shader::cache_compiled_program");

                Ref<Entry> shader_entry = weak_shader_entry.acquire();
                if (!shader_entry) {
                    return 0;
                }

                std::unique_lock lock(shader_entry->mutex);
                ShaderProgram&   entry = shader_entry->cache.get_or_add_entry(platform, permutation);

                if (!request->output.status) {
                    return 1;
                }

                std::size_t num_modules = request->output.bytecode.size();

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

            auto cache_task_hnd = cache_task.schedule(m_task_manager, compilation_task);

            cache_task_hnd.add_on_completion([platform, permutation, weak_shader_entry = WeakRef<Entry>(&shader_entry)](AsyncStatus status, std::optional<int>&) {
                Ref<Entry> shader_entry = weak_shader_entry.acquire();
                if (!shader_entry) {
                    return 0;
                }

                std::unique_lock lock(shader_entry->mutex);
                ShaderProgram&   entry = shader_entry->cache.get_or_add_entry(platform, permutation);

                entry.compilation_task.reset();
                entry.status = status == AsyncStatus::Ok ? ShaderStatus::InBytecode : ShaderStatus::Failed;

                WG_LOG_INFO("finish compilation of entry " << entry.name << " status=" << Enum::to_str(entry.status));
                return 0;
            });

            entry.status           = ShaderStatus::InCompilation;
            entry.permutation      = permutation;
            entry.name             = request->input.name;
            entry.compilation_task = cache_task_hnd.as_async();
        }

        return entry.program;
    }

    Ref<GfxShaderProgram> ShaderManager::find_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        auto&            entry = get_entry_ref(shader);
        std::shared_lock lock(entry.mutex);

        auto query = entry.cache.find_program(platform, permutation);
        if (!query) {
            return Ref<GfxShaderProgram>();
        }

        return query.value()->program;
    }

    std::optional<ShaderStatus> ShaderManager::find_program_status(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        auto&            entry = get_entry_ref(shader);
        std::shared_lock lock(entry.mutex);

        auto query = entry.cache.find_program(platform, permutation);
        if (!query) {
            return std::nullopt;
        }

        return query.value()->status;
    }

    Async ShaderManager::precache_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation) {
        WG_AUTO_PROFILE_GRC("ShaderManager::precache_program");

        return Async();
    }

    Async ShaderManager::compile_program(Shader* shader, GfxShaderPlatform platform, const ShaderPermutation& permutation, Ref<ShaderCompilerRequest>& request, Async depends_on) {
        WG_AUTO_PROFILE_GRC("ShaderManager::compile_program");

        Async result = Async::failed();

        if (platform == GfxShaderPlatform::None || platform == GfxShaderPlatform::Max) {
            WG_LOG_ERROR("cannot compile program for platfrom " << Enum::to_str(platform));
            return result;
        }

        ShaderCompiler* shader_compiler = find_compiler(platform);
        if (!shader_compiler) {
            WG_LOG_ERROR("no compiler found for platfrom " << Enum::to_str(platform));
            return result;
        }

        request                = make_ref<ShaderCompilerRequest>();
        request->input.env     = m_compiler_env;
        request->input.options = m_compiler_options;

        if (!shader->fill_compiler_input(shader_compiler->get_lang(), platform, permutation, shader_compiler, request->input)) {
            WG_LOG_ERROR("failed to get compiler input for " << request->input.name);
            return result;
        }

        return shader_compiler->compile(request, depends_on);
    }

    Status ShaderManager::load_cache(Shader* shader, GfxShaderPlatform platform, bool allow_missing) {
        WG_AUTO_PROFILE_GRC("ShaderManager::load_cache");

        auto&            entry = get_entry_ref(shader);
        std::unique_lock lock(entry.mutex);

        const std::string path = entry.cache.make_cache_file_name(m_shaders_cache_path, shader->get_shader_name().str(), platform);
        return entry.cache.load_cache(m_file_system, path, platform, allow_missing);
    }

    Status ShaderManager::save_cache(Shader* shader, GfxShaderPlatform platform) {
        WG_AUTO_PROFILE_GRC("ShaderManager::save_cache");

        auto&            entry = get_entry_ref(shader);
        std::unique_lock lock(entry.mutex);

        const std::string path = entry.cache.make_cache_file_name(m_shaders_cache_path, shader->get_shader_name().str(), platform);
        return entry.cache.save_cache(m_file_system, path, platform);
    }

    std::optional<Ref<ShaderType>> ShaderManager::find_global_type(Strid name) {
        auto q = m_global_types.find(name);
        if (q != m_global_types.end()) {
            return q->second;
        }
        return std::optional<Ref<ShaderType>>();
    }

    bool ShaderManager::is_global_type(Strid name) {
        auto q = m_global_types.find(name);
        return q != m_global_types.end();
    }

    ShaderCompiler* ShaderManager::find_compiler(GfxShaderPlatform platform) {
        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_compilers[int(platform)].get();
    }

    void ShaderManager::add_compiler(Ref<ShaderCompiler> compiler) {
        assert(compiler);
        assert(int(compiler->get_platform()) < GfxLimits::NUM_PLATFORMS);
        assert(!m_compilers[int(compiler->get_platform())]);
        m_compilers[int(compiler->get_platform())] = std::move(compiler);
    }

    void ShaderManager::add_global_type(const Ref<ShaderType>& type) {
        m_global_types[type->name] = type;
    }

    ShaderManager::Entry* ShaderManager::get_entry(Shader* shader) {
        std::shared_lock lock(m_mutex);

        auto query = m_shaders.find(shader);
        return query != m_shaders.end() ? query->second.get() : nullptr;
    }

    ShaderManager::Entry& ShaderManager::get_entry_ref(Shader* shader) {
        return *get_entry(shader);
    }

    ShaderManager::Entry& ShaderManager::add_entry(const Ref<Shader>& shader) {
        std::unique_lock lock(m_mutex);

        assert(shader);

        auto& entry     = m_shaders[shader.get()];
        entry           = make_ref<Entry>();
        entry->weak_ref = shader;

        return *entry;
    }

    void ShaderManager::remove_entry(Shader* shader) {
        std::unique_lock lock(m_mutex);

        m_shaders.erase(shader);
    }

}// namespace wmoge