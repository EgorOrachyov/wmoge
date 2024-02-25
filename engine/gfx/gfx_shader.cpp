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

#include "gfx_shader.hpp"

#include "debug/profiler.hpp"

namespace wmoge {

    WG_IO_BEGIN_NMSP(GfxShaderReflection, Texture)
    WG_IO_FIELD(name);
    WG_IO_FIELD(set);
    WG_IO_FIELD(binding);
    WG_IO_FIELD(array_size);
    WG_IO_FIELD(tex);
    WG_IO_END_NMSP(GfxShaderReflection, Texture)

    WG_IO_BEGIN_NMSP(GfxShaderReflection, Buffer)
    WG_IO_FIELD(name);
    WG_IO_FIELD(set);
    WG_IO_FIELD(binding);
    WG_IO_FIELD(size);
    WG_IO_END_NMSP(GfxShaderReflection, Buffer)

    WG_IO_BEGIN(GfxShaderReflection)
    WG_IO_FIELD(textures);
    WG_IO_FIELD(ub_buffers);
    WG_IO_FIELD(sb_buffers);
    WG_IO_FIELD(textures_per_desc);
    WG_IO_FIELD(ub_buffers_per_desc);
    WG_IO_FIELD(sb_buffers_per_desc);
    WG_IO_END(GfxShaderReflection)

}// namespace wmoge