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
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    Status Shader::from_reflection(ShaderReflection& reflection) {
        WG_AUTO_PROFILE_GRC("Shader::from_reflection");
        m_reflection = std::move(reflection);
        return WG_OK;
    }

    Status Shader::from_file(const ShaderFile& file) {
        WG_AUTO_PROFILE_GRC("Shader::from_file");

        ShaderManager*  shader_manager  = IocContainer::instance()->resolve_v<ShaderManager>();
        TextureManager* texture_manager = IocContainer::instance()->resolve_v<TextureManager>();
        FileSystem*     file_system     = IocContainer::instance()->resolve_v<FileSystem>();
        ShaderBuilder   builder;

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
                auto qtype = shader_manager->find_global_type(param.type);

                if (!qtype) {
                    WG_LOG_ERROR("failed to find param type " << param.type);
                    return StatusCode::InvalidData;
                }

                auto& type = qtype.value();
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
                auto& type = shader_manager->find_global_type(param.type).value();
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
                auto& type = shader_manager->find_global_type(param.type).value();
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

        Ref<Shader> shader{this};
        WG_CHECKED(builder.finish(shader));

        for (const auto& param_block : file.param_blocks) {
            for (const auto& param : param_block.params) {
                ShaderReflection& reflection = shader->get_reflection();
                ShaderParamInfo&  param_info = reflection.params_info[reflection.params_id[param.name]];
                param_info.ui_name           = param.ui_name.empty() ? param.name.str() : param.ui_name;
                param_info.ui_hint           = param.ui_hint;
            }
        }

        WG_CHECKED(reload_sources(shader_manager->get_shaders_folder(), file_system));

        return shader_manager->fit_shader(shader);
    }

    std::optional<std::int16_t> Shader::find_technique(Strid name) {
        auto r = m_reflection.techniques_map.find(name);
        return r != m_reflection.techniques_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    std::optional<std::int16_t> Shader::find_pass(std::int16_t technique, Strid name) {
        auto r = m_reflection.techniques[technique].passes_map.find(name);
        return r != m_reflection.techniques[technique].passes_map.end() ? std::optional<std::int16_t>(r->second) : std::nullopt;
    }

    ShaderParamId Shader::get_param_id(Strid name) {
        auto query = m_reflection.params_id.find(name);

        if (query != m_reflection.params_id.end()) {
            return ShaderParamId(query->second);
        }

        return ShaderParamId();
    }

    std::optional<ShaderParamInfo*> Shader::get_param_info(ShaderParamId id) {
        if (id.is_invalid()) {
            return std::nullopt;
        }
        if (id.index >= m_reflection.params_info.size()) {
            return std::nullopt;
        }

        return &m_reflection.params_info[id.index];
    }

    Status Shader::reload_sources(const std::string& folder, FileSystem* fs) {
        WG_AUTO_PROFILE_GRC("Shader::reload_sources");

        buffered_vector<ShaderInclude>    new_includes;
        buffered_vector<ShaderSourceFile> new_sources;
        flat_set<Strid>                   new_dependencies;

        for (ShaderSourceFile& source_file : m_reflection.sources) {
            GlslIncludeProcessor include_processor(folder, fs);

            if (!include_processor.parse_file(source_file.name)) {
                WG_LOG_ERROR("failed parse file " << source_file.name);
                return StatusCode::FailedParse;
            }

            for (const Strid& include : include_processor.get_includes()) {
                ShaderInclude& new_include = new_includes.emplace_back();
                new_include.module         = source_file.module;
                new_include.name           = include;
            }

            ShaderSourceFile& new_source_file = new_sources.emplace_back();
            new_source_file.name              = source_file.name;
            new_source_file.module            = source_file.module;
            new_source_file.content           = std::move(include_processor.get_result());
        }

        for (ShaderInclude& new_include_file : new_includes) {
            new_dependencies.insert(new_include_file.name);
        }

        for (ShaderSourceFile& new_source_file : new_sources) {
            new_dependencies.insert(new_source_file.name);
        }

        std::swap(m_reflection.includes, new_includes);
        std::swap(m_reflection.sources, new_sources);
        std::swap(m_reflection.dependencies, new_dependencies);

        return StatusCode::Ok;
    }

    Status Shader::fill_layout(GfxDescSetLayoutDesc& desc, int space) const {
        assert(0 <= space && space < m_reflection.spaces.size());

        const ShaderSpace& shader_space = m_reflection.spaces[space];
        assert(shader_space.type != ShaderSpaceType::Material);

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

        return StatusCode::Ok;
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

}// namespace wmoge