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

#include "shader_builder.hpp"

#include "core/string_utils.hpp"
#include "math/vec.hpp"

#include <cassert>
#include <cstring>

namespace wmoge {

    ShaderBuilder::StructBuilder::StructBuilder(ShaderBuilder& owner, Ref<ShaderType> struct_type)
        : m_owner(owner), m_struct_type(struct_type) {
    }

    ShaderBuilder::StructBuilder& ShaderBuilder::StructBuilder::add_field(Strid name, Strid struct_type) {
        ShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name               = name;
        field.type               = m_owner.m_reflection.declarations[struct_type];
        field.offset             = field.type->byte_size;
        return *this;
    }

    ShaderBuilder::StructBuilder& ShaderBuilder::StructBuilder::add_field(Strid name, Ref<ShaderType> type, Var value) {
        ShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name               = name;
        field.type               = type;
        field.default_value      = value;
        field.offset             = field.type->byte_size;
        return *this;
    }

    ShaderBuilder::StructBuilder& ShaderBuilder::StructBuilder::add_field_array(Strid name, Strid struct_type, int n_elements) {
        ShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name               = name;
        field.type               = m_owner.m_reflection.declarations[struct_type];
        field.is_array           = true;
        field.elem_count         = n_elements;
        field.offset             = n_elements * field.type->byte_size;
        return *this;
    }

    ShaderBuilder::StructBuilder& ShaderBuilder::StructBuilder::add_field_array(Strid name, Ref<ShaderType> type, int n_elements, Var value) {
        ShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name               = name;
        field.type               = type;
        field.default_value      = value;
        field.is_array           = true;
        field.elem_count         = n_elements;
        field.offset             = n_elements * field.type->byte_size;
        return *this;
    }

    ShaderBuilder& ShaderBuilder::StructBuilder::end_struct() {
        return m_owner;
    }

