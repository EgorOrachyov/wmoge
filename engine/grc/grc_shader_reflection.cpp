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

#include "grc_shader_reflection.hpp"

#include "grc/grc_shader_manager.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    WG_IO_BEGIN(GrcPipelineState)
    WG_IO_FIELD_OPT(poly_mode)
    WG_IO_FIELD_OPT(cull_mode)
    WG_IO_FIELD_OPT(front_face)
    WG_IO_FIELD_OPT(depth_enable)
    WG_IO_FIELD_OPT(depth_write)
    WG_IO_FIELD_OPT(depth_func)
    WG_IO_FIELD_OPT(stencil_enable)
    WG_IO_FIELD_OPT(stencil_wmask)
    WG_IO_FIELD_OPT(stencil_rvalue)
    WG_IO_FIELD_OPT(stencil_cmask)
    WG_IO_FIELD_OPT(stencil_comp_func)
    WG_IO_FIELD_OPT(stencil_sfail)
    WG_IO_FIELD_OPT(stencil_dfail)
    WG_IO_FIELD_OPT(stencil_dpass)
    WG_IO_FIELD_OPT(blending)
    WG_IO_END(GrcPipelineState);

    static Ref<GrcShaderType> make(GrcShaderBaseType base_type, Strid name, bool is_primitive = false) {
        Ref<GrcShaderType> t = make_ref<GrcShaderType>();
        t->name              = name;
        t->type              = base_type;
        t->byte_size         = GrcShaderBaseTypeSizes[int(base_type)];
        t->is_primitive      = is_primitive;
        return t;
    }
    static Ref<GrcShaderType> make(GrcShaderBaseType base_type, Strid name, int n_rows) {
        Ref<GrcShaderType> t = make_ref<GrcShaderType>();
        t->name              = name;
        t->type              = base_type;
        t->n_row             = n_rows;
        t->n_col             = 1;
        t->n_elem            = n_rows * 1;
        t->byte_size         = GrcShaderBaseTypeSizes[int(base_type)] * t->n_elem;
        t->is_primitive      = true;
        return t;
    }
    static Ref<GrcShaderType> make(GrcShaderBaseType base_type, Strid name, int n_rows, int n_cols) {
        Ref<GrcShaderType> t = make_ref<GrcShaderType>();
        t->name              = name;
        t->type              = base_type;
        t->n_row             = n_rows;
        t->n_col             = n_cols;
        t->n_elem            = n_rows * n_cols;
        t->byte_size         = GrcShaderBaseTypeSizes[int(base_type)] * t->n_elem;
        t->is_primitive      = true;
        return t;
    }

    Ref<GrcShaderType> GrcShaderTypes::FLOAT           = make(GrcShaderBaseType::Float, SID("float"), true);
    Ref<GrcShaderType> GrcShaderTypes::INT             = make(GrcShaderBaseType::Int, SID("int"), true);
    Ref<GrcShaderType> GrcShaderTypes::BOOL            = make(GrcShaderBaseType::Bool, SID("bool"), true);
    Ref<GrcShaderType> GrcShaderTypes::VEC2            = make(GrcShaderBaseType::Float, SID("vec2"), 2);
    Ref<GrcShaderType> GrcShaderTypes::VEC3            = make(GrcShaderBaseType::Float, SID("vec3"), 3);
    Ref<GrcShaderType> GrcShaderTypes::VEC4            = make(GrcShaderBaseType::Float, SID("vec4"), 4);
    Ref<GrcShaderType> GrcShaderTypes::IVEC2           = make(GrcShaderBaseType::Int, SID("ivec2"), 2);
    Ref<GrcShaderType> GrcShaderTypes::IVEC3           = make(GrcShaderBaseType::Int, SID("ivec3"), 3);
    Ref<GrcShaderType> GrcShaderTypes::IVEC4           = make(GrcShaderBaseType::Int, SID("ivec4"), 4);
    Ref<GrcShaderType> GrcShaderTypes::BVEC2           = make(GrcShaderBaseType::Bool, SID("bvec2"), 2);
    Ref<GrcShaderType> GrcShaderTypes::BVEC3           = make(GrcShaderBaseType::Bool, SID("bvec3"), 3);
    Ref<GrcShaderType> GrcShaderTypes::BVEC4           = make(GrcShaderBaseType::Bool, SID("bvec4"), 4);
    Ref<GrcShaderType> GrcShaderTypes::MAT2            = make(GrcShaderBaseType::Float, SID("mat2"), 2, 2);
    Ref<GrcShaderType> GrcShaderTypes::MAT3            = make(GrcShaderBaseType::Float, SID("mat3"), 3, 3);
    Ref<GrcShaderType> GrcShaderTypes::MAT4            = make(GrcShaderBaseType::Float, SID("mat4"), 4, 4);
    Ref<GrcShaderType> GrcShaderTypes::SAMPLER2D       = make(GrcShaderBaseType::Sampler2d, SID("sampler2D"));
    Ref<GrcShaderType> GrcShaderTypes::SAMPLER2D_ARRAY = make(GrcShaderBaseType::Sampler2dArray, SID("sampler2DArray"));
    Ref<GrcShaderType> GrcShaderTypes::SAMPLER_CUBE    = make(GrcShaderBaseType::SamplerCube, SID("samplerCube"));
    Ref<GrcShaderType> GrcShaderTypes::IMAGE2D         = make(GrcShaderBaseType::Image2d, SID("image2D"));

    std::vector<Ref<GrcShaderType>> GrcShaderTypes::builtin() {
        return {FLOAT,
                INT,
                BOOL,
                VEC2,
                VEC3,
                VEC4,
                IVEC2,
                IVEC3,
                IVEC4,
                BVEC2,
                BVEC3,
                BVEC4,
                MAT2,
                MAT3,
                MAT4,
                SAMPLER2D,
                SAMPLER2D_ARRAY,
                SAMPLER_CUBE,
                IMAGE2D};
    }

    GrcShaderStructRegister::GrcShaderStructRegister(Strid name, std::size_t size) {
        m_manager                = IocContainer::instance()->resolve_v<GrcShaderManager>();
        m_struct_type            = make_ref<GrcShaderType>();
        m_struct_type->name      = name;
        m_struct_type->byte_size = std::int16_t(size);
        m_struct_type->type      = GrcShaderBaseType::Struct;
    }

    GrcShaderStructRegister& GrcShaderStructRegister::add_field(Strid name, Strid struct_type) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = m_manager->find_global_type(struct_type).value();
        field.offset                = field.type->byte_size;
        return *this;
    }

    GrcShaderStructRegister& GrcShaderStructRegister::add_field(Strid name, Ref<GrcShaderType> type, Var value) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = type;
        field.default_value         = value;
        field.offset                = field.type->byte_size;
        return *this;
    }

    GrcShaderStructRegister& GrcShaderStructRegister::add_field_array(Strid name, Strid struct_type, int n_elements) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = m_manager->find_global_type(struct_type).value();
        field.is_array              = true;
        field.elem_count            = n_elements;
        field.offset                = n_elements * field.type->byte_size;
        return *this;
    }

    GrcShaderStructRegister& GrcShaderStructRegister::add_field_array(Strid name, Ref<GrcShaderType> type, int n_elements, Var value) {
        GrcShaderType::Field& field = m_struct_type->fields.emplace_back();
        field.name                  = name;
        field.type                  = type;
        field.default_value         = value;
        field.is_array              = true;
        field.elem_count            = n_elements;
        field.offset                = n_elements * field.type->byte_size;
        return *this;
    }

    Status GrcShaderStructRegister::finish() {
        m_manager->add_global_type(m_struct_type);
        m_struct_type.reset();

        return StatusCode::Ok;
    }

}// namespace wmoge