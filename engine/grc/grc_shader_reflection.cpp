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

namespace wmoge {

    static Ref<GrcShaderType> make(GrcShaderBaseType base_type, Strid name) {
        Ref<GrcShaderType> t = make_ref<GrcShaderType>();
        t->name              = name;
        t->type              = base_type;
        t->byte_size         = GrcShaderBaseTypeSizes[int(base_type)];
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
        return t;
    }

    Ref<GrcShaderType> GrcShaderTypes::FLOAT           = make(GrcShaderBaseType::Float, SID("float"));
    Ref<GrcShaderType> GrcShaderTypes::INT             = make(GrcShaderBaseType::Int, SID("int"));
    Ref<GrcShaderType> GrcShaderTypes::BOOL            = make(GrcShaderBaseType::Bool, SID("bool"));
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

}// namespace wmoge