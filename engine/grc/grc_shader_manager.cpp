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

#include "grc_shader_manager.hpp"

#include "core/log.hpp"
#include "core/string_utils.hpp"
#include "debug/profiler.hpp"
#include "grc/grc_shader_script_builder.hpp"
#include "io/enum.hpp"
#include "math/math_utils.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    GrcShaderManager::GrcShaderManager() {
        WG_AUTO_PROFILE_GRC("GrcShaderManager::GrcShaderManager");

        m_texture_manager = IocContainer::instance()->resolve_v<GrcTextureManager>();
        m_file_system     = IocContainer::instance()->resolve_v<FileSystem>();
        m_gfx_driver      = IocContainer::instance()->resolve_v<GfxDriver>();
        m_console         = IocContainer::instance()->resolve_v<Console>();

        auto builtin_types = GrcShaderTypes::builtin();

        for (auto& type : builtin_types) {
            add_global_type(type);
        }

        m_shaders_folder = "root://shaders";
        m_file_system->add_rule({m_shaders_folder, "root://../shaders"});
    }

    Status GrcShaderManager::load_script(const GrcShaderScriptFile& file) {
        WG_AUTO_PROFILE_GRC("GrcShaderManager::load_script");

        GrcShaderScriptBuilder builder;

        builder.set_name(file.name);
        builder.add_ui_info(file.ui_name.empty() ? file.name.str() : file.ui_name, file.ui_hint);

        for (const auto& source : file.sources) {
            builder.add_source(SID(source.file), source.module);
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
                auto qtype = m_global_types.find(param.type);

                if (qtype == m_global_types.end()) {
                    WG_LOG_ERROR("failed to find param type " << param.type);
                    return StatusCode::InvalidData;
                }

                auto& type = qtype->second;
                if (type->type == GrcShaderBaseType::Struct) {
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
                auto& type = m_global_types.find(param.type)->second;
                if (!type->is_primitive) {
                    continue;
                }

                std::size_t field_size         = type->byte_size;
                std::size_t field_size_aligned = Math::align(type->byte_size, sizeof(Vec4f));

                struct_bilder.add_field(param.name, type, param.value.empty() ? Var() : Var(param.value));

                while (field_size < field_size_aligned) {
                    struct_bilder.add_field(SID("__pad" + StringUtils::from_int(next_pad_idx++)), type);
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
                auto& type = m_global_types.find(param.type)->second;
                if (type->is_primitive) {
                    continue;
                }

                if (param.binding == GrcShaderBindingType::UniformBuffer) {
                    space_builder.add_uniform_buffer(param.name, param.type);
                    continue;
                }
                if (param.binding == GrcShaderBindingType::StorageBuffer) {
                    space_builder.add_storage_buffer(param.name, param.type);
                    continue;
                }
                if (type == GrcShaderTypes::SAMPLER2D) {
                    auto texture = Enum::parse<GrcDefaultTexture>((std::string) param.value);
                    auto sampler = GrcDefaultSampler::Default;
                    space_builder.add_texture_2d(param.name, m_texture_manager->get_texture(texture), m_texture_manager->get_sampler(sampler));
                    continue;
                }
                if (type == GrcShaderTypes::SAMPLER2D_ARRAY) {
                    space_builder.add_texture_2d_array(param.name, {}, {});
                    continue;
                }
                if (type == GrcShaderTypes::SAMPLER_CUBE) {
                    space_builder.add_texture_cube(param.name, {}, {});
                    continue;
                }
                if (type == GrcShaderTypes::IMAGE2D) {
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

        Ref<GrcShaderScript> shader_script;

        const Status build_status = builder.finish(shader_script);
        if (!build_status) { return build_status; }

        for (const auto& param_block : file.param_blocks) {
            for (const auto& param : param_block.params) {
                GrcShaderReflection& reflection = shader_script->get_reflection();
                GrcShaderParamInfo&  param_info = reflection.params_info[reflection.params_id[param.name]];
                param_info.ui_name              = param.ui_name.empty() ? param.name.str() : param.ui_name;
                param_info.ui_hint              = param.ui_hint;
            }
        }

        const Status reload_status = shader_script->reload_sources(m_shaders_folder, m_file_system);
        if (!reload_status) { return reload_status; }

        return fit_script(shader_script);
    }

    Status GrcShaderManager::fit_script(const Ref<GrcShaderScript>& script) {
        WG_AUTO_PROFILE_GRC("GrcShaderManager::fit_script");

        std::lock_guard guard(m_mutex);

        auto q = m_scripts.find(script->get_name());
        if (q != m_scripts.end()) {
            WG_LOG_ERROR("clash in script name " << script->get_name());
            return StatusCode::Error;
        }

        m_scripts[script->get_name()] = script;
        return StatusCode::Ok;
    }

    Ref<GrcShaderScript> GrcShaderManager::find_script(Strid name) {
        std::lock_guard guard(m_mutex);

        auto q = m_scripts.find(name);
        return q != m_scripts.end() ? q->second : Ref<GrcShaderScript>();
    }

    void GrcShaderManager::add_global_type(const Ref<GrcShaderType>& type) {
        std::lock_guard guard(m_mutex);

        m_global_types[type->name] = type;
    }

    std::optional<Ref<GrcShaderType>> GrcShaderManager::find_global_type(Strid name) {
        std::lock_guard guard(m_mutex);

        auto q = m_global_types.find(name);
        if (q != m_global_types.end()) {
            return q->second;
        }

        return std::optional<Ref<GrcShaderType>>();
    }

}// namespace wmoge