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
     * @class TextureDescBuilder
     * @brief Utility class to build texture desc from external image data
     */
    class TextureDescBuilder {
    public:
        TextureDescBuilder(std::string name);

        TextureDescBuilder& set_flags(TextureFlags flags);
        TextureDescBuilder& set_image(Ref<Image> image, GfxFormat format);
        TextureDescBuilder& set_images(std::vector<Ref<Image>> images, GfxFormat format);
        TextureDescBuilder& set_mipmaps(bool mips);
        TextureDescBuilder& set_swizz(GfxTexSwizz swizz);
        TextureDescBuilder& set_resize(TexResizeParams resize);
        TextureDescBuilder& set_compression(TexCompressionFormat format);
        TextureDescBuilder& set_compression(TexCompressionParams compression);
        TextureDescBuilder& set_sampler(DefaultSampler sampler);

        [[nodiscard]] Status build_desc_2d(TextureDesc& out_desc);
        [[nodiscard]] Status build_desc_cube(TextureDesc& out_desc);

    protected:
        TexResizeParams         m_resize;
        TexCompressionParams    m_compression;
        TexCompressionStats     m_compression_stats;
        TextureFlags            m_flags;
        GfxFormat               m_format = GfxFormat::RGBA8;
        GfxTexSwizz             m_swizz  = GfxTexSwizz::None;
        DefaultSampler          m_sampler;
        Ref<Image>              m_source_image;
        std::vector<Ref<Image>> m_source_images;
        std::string             m_name;
        bool                    m_mipmaps = false;
        bool                    m_log     = true;
    };

    /**
     * @class TextureBuilder
     * @brief Utility class to build textures from external image data
     */
    class TextureBuilder : public TextureDescBuilder {
    public:
        TextureBuilder(std::string name, TextureManager* texture_manager);

        [[nodiscard]] Status build_2d(Ref<Texture2d>& out_texture);
        [[nodiscard]] Status build_cube(Ref<TextureCube>& out_texture);

    private:
        TextureManager* m_texture_manager;
    };

}// namespace wmoge