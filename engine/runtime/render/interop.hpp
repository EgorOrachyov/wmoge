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

#pragma once

#include "core/string_id.hpp"
#include "grc/shader_reflection.hpp"

#include <vector>

namespace wmoge {

    class ShaderManager;

#define BEGIN_REFLECT(name)                    \
    static Status reflect(ShaderManager* sm) { \
        ShaderStructRegister r(Strid(#name), sizeof(name), sm)

#define BEGIN_REFLECT_STRUCTURED_BUFFER(name)  \
    static Status reflect(ShaderManager* sm) { \
        ShaderStructRegister r(Strid(#name), 0, sm)

#define END_REFLECT    \
    return r.finish(); \
    }

#define FIELD(type, name) \
    r.add_field(Strid(#name), Strid(#type))

#define STRUCT(type, name) \
    r.add_field(Strid(#name), Strid(#type))

#define FIELD_ARRAY(type, name, n) \
    r.add_field_array(Strid(#name), Strid(#type), n)

#define STRUCT_ARRAY(type, name, n) \
    r.add_field_array(Strid(#name), Strid(#type), n)

#define STRUCT_ARRAY_UNBOUND(type, name) \
    r.add_field_array(Strid(#name), Strid(#type))

    struct ShaderCanvas {
        static constexpr int MAX_IMAGES = 4;
    };

    struct GpuCanvasDrawCmdData {
        Vec4f Transform0;
        Vec4f Transform1;
        Vec4f Transform2;
        Vec4f ClipRect;
        int   TextureIdx;
        int   pad0;
        int   pad1;
        int   pad2;

        BEGIN_REFLECT(GpuCanvasDrawCmdData);
        FIELD(vec4, Transform0);
        FIELD(vec4, Transform1);
        FIELD(vec4, Transform2);
        FIELD(vec4, ClipRect);
        FIELD(int, TextureIdx);
        FIELD(int, pad0);
        FIELD(int, pad1);
        FIELD(int, pad2);
        END_REFLECT
    };

    struct GpuCanvasDrawCmdsBuffer {
        std::vector<GpuCanvasDrawCmdData> DrawCmds;

        BEGIN_REFLECT_STRUCTURED_BUFFER(GpuCanvasDrawCmdsBuffer);
        STRUCT_ARRAY_UNBOUND(GpuCanvasDrawCmdData, DrawCmds);
        END_REFLECT
    };

    Status reflect_shader_types(ShaderManager* sm);

#undef BEGIN_REFLECT
#undef END_REFLCET
#undef ADD_FIELD
#undef ADD_FIELD_ARRAY
#undef ADD_STRUCT
#undef ADD_STRUCT_ARRAY

}// namespace wmoge