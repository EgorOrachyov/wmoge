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
#include "core/buffered_vector.hpp"
#include "core/data.hpp"
#include "core/flat_map.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/image.hpp"
#include "grc/texture.hpp"
#include "io/serialization.hpp"
#include "math/vec.hpp"

#include <vector>

namespace wmoge {

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
        Ref<Data>                file_content;
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
        WG_RTTI_CLASS(Font, Asset);

        Font()           = default;
        ~Font() override = default;

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

        [[nodiscard]] const std::string&              get_family_name() const { return m_family_name; }
        [[nodiscard]] const std::string&              get_style_name() const { return m_style_name; }
        [[nodiscard]] const flat_map<int, FontGlyph>& get_glyphs() const { return m_glyphs; }
        [[nodiscard]] const Ref<Texture2d>&           get_texture() const { return m_texture; }
        [[nodiscard]] const Ref<GfxTexture>&          get_bitmap() const { return m_texture->get_texture(); }
        [[nodiscard]] const Ref<GfxSampler>&          get_sampler() const { return m_texture->get_sampler(); }
        [[nodiscard]] int                             get_height() const { return m_height; }
        [[nodiscard]] int                             get_glyphs_in_row() const { return m_glyphs_in_row; }
        [[nodiscard]] int                             get_max_width() const { return m_max_width; }
        [[nodiscard]] int                             get_max_height() const { return m_max_height; }
        [[nodiscard]] Ref<Data>                       get_file_content() const { return m_file_content; }

    private:
        flat_map<int, FontGlyph> m_glyphs;
        Ref<Texture2d>           m_texture;
        std::string              m_family_name;
        std::string              m_style_name;
        int                      m_height        = -1;
        int                      m_glyphs_in_row = -1;
        int                      m_max_height    = -1;
        int                      m_max_width     = -1;
        Ref<Data>                m_file_content;
    };

    WG_RTTI_CLASS_BEGIN(Font) {
        WG_RTTI_META_DATA(RttiUiHint(""));
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge