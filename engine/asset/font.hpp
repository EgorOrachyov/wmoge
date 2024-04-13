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

#include "asset/asset.hpp"
#include "asset/image.hpp"
#include "asset/texture.hpp"
#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "io/serialization.hpp"
#include "math/vec.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class FontImportOptions
     * @brief Options to import a font asset
     */
    struct FontImportOptions {
        std::string source_file;
        int         height        = 32;
        int         glyphs_in_row = 16;

        WG_IO_DECLARE(FontImportOptions);
    };

    /**
     * @brief Alignment of text for layout
     */
    enum class FontTextAlignment {
        Center,
        Left
    };

    /**
     * @class FontGlyph
     * @brief Stores info about single glyph in a font file
     */
    struct FontGlyph {
        Size2i size;            /** character glyph width and height in pixels */
        Vec2i  bearing;         /** offset from baseline to left of glyph and to top of glyph in pixels */
        Vec2i  advance;         /** offset to advance to next glyph in X and Y axis in pixels */
        Vec2f  bitmap_uv0;      /** left/top glyph corners positions in font bitmap */
        Vec2f  bitmap_uv1;      /** right/bottom glyph corners positions in font bitmap */
        int    code_point = -1; /** code point */
    };

    /**
     * @class FontDesc
     * @brief Describes font data internal
    */
    struct FontDesc {
        flat_map<int, FontGlyph> glyphs;
        Ref<Texture2d>           texture;
        std::string              family_name;
        std::string              style_name;
        int                      height        = -1;
        int                      glyphs_in_row = -1;
        int                      max_height    = -1;
        int                      max_width     = -1;
    };

    /**
     * @class Font
     * @brief Font asset for rendering texts from true type fonts
     *
     * Font assets stores gpu bitmaps with font glyphs data as well as
     * glyph metrics per each loaded font symbol. Font allows to render
     * text using multiple sizes. Allow to compute precis text strings sizes
     * using selected font size settings.
     */
    class Font : public Asset {
    public:
        WG_OBJECT(Font, Asset);

        /**
         * @brief Init font from desc
         * 
         * @param desc Description of font
         * 
         * @return Ok on success
        */
        Status init(const FontDesc& desc);

        /**
         * @brief Returns the size in pixels of a text in given font
         *
         * @param text Text which size is to calculate
         * @param size Scale of font size
         *
         * @return Width and height in pixels of a text
         */
        Vec2f get_string_size(const std::string& text, float size);

        Status      copy_to(Object& copy) const override;
        std::string to_string() override;

        [[nodiscard]] const std::string&              get_family_name() { return m_family_name; }
        [[nodiscard]] const std::string&              get_style_name() { return m_style_name; }
        [[nodiscard]] const flat_map<int, FontGlyph>& get_glyphs() { return m_glyphs; }
        [[nodiscard]] const Ref<Texture2d>&           get_texture() { return m_texture; }
        [[nodiscard]] const Ref<GfxTexture>&          get_bitmap() { return m_texture->get_texture(); }
        [[nodiscard]] const Ref<GfxSampler>&          get_sampler() { return m_texture->get_sampler(); }
        [[nodiscard]] int                             get_height() { return m_height; }
        [[nodiscard]] int                             get_glyphs_in_row() { return m_glyphs_in_row; }
        [[nodiscard]] int                             get_max_width() { return m_max_width; }
        [[nodiscard]] int                             get_max_height() { return m_max_height; }

    private:
        flat_map<int, FontGlyph> m_glyphs;
        Ref<Texture2d>           m_texture;
        std::string              m_family_name;
        std::string              m_style_name;
        int                      m_height        = -1;
        int                      m_glyphs_in_row = -1;
        int                      m_max_height    = -1;
        int                      m_max_width     = -1;
    };

}// namespace wmoge