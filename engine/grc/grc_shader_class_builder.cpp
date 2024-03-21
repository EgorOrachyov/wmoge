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

#include "grc_shader_class_builder.hpp"

#include "core/string_utils.hpp"

namespace wmoge {

    GrcShaderClassBuilder::StructBuilder::StructBuilder(GrcShaderClassBuilder& owner, Ref<GrcShaderType> struct_type)
        : m_owner(owner), m_struct_type(struct_type) {
    }

    GrcShaderClassBuilder::StructBuilder& GrcShaderClassBuilder::StructBuilder::add_field(Strid name, Strid struct_type) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = m_owner.m_reflection.declarations[struct_type];
        field.offset                = field.type->byte_size;
        return *this;
    }

    GrcShaderClassBuilder::StructBuilder& GrcShaderClassBuilder::StructBuilder::add_field(Strid name, Ref<GrcShaderType> type, Var value) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = type;
        field.default_value         = value;
        field.offset                = field.type->byte_size;
        return *this;
    }

    GrcShaderClassBuilder::StructBuilder& GrcShaderClassBuilder::StructBuilder::add_field_array(Strid name, Strid struct_type, int n_elements) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = m_owner.m_reflection.declarations[struct_type];
        field.is_array              = true;
        field.n_array_elem          = n_elements;
        field.offset                = n_elements * field.type->byte_size;
        return *this;
    }

    GrcShaderClassBuilder::StructBuilder& GrcShaderClassBuilder::StructBuilder::add_field_array(Strid name, Ref<GrcShaderType> type, Var value, int n_elements) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = type;
        field.default_value         = value;
        field.is_array              = true;
        field.n_array_elem          = n_elements;
        field.offset                = n_elements * field.type->byte_size;
        return *this;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::StructBuilder::end_struct() {
        return m_owner;
    }

    GrcShaderClassBuilder::SpaceBuilder::SpaceBuilder(GrcShaderClassBuilder& owner, GrcShaderSpace& space)
        : m_owner(owner), m_space(space) {
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_inline_uniform_buffer(Strid name, Strid type_struct) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::InlineUniformBuffer;
        binding.type              = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers.std140 = true;
        return *this;
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_uniform_buffer(Strid name, Strid type_struct) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::UniformBuffer;
        binding.type              = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers.std140 = true;
        return *this;
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_texture_2d(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::Sampler2d;
        binding.type              = GrcShaderTypes::SAMPLER2D;
        binding.default_tex       = texture;
        binding.default_sampler   = sampler;
        return *this;
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_texture_2d_array(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::Sampler2dArray;
        binding.type              = GrcShaderTypes::SAMPLER2D_ARRAY;
        binding.default_tex       = texture;
        binding.default_sampler   = sampler;
        return *this;
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_texture_cube(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::SamplerCube;
        binding.type              = GrcShaderTypes::SAMPLER_CUBE;
        binding.default_tex       = texture;
        binding.default_sampler   = sampler;
        return *this;
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_storage_buffer(Strid name, Strid type_struct) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::StorageBuffer;
        binding.type              = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers.std430 = true;
        return *this;
    }

    GrcShaderClassBuilder::SpaceBuilder& GrcShaderClassBuilder::SpaceBuilder::add_storage_image_2d(Strid name) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::StorageImage2d;
        return *this;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::SpaceBuilder::end_space() {
        return m_owner;
    }

    GrcShaderClassBuilder::PassBuilder::PassBuilder(GrcShaderClassBuilder& owner, GrcShaderPass& pass)
        : m_owner(owner), m_pass(pass) {
    }

    GrcShaderClassBuilder::PassBuilder& GrcShaderClassBuilder::PassBuilder::add_option(Strid name, fast_vector<Strid> variants) {
        GrcShaderOption& option = m_pass.options.emplace_back();
        option.name             = name;
        option.variants         = std::move(variants);
        option.mappings.resize(option.variants.size());

        m_owner.m_reflection.options_map[name] = m_owner.m_next_option_idx++;

        for (int i = 0; i < int(option.variants.size()); i++) {
            option.mappings[i]                                    = m_owner.m_next_variant_idx++;
            m_owner.m_reflection.variants_map[option.variants[i]] = option.mappings[i];
        }

        return *this;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::PassBuilder::end_pass() {
        return m_owner;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::set_name(Strid name) {
        m_reflection.shader_name = name;
        return *this;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::add_source(Strid file, GfxShaderModule module) {
        GrcShaderSourceFile& source_file = m_reflection.sources.emplace_back();
        source_file.name                 = file;
        source_file.module               = module;
        return *this;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::add_constant(Strid name, Var value) {
        GrcShaderConstant& constant = m_reflection.constants.emplace_back();
        constant.name               = name;
        constant.str                = value.to_string();
        constant.value              = std::move(value);
        return *this;
    }

    GrcShaderClassBuilder& GrcShaderClassBuilder::add_option(Strid name, fast_vector<Strid> variants) {
        GrcShaderOption& option = m_reflection.options.emplace_back();
        option.name             = name;
        option.variants         = std::move(variants);
        option.mappings.resize(option.variants.size());

        m_reflection.options_map[name] = m_next_option_idx++;

        for (int i = 0; i < int(option.variants.size()); i++) {
            option.mappings[i]                            = m_next_variant_idx++;
            m_reflection.variants_map[option.variants[i]] = option.mappings[i];
        }

        return *this;
    }

    GrcShaderClassBuilder::StructBuilder GrcShaderClassBuilder::add_struct(Strid name, int byte_size) {
        Ref<GrcShaderType> struct_type  = make_ref<GrcShaderType>();
        struct_type->name               = name;
        struct_type->type               = GrcShaderBaseType::Struct;
        struct_type->byte_size          = byte_size;
        m_reflection.declarations[name] = struct_type;
        return StructBuilder(*this, struct_type);
    }

    GrcShaderClassBuilder::SpaceBuilder GrcShaderClassBuilder::add_space(Strid name, GrcShaderSpaceType type) {
        GrcShaderSpace& space = m_reflection.spaces.emplace_back();
        space.name            = name;
        space.type            = type;
        return SpaceBuilder(*this, space);
    }

    GrcShaderClassBuilder::PassBuilder GrcShaderClassBuilder::add_pass(Strid name) {
        GrcShaderPass& pass           = m_reflection.passes.emplace_back();
        pass.name                     = name;
        m_reflection.passes_map[name] = m_next_pass_idx++;
        return PassBuilder(*this, pass);
    }

    Status GrcShaderClassBuilder::finish(std::shared_ptr<GrcShaderClass>& shader_class) {
        for (const auto& [name, type] : m_reflection.declarations) {
            int byte_size = 0;

            for (const auto& f : type->fields) {
                byte_size += f.offset;
            }

            if (byte_size != type->byte_size) {
                WG_LOG_ERROR("invalid size for type "
                             << name
                             << " exp=" << type->byte_size
                             << " actual=" << byte_size
                             << " in " << m_reflection.shader_name);
                return StatusCode::Error;
            }

            if ((byte_size % int(sizeof(Vec4f))) != 0) {
                WG_LOG_ERROR("invalid alignment of type "
                             << name
                             << " size=" << byte_size
                             << " in " << m_reflection.shader_name);
                return StatusCode::Error;
            }
        }

        std::int16_t space_idx  = 0;
        std::int16_t buffer_idx = 0;

        for (const auto& space : m_reflection.spaces) {
            std::int16_t binding_idx = 0;

            for (const auto& binding : space.bindings) {
                switch (binding.binding) {
                    case GrcShaderBindingType::InlineUniformBuffer: {
                        GrcShaderBufferInfo& buffer = m_reflection.buffers.emplace_back();
                        buffer.space                = space_idx;
                        buffer.binding              = binding_idx;

                        std::int16_t offset = 0;

                        for (const auto& field : binding.type->fields) {
                            if (field.is_array && field.n_array_elem == 0) {
                                WG_LOG_ERROR("no size array not allowed in "
                                             << " name=" << binding.name
                                             << " in " << m_reflection.shader_name);
                                return StatusCode::Error;
                            }

                            const std::string param_name_base = binding.name.str() + "." + field.name.str();
                            const int         params_to_add   = field.is_array ? field.n_array_elem : 1;

                            const Var& default_var = field.default_value;
                            Array      default_vars;

                            if (field.is_array) {
                                default_vars = std::move(default_var.operator wmoge::Array());
                            }

                            for (int i = 0; i < params_to_add; i++) {
                                std::string param_name = param_name_base;

                                if (field.is_array) {
                                    param_name += "[" + StringUtils::from_int(i) + "]";
                                }

                                GrcShaderParamInfo& param = m_reflection.params_info.emplace_back();
                                param.name                = SID(param_name);
                                param.type                = field.type;
                                param.space               = space_idx;
                                param.binding             = binding_idx;
                                param.buffer              = buffer_idx;
                                param.offset              = offset;
                                param.default_var         = i < default_vars.size() ? default_vars[i] : default_var;
                                param.default_value_str   = param.default_var.to_string();
                                param.binding_type        = binding.binding;

                                offset += field.type->byte_size;
                            }
                        }

                        if (offset == 0) {
                            WG_LOG_ERROR("empty inline uniform buffer not allowed "
                                         << " name=" << binding.name
                                         << " in " << m_reflection.shader_name);
                            return StatusCode::Error;
                        }

                        if (offset != binding.type->byte_size) {
                            WG_LOG_ERROR("error in params layout "
                                         << " layout size=" << offset
                                         << " actual size=" << binding.type->byte_size
                                         << " in " << m_reflection.shader_name);
                            return StatusCode::Error;
                        }

                        buffer.size = offset;
                        buffer_idx++;
                        break;
                    }

                    case GrcShaderBindingType::Sampler2d:
                    case GrcShaderBindingType::Sampler2dArray:
                    case GrcShaderBindingType::SamplerCube: {
                        GrcShaderParamInfo& param = m_reflection.params_info.emplace_back();
                        param.name                = binding.name;
                        param.type                = binding.type;
                        param.space               = space_idx;
                        param.binding             = binding_idx;
                        param.default_tex         = binding.default_tex;
                        param.default_sampler     = binding.default_sampler;
                        param.default_value_str   = binding.default_tex->name().str();
                        param.binding_type        = binding.binding;
                        break;
                    }

                    case GrcShaderBindingType::UniformBuffer:
                    case GrcShaderBindingType::StorageBuffer:
                    case GrcShaderBindingType::StorageImage2d: {
                        GrcShaderParamInfo& param = m_reflection.params_info.emplace_back();
                        param.name                = binding.name;
                        param.type                = binding.type;
                        param.space               = space_idx;
                        param.binding             = binding_idx;
                        param.binding_type        = binding.binding;
                        break;
                    }

                    default:
                        return StatusCode::InvalidState;
                }

                binding_idx++;
            }

            space_idx++;
        }

        int param_idx = 0;

        for (auto& param : m_reflection.params_info) {
            m_reflection.params_id[param.name] = param_idx++;
        }

        shader_class = std::make_shared<GrcShaderClass>(std::move(m_reflection));

        return StatusCode::Ok;
    }

}// namespace wmoge