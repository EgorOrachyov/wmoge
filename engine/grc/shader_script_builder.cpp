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

#include "shader_script_builder.hpp"

#include "core/string_utils.hpp"
#include "math/vec.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

    GrcShaderScriptBuilder::StructBuilder::StructBuilder(GrcShaderScriptBuilder& owner, Ref<GrcShaderType> struct_type)
        : m_owner(owner), m_struct_type(struct_type) {
    }

    GrcShaderScriptBuilder::StructBuilder& GrcShaderScriptBuilder::StructBuilder::add_field(Strid name, Strid struct_type) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = m_owner.m_reflection.declarations[struct_type];
        field.offset                = field.type->byte_size;
        return *this;
    }

    GrcShaderScriptBuilder::StructBuilder& GrcShaderScriptBuilder::StructBuilder::add_field(Strid name, Ref<GrcShaderType> type, Var value) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = type;
        field.default_value         = value;
        field.offset                = field.type->byte_size;
        return *this;
    }

    GrcShaderScriptBuilder::StructBuilder& GrcShaderScriptBuilder::StructBuilder::add_field_array(Strid name, Strid struct_type, int n_elements) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = m_owner.m_reflection.declarations[struct_type];
        field.is_array              = true;
        field.elem_count            = n_elements;
        field.offset                = n_elements * field.type->byte_size;
        return *this;
    }

    GrcShaderScriptBuilder::StructBuilder& GrcShaderScriptBuilder::StructBuilder::add_field_array(Strid name, Ref<GrcShaderType> type, int n_elements, Var value) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = type;
        field.default_value         = value;
        field.is_array              = true;
        field.elem_count            = n_elements;
        field.offset                = n_elements * field.type->byte_size;
        return *this;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::StructBuilder::end_struct() {
        return m_owner;
    }

    GrcShaderScriptBuilder::SpaceBuilder::SpaceBuilder(GrcShaderScriptBuilder& owner, GrcShaderSpace& space)
        : m_owner(owner), m_space(space) {
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_inline_uniform_buffer(Strid name, Strid type_struct) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::InlineUniformBuffer;
        binding.type              = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers.std140 = true;
        return *this;
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_uniform_buffer(Strid name, Strid type_struct) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::UniformBuffer;
        binding.type              = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers.std140 = true;
        return *this;
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_texture_2d(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::Sampler2d;
        binding.type              = GrcShaderTypes::SAMPLER2D;
        binding.default_tex       = texture;
        binding.default_sampler   = sampler;
        return *this;
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_texture_2d_array(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::Sampler2dArray;
        binding.type              = GrcShaderTypes::SAMPLER2D_ARRAY;
        binding.default_tex       = texture;
        binding.default_sampler   = sampler;
        return *this;
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_texture_cube(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::SamplerCube;
        binding.type              = GrcShaderTypes::SAMPLER_CUBE;
        binding.default_tex       = texture;
        binding.default_sampler   = sampler;
        return *this;
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_storage_buffer(Strid name, Strid type_struct) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::StorageBuffer;
        binding.type              = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers.std430 = true;
        return *this;
    }

    GrcShaderScriptBuilder::SpaceBuilder& GrcShaderScriptBuilder::SpaceBuilder::add_storage_image_2d(Strid name) {
        GrcShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name              = name;
        binding.binding           = GrcShaderBindingType::StorageImage2d;
        return *this;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::SpaceBuilder::end_space() {
        return m_owner;
    }

    GrcShaderScriptBuilder::PassBuilder::PassBuilder(GrcShaderScriptBuilder& owner, GrcShaderPassInfo& pass, TechniqueBuilder& technique)
        : m_owner(owner), m_pass(pass), m_technique(technique) {
    }

    GrcShaderScriptBuilder::PassBuilder& GrcShaderScriptBuilder::PassBuilder::add_option(Strid name, const buffered_vector<Strid>& variants) {
        GrcShaderOption& option = m_pass.options.options.emplace_back();
        option.name             = name;

        m_pass.options.options_map[name] = m_next_option_idx++;

        for (int i = 0; i < int(variants.size()); i++) {
            option.variants[variants[i]] = m_technique.m_next_variant_idx++;
        }

        return *this;
    }

    GrcShaderScriptBuilder::PassBuilder& GrcShaderScriptBuilder::PassBuilder::add_ui_info(const std::string& name, const std::string& hint) {
        m_pass.ui_name = name;
        m_pass.ui_hint = hint;
        return *this;
    }

    GrcShaderScriptBuilder::PassBuilder& GrcShaderScriptBuilder::PassBuilder::add_state(const GrcPipelineState& state) {
        m_pass.state = state;
        return *this;
    }

    GrcShaderScriptBuilder::PassBuilder& GrcShaderScriptBuilder::PassBuilder::add_tag(Strid name, Var value) {
        m_pass.tags[name] = std::move(value);
        return *this;
    }

    GrcShaderScriptBuilder::TechniqueBuilder& GrcShaderScriptBuilder::PassBuilder::end_pass() {
        return m_technique;
    }

    GrcShaderScriptBuilder::TechniqueBuilder::TechniqueBuilder(GrcShaderScriptBuilder& owner, GrcShaderTechniqueInfo& technique)
        : m_owner(owner), m_technique(technique) {
    }

    GrcShaderScriptBuilder::TechniqueBuilder& GrcShaderScriptBuilder::TechniqueBuilder::add_option(Strid name, const buffered_vector<Strid>& variants) {
        GrcShaderOption& option = m_technique.options.options.emplace_back();
        option.name             = name;

        m_technique.options.options_map[name] = m_next_option_idx++;

        for (int i = 0; i < int(variants.size()); i++) {
            option.variants[variants[i]] = m_next_variant_idx++;
        }

        return *this;
    }

    GrcShaderScriptBuilder::TechniqueBuilder& GrcShaderScriptBuilder::TechniqueBuilder::add_tag(Strid name, Var value) {
        m_technique.tags[name] = std::move(value);
        return *this;
    }

    GrcShaderScriptBuilder::TechniqueBuilder& GrcShaderScriptBuilder::TechniqueBuilder::add_ui_info(const std::string& name, const std::string& hint) {
        m_technique.ui_name = name;
        m_technique.ui_hint = hint;
        return *this;
    }

    GrcShaderScriptBuilder::PassBuilder GrcShaderScriptBuilder::TechniqueBuilder::add_pass(Strid name) {
        GrcShaderPassInfo& pass      = m_technique.passes.emplace_back();
        pass.name                    = name;
        m_technique.passes_map[name] = m_next_pass_idx++;
        return PassBuilder(m_owner, pass, *this);
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::TechniqueBuilder::end_technique() {
        return m_owner;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::set_name(Strid name) {
        m_reflection.shader_name = name;
        return *this;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::add_ui_info(const std::string& name, const std::string& hint) {
        m_reflection.ui_name = name;
        m_reflection.ui_hint = hint;
        return *this;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::add_source(Strid file, GfxShaderModule module) {
        GrcShaderSourceFile& source_file = m_reflection.sources.emplace_back();
        source_file.name                 = file;
        source_file.module               = module;
        return *this;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::add_constant(Strid name, Var value) {
        GrcShaderConstant& constant = m_reflection.constants.emplace_back();
        constant.name               = name;
        constant.str                = value.to_string();
        constant.value              = std::move(value);
        return *this;
    }

    GrcShaderScriptBuilder& GrcShaderScriptBuilder::add_struct(const Ref<GrcShaderType>& struct_type) {
        assert(struct_type);
        assert(struct_type->type == GrcShaderBaseType::Struct);
        m_reflection.declarations[struct_type->name] = struct_type;

        for (const auto& field : struct_type->fields) {
            if (field.type->type == GrcShaderBaseType::Struct) {
                add_struct(field.type);
            }
        }

        return *this;
    }

    GrcShaderScriptBuilder::StructBuilder GrcShaderScriptBuilder::add_struct(Strid name, int byte_size) {
        Ref<GrcShaderType> struct_type  = make_ref<GrcShaderType>();
        struct_type->name               = name;
        struct_type->type               = GrcShaderBaseType::Struct;
        struct_type->byte_size          = byte_size;
        m_reflection.declarations[name] = struct_type;
        return StructBuilder(*this, struct_type);
    }

    GrcShaderScriptBuilder::SpaceBuilder GrcShaderScriptBuilder::add_space(Strid name, GrcShaderSpaceType type) {
        GrcShaderSpace& space = m_reflection.spaces.emplace_back();
        space.name            = name;
        space.type            = type;
        return SpaceBuilder(*this, space);
    }

    GrcShaderScriptBuilder::TechniqueBuilder GrcShaderScriptBuilder::add_technique(Strid name) {
        GrcShaderTechniqueInfo& technique = m_reflection.techniques.emplace_back();
        technique.name                    = name;
        m_reflection.techniques_map[name] = m_next_technique_idx++;
        return TechniqueBuilder(*this, technique);
    }

    Status GrcShaderScriptBuilder::finish(Ref<GrcShaderScript>& shader_script) {
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

        std::int16_t space_idx = 0;

        for (const auto& space : m_reflection.spaces) {
            std::int16_t binding_idx = 0;
            std::int16_t buffer_idx  = 0;

            for (const auto& binding : space.bindings) {
                switch (binding.binding) {
                    case GrcShaderBindingType::InlineUniformBuffer: {
                        GrcShaderBufferInfo& buffer = m_reflection.buffers.emplace_back();
                        buffer.space                = space_idx;
                        buffer.binding              = binding_idx;
                        buffer.idx                  = buffer_idx;

                        std::int16_t offset = 0;

                        for (const auto& field : binding.type->fields) {
                            if (field.is_array && field.elem_count == 0) {
                                WG_LOG_ERROR("no size array not allowed in "
                                             << " name=" << binding.name
                                             << " in " << m_reflection.shader_name);
                                return StatusCode::Error;
                            }

                            const std::string param_name_base = field.name.str();
                            const Var&        default_var     = field.default_value;

                            GrcShaderParamInfo& field_param = m_reflection.params_info.emplace_back();
                            field_param.name                = SID(param_name_base);
                            field_param.type                = field.type;
                            field_param.space               = space_idx;
                            field_param.binding             = binding_idx;
                            field_param.buffer              = buffer_idx;
                            field_param.offset              = offset;
                            field_param.elem_count          = field.is_array ? field.elem_count : 1;
                            field_param.default_var         = default_var;
                            field_param.default_value_str   = default_var.to_string();
                            field_param.binding_type        = binding.binding;
                            field_param.byte_size           = field_param.elem_count * field_param.type->byte_size;

                            if (field.is_array) {
                                Array        default_vars    = default_var;
                                std::int16_t elemenet_offset = offset;

                                for (int i = 0; i < field.elem_count; i++) {

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
                                    param.offset              = elemenet_offset;
                                    param.elem_idx            = std::int16_t(i);
                                    param.default_var         = i < default_vars.size() ? default_vars[i] : default_var;
                                    param.default_value_str   = param.default_var.to_string();
                                    param.binding_type        = binding.binding;
                                    param.byte_size           = param.elem_count * param.type->byte_size;

                                    elemenet_offset += field.type->byte_size;
                                }
                            }

                            offset += field.offset;
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
                        param.default_value_str   = binding.default_tex ? binding.default_tex->name().str() : "Nil";
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

        std::int16_t param_idx = 0;

        for (auto& param : m_reflection.params_info) {
            m_reflection.params_id[param.name] = param_idx++;
        }

        for (auto& buffer : m_reflection.buffers) {
            Ref<Data>& defaults_data = buffer.defaults;
            defaults_data            = make_ref<Data>(buffer.size);

            std::uint8_t* defaults_ptr = defaults_data->buffer();
            std::memset(defaults_ptr, 0, buffer.size);

            const GrcShaderBinding&   binding     = m_reflection.spaces[buffer.space].bindings[buffer.binding];
            const Ref<GrcShaderType>& buffer_type = binding.type;

            assert(buffer_type->type == GrcShaderBaseType::Struct);

            for (const GrcShaderParamInfo& param : m_reflection.params_info) {
                if (param.binding != buffer.binding) {
                    continue;
                }
                if (param.elem_count > 1) {
                    continue;
                }
                if (param.default_var.type() == VarType::Nil) {
                    continue;
                }

                const Var&         var        = param.default_var;
                const auto&        param_type = param.type;
                const std::int16_t param_size = param.byte_size;
                std::uint8_t*      param_ptr  = defaults_ptr + param.offset;

                if (param_type == GrcShaderTypes::BOOL) {
                    const int v = var;
                    std::memcpy(param_ptr, &v, sizeof(v));
                } else if (param_type == GrcShaderTypes::INT) {
                    const int v = var;
                    std::memcpy(param_ptr, &v, sizeof(v));
                } else if (param_type == GrcShaderTypes::FLOAT) {
                    const float v = var;
                    std::memcpy(param_ptr, &v, sizeof(v));
                } else {
                    WG_LOG_WARNING("unsuported defaults type " << param_type->name);
                }
            }
        }

        shader_script = make_ref<GrcShaderScript>(std::move(m_reflection));
        return StatusCode::Ok;
    }

}// namespace wmoge