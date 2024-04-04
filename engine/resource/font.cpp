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

#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "platform/file_system.hpp"
#include "resource/image.hpp"
#include "system/engine.hpp"

#include <sstream>
#include <string>

namespace wmoge {

    WG_IO_BEGIN(FontImportOptions)
    WG_IO_FIELD(source_file)
    WG_IO_FIELD(height)
    WG_IO_FIELD(glyphs_in_row)
    WG_IO_END(FontImportOptions)

    Status Font::init(const FontDesc& desc) {
        m_glyphs        = desc.glyphs;
        m_texture       = desc.texture;
        m_family_name   = desc.family_name;
        m_style_name    = desc.style_name;
        m_height        = desc.height;
        m_glyphs_in_row = desc.glyphs_in_row;
        m_max_height    = desc.max_height;
        m_max_width     = desc.max_width;

        return StatusCode::Ok;
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

    Status Font::copy_to(Object& copy) const {
        Resource::copy_to(copy);
        auto font             = dynamic_cast<Font*>(&copy);
        font->m_glyphs        = m_glyphs;
        font->m_family_name   = m_family_name;
        font->m_style_name    = m_style_name;
        font->m_texture       = m_texture;
        font->m_height        = m_height;
        font->m_glyphs_in_row = m_glyphs_in_row;
        return StatusCode::Ok;
    }

    std::string Font::to_string() {
        std::stringstream ss;
        ss << "{Font: family=" << m_family_name << ",style=" << m_style_name << ",h=" << m_height << ",path=" << get_name() << "}";
        return ss.str();
    }

    void Font::register_class() {
        auto cls = Class::register_class<Font>();
        cls->add_property(ClassProperty(VarType::String, SID("family_name"), SID("get_family_name")));
        cls->add_method(ClassMethod(VarType::String, SID("get_family_name"), {}), &Font::get_family_name, {});
        cls->add_property(ClassProperty(VarType::String, SID("style_name"), SID("get_style_name")));
        cls->add_method(ClassMethod(VarType::String, SID("get_style_name"), {}), &Font::get_style_name, {});
        cls->add_property(ClassProperty(VarType::Int, SID("height"), SID("get_height")));
        cls->add_method(ClassMethod(VarType::Int, SID("get_height"), {}), &Font::get_height, {});
        cls->add_property(ClassProperty(VarType::Int, SID("glyphs_in_row"), SID("get_glyphs_in_row")));
        cls->add_method(ClassMethod(VarType::Int, SID("get_glyphs_in_row"), {}), &Font::get_glyphs_in_row, {});
        cls->add_property(ClassProperty(VarType::Int, SID("max_width"), SID("get_max_width")));
        cls->add_method(ClassMethod(VarType::Int, SID("get_max_width"), {}), &Font::get_max_width, {});
        cls->add_property(ClassProperty(VarType::Int, SID("max_height"), SID("get_max_height")));
        cls->add_method(ClassMethod(VarType::Int, SID("get_max_height"), {}), &Font::get_max_height, {});
    }

}// namespace wmoge