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

#include "gfx/gfx_resource.hpp"

namespace wmoge {

    /**
     * @class GfxTextureDesc
     * @brief Gfx device texture descriptor
     */
    struct GfxTextureDesc {
        [[nodiscard]] bool operator==(const GfxTextureDesc& other) const;
        [[nodiscard]] bool is_compatible(const GfxTextureDesc& other) const;

        int          width        = 0;
        int          height       = 0;
        int          depth        = 0;
        int          mips_count   = 1;
        int          array_slices = 1;
        GfxTex       tex_type     = GfxTex::Tex2d;
        GfxTexSwizz  swizz        = GfxTexSwizz::None;
        GfxFormat    format       = GfxFormat::RGBA8;
        GfxMemUsage  mem_usage    = GfxMemUsage::GpuLocal;
        GfxTexUsages usages;

        static GfxTextureDesc make_2d(GfxFormat format, int width, int height, int mips, GfxTexSwizz swizz = GfxTexSwizz::None);
        static GfxTextureDesc make_2d(GfxFormat format, int width, int height, GfxTexSwizz swizz = GfxTexSwizz::None);
        static GfxTextureDesc make_cube(GfxFormat format, int width, int height, int mips, GfxTexSwizz swizz = GfxTexSwizz::None);
        static GfxTextureDesc make_cube(GfxFormat format, int width, int height, GfxTexSwizz swizz = GfxTexSwizz::None);
    };

    /**
     * @class GfxTexture
     * @brief Gfx device texture resource
     */
    class GfxTexture : public GfxResource {
    public:
        ~GfxTexture() override = default;

        [[nodiscard]] const GfxTextureDesc& desc() const { return m_desc; }
        [[nodiscard]] int                   width() const { return m_desc.width; }
        [[nodiscard]] int                   height() const { return m_desc.height; }
        [[nodiscard]] int                   depth() const { return m_desc.depth; }
        [[nodiscard]] int                   mips_count() const { return m_desc.mips_count; }
        [[nodiscard]] int                   array_slices() const { return m_desc.array_slices; }
        [[nodiscard]] GfxTex                tex_type() const { return m_desc.tex_type; }
        [[nodiscard]] GfxTexSwizz           tex_swizz() const { return m_desc.swizz; }
        [[nodiscard]] GfxFormat             format() const { return m_desc.format; }
        [[nodiscard]] GfxMemUsage           mem_usage() const { return m_desc.mem_usage; }
        [[nodiscard]] GfxTexUsages          usages() const { return m_desc.usages; }

    protected:
        GfxTextureDesc m_desc;
    };

    using GfxTextureRef = Ref<GfxTexture>;

}// namespace wmoge