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

#include "shader_reflection.hpp"

#include "core/crc32.hpp"
#include "grc/shader_manager.hpp"

namespace wmoge {

    static Ref<ShaderType> make(ShaderBaseType base_type, Strid name, bool is_primitive = false) {
        Ref<ShaderType> t = make_ref<ShaderType>();
        t->name           = name;
        t->type           = base_type;
        t->byte_size      = ShaderBaseTypeSizes[int(base_type)];
        t->is_primitive   = is_primitive;
        t->is_builtin     = true;
        return t;
    }
    static Ref<ShaderType> make(ShaderBaseType base_type, Strid name, int n_rows) {
        Ref<ShaderType> t = make_ref<ShaderType>();
        t->name           = name;
        t->type           = base_type;
        t->n_row          = n_rows;
        t->n_col          = 1;
        t->n_elem         = n_rows * 1;
        t->byte_size      = ShaderBaseTypeSizes[int(base_type)] * t->n_elem;
        t->is_primitive   = true;
        t->is_builtin     = true;
        return t;
    }
    static Ref<ShaderType> make(ShaderBaseType base_type, Strid name, int n_rows, int n_cols) {
        Ref<ShaderType> t = make_ref<ShaderType>();
        t->name           = name;
        t->type           = base_type;
        t->n_row          = n_rows;
        t->n_col          = n_cols;
        t->n_elem         = n_rows * n_cols;
        t->byte_size      = ShaderBaseTypeSizes[int(base_type)] * t->n_elem;
        t->is_primitive   = true;
        t->is_builtin     = true;
        return t;
    }

    Ref<ShaderType> ShaderTypes::FLOAT           = make(ShaderBaseType::Float, SID("float"), true);
    Ref<ShaderType> ShaderTypes::INT             = make(ShaderBaseType::Int, SID("int"), true);
    Ref<ShaderType> ShaderTypes::BOOL            = make(ShaderBaseType::Bool, SID("bool"), true);
    Ref<ShaderType> ShaderTypes::VEC2            = make(ShaderBaseType::Float, SID("vec2"), 2);
    Ref<ShaderType> ShaderTypes::VEC3            = make(ShaderBaseType::Float, SID("vec3"), 3);
    Ref<ShaderType> ShaderTypes::VEC4            = make(ShaderBaseType::Float, SID("vec4"), 4);
    Ref<ShaderType> ShaderTypes::IVEC2           = make(ShaderBaseType::Int, SID("ivec2"), 2);
    Ref<ShaderType> ShaderTypes::IVEC3           = make(ShaderBaseType::Int, SID("ivec3"), 3);
    Ref<ShaderType> ShaderTypes::IVEC4           = make(ShaderBaseType::Int, SID("ivec4"), 4);
    Ref<ShaderType> ShaderTypes::BVEC2           = make(ShaderBaseType::Bool, SID("bvec2"), 2);
    Ref<ShaderType> ShaderTypes::BVEC3           = make(ShaderBaseType::Bool, SID("bvec3"), 3);
    Ref<ShaderType> ShaderTypes::BVEC4           = make(ShaderBaseType::Bool, SID("bvec4"), 4);
    Ref<ShaderType> ShaderTypes::MAT2            = make(ShaderBaseType::Float, SID("mat2"), 2, 2);
    Ref<ShaderType> ShaderTypes::MAT3            = make(ShaderBaseType::Float, SID("mat3"), 3, 3);
    Ref<ShaderType> ShaderTypes::MAT4            = make(ShaderBaseType::Float, SID("mat4"), 4, 4);
    Ref<ShaderType> ShaderTypes::SAMPLER2D       = make(ShaderBaseType::Sampler2d, SID("sampler2D"));
    Ref<ShaderType> ShaderTypes::SAMPLER2D_ARRAY = make(ShaderBaseType::Sampler2dArray, SID("sampler2DArray"));
    Ref<ShaderType> ShaderTypes::SAMPLER_CUBE    = make(ShaderBaseType::SamplerCube, SID("samplerCube"));
    Ref<ShaderType> ShaderTypes::IMAGE2D         = make(ShaderBaseType::Image2d, SID("image2D"));

    std::vector<Ref<ShaderType>> ShaderTypes::builtin() {
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

    ShaderStructRegister::ShaderStructRegister(Strid name, std::size_t size, ShaderManager* shader_manager) {
        m_manager                = shader_manager;
        m_struct_type            = make_ref<ShaderType>();
        m_struct_type->name      = name;
        m_struct_type->byte_size = std::int16_t(size);
        m_struct_type->type      = ShaderBaseType::Struct;
    }

    ShaderStructRegister& ShaderStructRegister::add_field(Strid name, Strid struct_type) {
        ShaderTypeField& field = m_struct_type->fields.emplace_back();
        field.name             = name;
        field.type             = m_manager->find_global_type_idx(struct_type).value();
        field.offset           = m_manager->find_global_type(field.type).value()->byte_size;
        return *this;
    }

    ShaderStructRegister& ShaderStructRegister::add_field_array(Strid name, Strid struct_type, int n_elements) {
        ShaderTypeField& field = m_struct_type->fields.emplace_back();
        field.name             = name;
        field.type             = m_manager->find_global_type_idx(struct_type).value();
        field.is_array         = true;
        field.elem_count       = n_elements;
        field.offset           = n_elements * m_manager->find_global_type(field.type).value()->byte_size;
        return *this;
    }

    Status ShaderStructRegister::finish() {
        m_manager->add_global_type(m_struct_type);
        m_struct_type.reset();

        return WG_OK;
    }

    bool ShaderPermutation::operator==(const ShaderPermutation& other) const {
        return options == other.options &&
               vert_attribs == other.vert_attribs &&
               technique_idx == other.technique_idx &&
               pass_idx == pass_idx;
    }

    std::size_t ShaderPermutation::hash() const {
        return std::hash<ShaderOptions::Mask>()(options) ^
               std::hash<GfxVertAttribs::BitsetType>()(vert_attribs.bits) ^
               std::hash<std::int16_t>()(technique_idx) ^
               std::hash<std::int16_t>()(pass_idx);
    }

    void PipelineState::fill(GfxPsoStateGraphics& state) const {
        state.prim_type            = prim_type;
        state.rs.poly_mode         = rs.poly_mode;
        state.rs.cull_mode         = rs.cull_mode;
        state.rs.front_face        = rs.front_face;
        state.ds.depth_enable      = ds.depth_enable;
        state.ds.depth_write       = ds.depth_write;
        state.ds.depth_func        = ds.depth_func;
        state.ds.stencil_enable    = ds.stencil_enable;
        state.ds.stencil_wmask     = ds.stencil_wmask;
        state.ds.stencil_rvalue    = ds.stencil_rvalue;
        state.ds.stencil_cmask     = ds.stencil_cmask;
        state.ds.stencil_comp_func = ds.stencil_comp_func;
        state.ds.stencil_sfail     = ds.stencil_sfail;
        state.ds.stencil_dfail     = ds.stencil_dfail;
        state.ds.stencil_dpass     = ds.stencil_dpass;
        state.bs.blending          = bs.blending;
    }

    void rtti_grc_shader_reflection() {
        rtti_type<ShaderPermutation>();
        rtti_type<ShaderReflection>();
    }

}// namespace wmoge