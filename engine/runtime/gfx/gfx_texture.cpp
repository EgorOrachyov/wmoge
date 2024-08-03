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

#include "gfx_texture.hpp"

namespace wmoge {

    bool GfxTextureDesc::operator==(const GfxTextureDesc& other) const {
        return width == other.width &&
               height == other.height &&
               depth == other.depth &&
               mips_count == other.mips_count &&
               array_slices == other.array_slices &&
               tex_type == other.tex_type &&
               swizz == other.swizz &&
               format == other.format &&
               mem_usage == other.mem_usage &&
               usages == other.usages;
    }

    bool GfxTextureDesc::is_compatible(const GfxTextureDesc& other) const {
        return *this == other;
    }

    GfxTextureDesc GfxTextureDesc::make_2d(GfxFormat format, int width, int height, GfxTexSwizz swizz) {
        GfxTextureDesc desc;
        desc.format       = format;
        desc.width        = width;
        desc.height       = height;
        desc.swizz        = swizz;
        desc.depth        = 1;
        desc.array_slices = 1;
        desc.tex_type     = GfxTex::Tex2d;
        return desc;
    }

    GfxTextureDesc GfxTextureDesc::make_cube(GfxFormat format, int width, int height, GfxTexSwizz swizz) {
        GfxTextureDesc desc;
        desc.format       = format;
        desc.width        = width;
        desc.height       = height;
        desc.swizz        = swizz;
        desc.depth        = 1;
        desc.array_slices = 6;
        desc.tex_type     = GfxTex::TexCube;
        return desc;
    }

}// namespace wmoge