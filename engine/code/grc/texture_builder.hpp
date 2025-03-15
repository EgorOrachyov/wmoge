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

#include "core/status.hpp"
#include "grc/texture.hpp"
#include "grc/texture_compression.hpp"
#include "grc/texture_manager.hpp"
#include "grc/texture_resize.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class TextureBuilder
     * @brief Utility class to build textures from external image data
     */
    class TextureBuilder {
    public:
        TextureBuilder(Strid name, TextureManager* texture_manager);

        TextureBuilder& set_flags(TextureFlags flags);
        TextureBuilder& set_image(Ref<Image> image, GfxFormat format);
        TextureBuilder& set_images(std::vector<Ref<Image>> images, GfxFormat format);
        TextureBuilder& set_mipmaps(bool mips);
        TextureBuilder& set_swizz(GfxTexSwizz swizz);
        TextureBuilder& set_resize(TexResizeParams resize);
        TextureBuilder& set_compression(TexCompressionFormat format);
        TextureBuilder& set_compression(TexCompressionParams compression);
        TextureBuilder& set_sampler(DefaultSampler sampler);
        TextureBuilder& set_sampler(GfxSamplerDesc sampler);

        [[nodiscard]] Status build_2d(Ref<Texture2d>& out_texture);
        [[nodiscard]] Status build_cube(Ref<TextureCube>& out_texture);

    private:
        TextureDesc             m_desc;
        TexResizeParams         m_resize;
        TexCompressionParams    m_compression;
        TexCompressionStats     m_compression_stats;
        TextureFlags            m_flags;
        GfxFormat               m_format = GfxFormat::RGBA8;
        GfxTexSwizz             m_swizz  = GfxTexSwizz::None;
        Ref<GfxSampler>         m_sampler;
        Ref<Image>              m_source_image;
        std::vector<Ref<Image>> m_source_images;
        TextureManager*         m_texture_manager;
        Strid                   m_builder_name;
        bool                    m_mipmaps = false;
        bool                    m_log     = true;
    };

}// namespace wmoge