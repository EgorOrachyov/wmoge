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

#include "freetype_font.hpp"

#include "gfx/gfx_driver.hpp"
#include "grc/image.hpp"
#include "grc/texture.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

#include <freetype/freetype.h>

namespace wmoge {

    Status FreetypeFont::load(const Ref<Font>& font, const std::string& path, int height, int glyphs_in_row) {
        WG_AUTO_PROFILE_ASSET("FreetypeFont::load");

        static const int GLYPHS_SIZE_SHIFT    = 6;
        static const int GLYPHS_BITMAP_OFFSET = 2;

        std::vector<std::uint8_t> ttf_data;
        if (!Engine::instance()->file_system()->read_file(path, ttf_data)) {
            WG_LOG_ERROR("failed to load font data from asset pak " << path);
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

        FontDesc font_desc;
        font_desc.family_name = ft_face->family_name;
        font_desc.style_name  = ft_face->style_name;

        FT_Select_Charmap(ft_face, ft_encoding_unicode);
        FT_Set_Pixel_Sizes(ft_face, 0, height);

        font_desc.glyphs[0] = FontGlyph();
        for (int i = 32; i < 126; i++) {
            font_desc.glyphs[i] = FontGlyph();
        }

        int                       loaded     = 0;
        int                       max_width  = 0;
        int                       max_height = 0;
        std::vector<std::uint8_t> glyphs_rendered;

        for (auto& entry : font_desc.glyphs) {
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
        bitmap->set_name(SID(font->get_name().str() + "_bitmap"));
        bitmap->create(bitmap_width, bitmap_height, 1, 1);
        auto* dst_ptr     = bitmap->get_pixel_data()->buffer();
        auto* src_ptr     = glyphs_rendered.data();
        int   read_offset = 0;
        int   count       = 0;

        std::memset(dst_ptr, 0, bitmap_size);

        for (auto& entry : font_desc.glyphs) {
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

        font_desc.glyphs_in_row = glyphs_in_row;
        font_desc.height        = height;
        font_desc.max_width     = max_width;
        font_desc.max_height    = max_height;

        GfxSamplerDesc sampler_desc;
        sampler_desc.brd_clr        = GfxSampBrdClr::Black;
        sampler_desc.mag_flt        = GfxSampFlt::Linear;
        sampler_desc.min_flt        = GfxSampFlt::LinearMipmapLinear;
        sampler_desc.max_anisotropy = Engine::instance()->gfx_driver()->device_caps().max_anisotropy;
        sampler_desc.u              = GfxSampAddress::ClampToBorder;
        sampler_desc.v              = GfxSampAddress::ClampToBorder;

        TexCompressionParams compression_params{};
        compression_params.format = TexCompressionFormat::BC4;

        font_desc.texture = make_ref<Texture2d>(GfxFormat::R8, bitmap_width, bitmap_height, GfxTexSwizz::RRRRtoRGBA);
        font_desc.texture->set_name(SID(font->get_name().str() + "_texture"));
        font_desc.texture->set_sampler_from_desc(sampler_desc);
        font_desc.texture->set_compression(compression_params);
        font_desc.texture->set_source_images({bitmap});

        if (!font_desc.texture->generate_mips()) {
            WG_LOG_ERROR("failed to gen font mips " << font->get_name());
            return StatusCode::Error;
        }
        if (!font_desc.texture->generate_compressed_data()) {
            WG_LOG_ERROR("failed to compress font texture " << font->get_name());
            return StatusCode::Error;
        }
        if (!font_desc.texture->generate_gfx_resource()) {
            WG_LOG_ERROR("failed to create gfx font texture " << font->get_name());
            return StatusCode::Error;
        }

        return font->init(font_desc);
    }

}// namespace wmoge