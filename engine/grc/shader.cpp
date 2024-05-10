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

#include "glsl/glsl_include_processor.hpp"
#include "grc/shader_builder.hpp"
#include "grc/shader_cache.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

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

}// namespace wmoge
