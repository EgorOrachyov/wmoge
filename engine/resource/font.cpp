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

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "platform/file_system.hpp"
#include "resource/image.hpp"

#include <freetype/freetype.h>

#include <sstream>
#include <string>

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, FontImportOptions& options) {
        WG_YAML_READ_AS(node, "source_file", options.source_file);
        WG_YAML_READ_AS_OPT(node, "height", options.height);
        WG_YAML_READ_AS_OPT(node, "glyphs_in_row", options.glyphs_in_row);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const FontImportOptions& options) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "source_file", options.source_file);
        WG_YAML_WRITE_AS(node, "height", options.height);
        WG_YAML_WRITE_AS(node, "glyphs_in_row", options.glyphs_in_row);

        return StatusCode::Ok;
    }

    Status Font::load(const std::string& path, int height, int glyphs_in_row) {
        WG_AUTO_PROFILE_RESOURCE("Font::load");

        static const int GLYPHS_SIZE_SHIFT    = 6;
        static const int GLYPHS_BITMAP_OFFSET = 2;

        std::vector<std::uint8_t> ttf_data;
        if (!Engine::instance()->file_system()->read_file(path, ttf_data)) {
            WG_LOG_ERROR("failed to load font data from resource pak " << path);
            return StatusCode::FailedRead;
        }

        FT_Library ft_library;
        if (FT_Init_FreeType(&ft_library)) {
            WG_LOG_ERROR("failed to init free type library");
            return StatusCode::Error;
        }

        FT_Face ft_face;
        if (FT_New_Memory_Face(ft_library, ttf_data.data(), static_cast<FT_Long>(ttf_data.size()), 0, &ft_face)) {
            WG_LOG_ERROR("failed to parse font data for " << path);
            FT_Done_FreeType(ft_library);
            return StatusCode::FailedParse;
        }

        m_family_name = ft_face->family_name;
        m_style_name  = ft_face->style_name;

        FT_Select_Charmap(ft_face, ft_encoding_unicode);
        FT_Set_Pixel_Sizes(ft_face, 0, height);

        m_glyphs[0];
        for (int i = 32; i < 126; i++) m_glyphs[i];

        int                       loaded     = 0;
        int                       max_width  = 0;
        int                       max_height = 0;
        std::vector<std::uint8_t> glyphs_rendered;

        for (auto& entry : m_glyphs) {
            auto& glyph = entry.second;

            if (FT_Load_Char(ft_face, entry.first, FT_LOAD_RENDER)) continue;

            FT_GlyphSlot ft_glyph = ft_face->glyph;

            glyph.code_point = entry.first;
            glyph.size       = Size2i(ft_glyph->bitmap.width, ft_glyph->bitmap.rows);
            glyph.bearing    = Vec2i(ft_glyph->bitmap_left, ft_glyph->bitmap_top);
            glyph.advance    = Vec2i(ft_glyph->advance.x >> GLYPHS_SIZE_SHIFT, ft_glyph->advance.y >> GLYPHS_SIZE_SHIFT);

            auto offset = glyphs_rendered.size();
            auto size   = glyph.size.x() * glyph.size.y();
            glyphs_rendered.resize(offset + size);
            std::memcpy(glyphs_rendered.data() + offset, ft_glyph->bitmap.buffer, size);

            max_width  = Math::max(max_width, glyph.size.x());
            max_height = Math::max(max_height, glyph.size.y());
            loaded += 1;
        }

        FT_Done_Face(ft_face);
        FT_Done_FreeType(ft_library);

        int bitmap_cols   = glyphs_in_row;
        int bitmap_rows   = loaded / bitmap_cols + (loaded % bitmap_cols ? 1 : 0);
        int bitmap_width  = bitmap_cols * max_width + Math::max(0, bitmap_cols - 1) * GLYPHS_BITMAP_OFFSET;
        int bitmap_height = bitmap_rows * max_height + Math::max(0, bitmap_rows - 1) * GLYPHS_BITMAP_OFFSET;
        int bitmap_size   = bitmap_width * bitmap_height;

        Ref<Image> bitmap = make_ref<Image>();
        bitmap->set_name(SID(get_name().str() + "_bitmap"));
        bitmap->create(bitmap_width, bitmap_height, 1, 1);
        auto* dst_ptr     = bitmap->get_pixel_data()->buffer();
        auto* src_ptr     = glyphs_rendered.data();
        int   read_offset = 0;
        int   count       = 0;

        std::memset(dst_ptr, 0, bitmap_size);

        for (auto& entry : m_glyphs) {
            if (entry.second.code_point < 0)
                continue;

            int row          = count / bitmap_cols;
            int col          = count % bitmap_cols;
            int write_offset = row * (max_height + GLYPHS_BITMAP_OFFSET) * bitmap_width + col * (max_width + GLYPHS_BITMAP_OFFSET);

            auto& glyph         = entry.second;
            glyph.bitmap_uv0[0] = static_cast<float>(col * (max_width + GLYPHS_BITMAP_OFFSET)) / static_cast<float>(bitmap_width);
            glyph.bitmap_uv0[1] = 1.0f - static_cast<float>(row * (max_height + GLYPHS_BITMAP_OFFSET)) / static_cast<float>(bitmap_height);
            glyph.bitmap_uv1[0] = glyph.bitmap_uv0[0] + static_cast<float>(glyph.size.x()) / static_cast<float>(bitmap_width);
            glyph.bitmap_uv1[1] = glyph.bitmap_uv0[1] - static_cast<float>(glyph.size.y()) / static_cast<float>(bitmap_height);

            for (int i = 0; i < glyph.size.y(); i++) {
                std::memcpy(dst_ptr + write_offset, src_ptr + read_offset, glyph.size.x());
                read_offset += glyph.size.x();
                write_offset += bitmap_width;
            }

            count += 1;
        }

        m_glyphs_in_row = glyphs_in_row;
        m_height        = height;
        m_max_width     = max_width;
        m_max_height    = max_height;

        GfxSamplerDesc sampler_desc;
        sampler_desc.brd_clr        = GfxSampBrdClr::Black;
        sampler_desc.mag_flt        = GfxSampFlt::Linear;
        sampler_desc.min_flt        = GfxSampFlt::LinearMipmapLinear;
        sampler_desc.max_anisotropy = Engine::instance()->gfx_driver()->device_caps().max_anisotropy;
        sampler_desc.u              = GfxSampAddress::ClampToBorder;
        sampler_desc.v              = GfxSampAddress::ClampToBorder;

        TexCompressionParams compression_params{};
        compression_params.format = TexCompressionFormat::BC4;

        m_texture = make_ref<Texture2d>(GfxFormat::R8, bitmap_width, bitmap_height, GfxTexSwizz::RRRRtoRGBA);
        m_texture->set_name(SID(get_name().str() + "_texture"));
        m_texture->set_sampler_from_desc(sampler_desc);
        m_texture->set_compression(compression_params);
        m_texture->set_source_images({bitmap});

        if (!m_texture->generate_mips()) {
            WG_LOG_ERROR("failed to gen font mips " << get_name());
            return StatusCode::Error;
        }
        if (!m_texture->generate_compressed_data()) {
            WG_LOG_ERROR("failed to compress font texture " << get_name());
            return StatusCode::Error;
        }
        if (!m_texture->generate_gfx_resource()) {
            WG_LOG_ERROR("failed to create gfx font texture " << get_name());
            return StatusCode::Error;
        }

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