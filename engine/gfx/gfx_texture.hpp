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

#ifndef WMOGE_GFX_TEXTURE_HPP
#define WMOGE_GFX_TEXTURE_HPP

#include "gfx/gfx_resource.hpp"

namespace wmoge {

    /**
     * @class GfxTexture
     * @brief Gfx device texture resource
     */
    class GfxTexture : public GfxResource {
    public:
        ~GfxTexture() override = default;

        [[nodiscard]] int          width() const { return m_width; }
        [[nodiscard]] int          height() const { return m_height; }
        [[nodiscard]] int          depth() const { return m_depth; }
        [[nodiscard]] int          mips_count() const { return m_mips_count; }
        [[nodiscard]] int          array_slices() const { return m_array_slices; }
        [[nodiscard]] GfxTex       tex_type() const { return m_tex_type; }
        [[nodiscard]] GfxFormat    format() const { return m_format; }
        [[nodiscard]] GfxMemUsage  mem_usage() const { return m_mem_usage; }
        [[nodiscard]] GfxTexUsages usages() const { return m_usages; }

    protected:
        int          m_width        = 0;
        int          m_height       = 0;
        int          m_depth        = 0;
        int          m_mips_count   = 1;
        int          m_array_slices = 1;
        GfxTex       m_tex_type     = GfxTex::Tex2d;
        GfxFormat    m_format       = GfxFormat::RGBA8;
        GfxMemUsage  m_mem_usage    = GfxMemUsage::GpuLocal;
        GfxTexUsages m_usages;
    };

}// namespace wmoge

#endif//WMOGE_GFX_TEXTURE_HPP