    ShaderBuilder::SpaceBuilder::SpaceBuilder(ShaderBuilder& owner, ShaderSpace& space)
        : m_owner(owner), m_space(space) {
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_inline_uniform_buffer(Strid name, Strid type_struct) {
        ShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name           = name;
        binding.binding        = ShaderBindingType::InlineUniformBuffer;
        binding.type           = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers     = {ShaderQualifier::Std140};
        return *this;
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_uniform_buffer(Strid name, Strid type_struct) {
        ShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name           = name;
        binding.binding        = ShaderBindingType::UniformBuffer;
        binding.type           = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers     = {ShaderQualifier::Std140};
        return *this;
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_texture_2d(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        ShaderBinding& binding  = m_space.bindings.emplace_back();
        binding.name            = name;
        binding.binding         = ShaderBindingType::Sampler2d;
        binding.type            = ShaderTypes::SAMPLER2D;
        binding.default_tex     = texture;
        binding.default_sampler = sampler;
        return *this;
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_texture_2d_array(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        ShaderBinding& binding  = m_space.bindings.emplace_back();
        binding.name            = name;
        binding.binding         = ShaderBindingType::Sampler2dArray;
        binding.type            = ShaderTypes::SAMPLER2D_ARRAY;
        binding.default_tex     = texture;
        binding.default_sampler = sampler;
        return *this;
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_texture_cube(Strid name, Ref<GfxTexture> texture, Ref<GfxSampler> sampler) {
        ShaderBinding& binding  = m_space.bindings.emplace_back();
        binding.name            = name;
        binding.binding         = ShaderBindingType::SamplerCube;
        binding.type            = ShaderTypes::SAMPLER_CUBE;
        binding.default_tex     = texture;
        binding.default_sampler = sampler;
        return *this;
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_storage_buffer(Strid name, Strid type_struct) {
        ShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name           = name;
        binding.binding        = ShaderBindingType::StorageBuffer;
        binding.type           = m_owner.m_reflection.declarations[type_struct];
        binding.qualifiers     = {ShaderQualifier::Std430};
        return *this;
    }

    ShaderBuilder::SpaceBuilder& ShaderBuilder::SpaceBuilder::add_storage_image_2d(Strid name) {
        ShaderBinding& binding = m_space.bindings.emplace_back();
        binding.name           = name;
        binding.binding        = ShaderBindingType::StorageImage2d;
        return *this;
    }

    ShaderBuilder& ShaderBuilder::SpaceBuilder::end_space() {
        return m_owner;
    }

    ShaderBuilder::PassBuilder::PassBuilder(ShaderBuilder& owner, ShaderPassInfo& pass, TechniqueBuilder& technique)
        : m_owner(owner), m_pass(pass), m_technique(technique) {
    }

    ShaderBuilder::PassBuilder& ShaderBuilder::PassBuilder::add_option(Strid name, const buffered_vector<Strid>& variants) {
        ShaderOption& option = m_pass.options.options.emplace_back();
        option.name          = name;
        option.base_variant  = variants.front();

        m_pass.options.options_map[name] = m_next_option_idx++;

        for (int i = 0; i < int(variants.size()); i++) {
            option.variants[variants[i]] = m_technique.m_next_variant_idx++;
            m_technique.m_technique.options_remap.emplace_back(name);
            m_technique.m_technique.variants_remap.emplace_back(variants[i]);
        }

        return *this;
    }

    ShaderBuilder::PassBuilder& ShaderBuilder::PassBuilder::add_ui_info(const std::string& name, const std::string& hint) {
        m_pass.ui_name = name;
        m_pass.ui_hint = hint;
        return *this;
    }

    ShaderBuilder::PassBuilder& ShaderBuilder::PassBuilder::add_state(const PipelineState& state) {
        m_pass.state = state;
        return *this;
    }

    ShaderBuilder::PassBuilder& ShaderBuilder::PassBuilder::add_tag(Strid name, Var value) {
        m_pass.tags[name] = std::move(value);
        return *this;
    }

    ShaderBuilder::TechniqueBuilder& ShaderBuilder::PassBuilder::end_pass() {
        return m_technique;
    }

    ShaderBuilder::TechniqueBuilder::TechniqueBuilder(ShaderBuilder& owner, ShaderTechniqueInfo& technique)
        : m_owner(owner), m_technique(technique) {
    }

    ShaderBuilder::TechniqueBuilder& ShaderBuilder::TechniqueBuilder::add_option(Strid name, const buffered_vector<Strid>& variants) {
        ShaderOption& option = m_technique.options.options.emplace_back();
        option.name          = name;
        option.base_variant  = variants.front();

        m_technique.options.options_map[name] = m_next_option_idx++;

        for (int i = 0; i < int(variants.size()); i++) {
            option.variants[variants[i]] = m_next_variant_idx++;
            m_technique.options_remap.emplace_back(name);
            m_technique.variants_remap.emplace_back(variants[i]);
        }

        return *this;
    }

    ShaderBuilder::TechniqueBuilder& ShaderBuilder::TechniqueBuilder::add_tag(Strid name, Var value) {
        m_technique.tags[name] = std::move(value);
        return *this;
    }

    ShaderBuilder::TechniqueBuilder& ShaderBuilder::TechniqueBuilder::add_ui_info(const std::string& name, const std::string& hint) {
        m_technique.ui_name = name;
        m_technique.ui_hint = hint;
        return *this;
    }

    ShaderBuilder::PassBuilder ShaderBuilder::TechniqueBuilder::add_pass(Strid name) {
        ShaderPassInfo& pass         = m_technique.passes.emplace_back();
        pass.name                    = name;
        m_technique.passes_map[name] = m_next_pass_idx++;
        return PassBuilder(m_owner, pass, *this);
    }

    ShaderBuilder& ShaderBuilder::TechniqueBuilder::end_technique() {
        return m_owner;
    }

    ShaderBuilder& ShaderBuilder::set_name(Strid name) {
        m_reflection.shader_name = name;
        return *this;
    }

    ShaderBuilder& ShaderBuilder::set_domain(ShaderDomain domain) {
        m_reflection.domain = domain;
        return *this;
    }

    ShaderBuilder& ShaderBuilder::add_ui_info(const std::string& name, const std::string& hint) {
        m_reflection.ui_name = name;
        m_reflection.ui_hint = hint;
        return *this;
    }

    ShaderBuilder& ShaderBuilder::add_source(Strid file, GfxShaderModule module, GfxShaderLang lang) {
        ShaderSourceFile& source_file = m_reflection.sources.emplace_back();
        source_file.file              = file;
        source_file.module            = module;
        source_file.lang              = lang;
        m_reflection.languages.insert(lang);
        return *this;
    }

    ShaderBuilder& ShaderBuilder::add_constant(Strid name, Var value) {
        ShaderConstant& constant = m_reflection.constants.emplace_back();
        constant.name            = name;
        constant.str             = value.to_string();
        constant.value           = std::move(value);
        return *this;
    }

    ShaderBuilder& ShaderBuilder::add_struct(const Ref<ShaderType>& struct_type) {
        assert(struct_type);
        assert(struct_type->type == ShaderBaseType::Struct);
        m_reflection.declarations[struct_type->name] = struct_type;

        for (const auto& field : struct_type->fields) {
            if (field.type->type == ShaderBaseType::Struct) {
                add_struct(field.type);
            }
        }

        return *this;
    }

    ShaderBuilder::StructBuilder ShaderBuilder::add_struct(Strid name, int byte_size) {
        Ref<ShaderType> struct_type     = make_ref<ShaderType>();
        struct_type->name               = name;
        struct_type->type               = ShaderBaseType::Struct;
        struct_type->byte_size          = byte_size;
        m_reflection.declarations[name] = struct_type;
        return StructBuilder(*this, struct_type);
    }

    ShaderBuilder::SpaceBuilder ShaderBuilder::add_space(Strid name, ShaderSpaceType type) {
        ShaderSpace& space = m_reflection.spaces.emplace_back();
        space.name         = name;
        space.type         = type;
        return SpaceBuilder(*this, space);
    }

    ShaderBuilder::TechniqueBuilder ShaderBuilder::add_technique(Strid name) {
        ShaderTechniqueInfo& technique    = m_reflection.techniques.emplace_back();
        technique.name                    = name;
        m_reflection.techniques_map[name] = m_next_technique_idx++;
        return TechniqueBuilder(*this, technique);
    }

    Status ShaderBuilder::finish() {
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
                    case ShaderBindingType::InlineUniformBuffer: {
                        ShaderBufferInfo& buffer = m_reflection.buffers.emplace_back();
                        buffer.space             = space_idx;
                        buffer.binding           = binding_idx;
                        buffer.idx               = buffer_idx;

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

                            ShaderParamInfo& field_param  = m_reflection.params_info.emplace_back();
                            field_param.name              = SID(param_name_base);
                            field_param.type              = field.type;
                            field_param.space             = space_idx;
                            field_param.binding           = binding_idx;
                            field_param.buffer            = buffer_idx;
                            field_param.offset            = offset;
                            field_param.elem_count        = field.is_array ? field.elem_count : 1;
                            field_param.default_var       = default_var;
                            field_param.default_value_str = default_var.to_string();
                            field_param.binding_type      = binding.binding;
                            field_param.byte_size         = field_param.elem_count * field_param.type->byte_size;

                            if (field.is_array) {
                                Array        default_vars    = default_var;
                                std::int16_t elemenet_offset = offset;

                                for (int i = 0; i < field.elem_count; i++) {

                                    std::string param_name = param_name_base;

                                    if (field.is_array) {
                                        param_name += "[" + StringUtils::from_int(i) + "]";
                                    }

                                    ShaderParamInfo& param  = m_reflection.params_info.emplace_back();
                                    param.name              = SID(param_name);
                                    param.type              = field.type;
                                    param.space             = space_idx;
                                    param.binding           = binding_idx;
                                    param.buffer            = buffer_idx;
                                    param.offset            = elemenet_offset;
                                    param.elem_idx          = std::int16_t(i);
                                    param.default_var       = i < default_vars.size() ? default_vars[i] : default_var;
                                    param.default_value_str = param.default_var.to_string();
                                    param.binding_type      = binding.binding;
                                    param.byte_size         = param.elem_count * param.type->byte_size;

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

                    case ShaderBindingType::Sampler2d:
                    case ShaderBindingType::Sampler2dArray:
                    case ShaderBindingType::SamplerCube: {
                        ShaderParamInfo& param  = m_reflection.params_info.emplace_back();
                        param.name              = binding.name;
                        param.type              = binding.type;
                        param.space             = space_idx;
                        param.binding           = binding_idx;
                        param.default_tex       = binding.default_tex;
                        param.default_sampler   = binding.default_sampler;
                        param.default_value_str = binding.default_tex ? binding.default_tex->name().str() : "Nil";
                        param.binding_type      = binding.binding;
                        break;
                    }

                    case ShaderBindingType::UniformBuffer:
                    case ShaderBindingType::StorageBuffer:
                    case ShaderBindingType::StorageImage2d: {
                        ShaderParamInfo& param = m_reflection.params_info.emplace_back();
                        param.name             = binding.name;
                        param.type             = binding.type;
                        param.space            = space_idx;
                        param.binding          = binding_idx;
                        param.binding_type     = binding.binding;
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

            const ShaderBinding&   binding     = m_reflection.spaces[buffer.space].bindings[buffer.binding];
            const Ref<ShaderType>& buffer_type = binding.type;

            assert(buffer_type->type == ShaderBaseType::Struct);

            for (const ShaderParamInfo& param : m_reflection.params_info) {
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

                if (param_type == ShaderTypes::BOOL) {
                    const int v = var;
                    std::memcpy(param_ptr, &v, sizeof(v));
                } else if (param_type == ShaderTypes::INT) {
                    const int v = var;
                    std::memcpy(param_ptr, &v, sizeof(v));
                } else if (param_type == ShaderTypes::FLOAT) {
                    const float v = var;
                    std::memcpy(param_ptr, &v, sizeof(v));
                } else {
                    WG_LOG_WARNING("unsuported defaults type " << param_type->name);
                }
            }
        }

        return WG_OK;
    }

    ShaderReflection& ShaderBuilder::get_reflection() {
        return m_reflection;
    }

}// namespace wmoge