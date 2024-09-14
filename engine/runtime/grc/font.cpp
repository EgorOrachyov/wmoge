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

#include "font.hpp"

#include "gfx/gfx_driver.hpp"
#include "grc/image.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"

#include <sstream>
#include <string>

namespace wmoge {

    Status Font::init(const FontDesc& desc) {
        m_glyphs        = desc.glyphs;
        m_texture       = desc.texture;
        m_family_name   = desc.family_name;
        m_style_name    = desc.style_name;
        m_height        = desc.height;
        m_glyphs_in_row = desc.glyphs_in_row;
        m_max_height    = desc.max_height;
        m_max_width     = desc.max_width;

        return WG_OK;
    }

    Vec2f Font::get_string_size(const std::string& text, float size) {
        const int   n          = int(text.size());
        const float scale      = size > 0 ? size / float(get_height()) : 1.0f;
        const auto  null_glyph = m_glyphs.find(0)->second;

        float advance_x = 0.0f;
        float height    = 0.0f;

        for (int i = 0; i < n; ++i) {
            auto c     = text[i];
            auto query = m_glyphs.find(int(c));

            FontGlyph font_glyph = null_glyph;
            if (query != m_glyphs.end()) font_glyph = query->second;

            float top    = scale * float(font_glyph.bearing.y());
            float bottom = top - scale * float(font_glyph.size.y());

            advance_x += scale * float(font_glyph.advance.x());
            height = Math::max(height, top - bottom);
        }

        return Vec2f(advance_x, height);
    }

}// namespace wmoge