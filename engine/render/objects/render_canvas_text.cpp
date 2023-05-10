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

#include "render_canvas_text.hpp"

#include "debug/profiler.hpp"

#include <cstdlib>
#include <cstring>

namespace wmoge {

    void RenderCanvasText::initialize(std::string text, ref_ptr<Font> font, float font_size, FontTextAlignment alignment) {
        set_text(std::move(text));
        set_font(font);
        set_font_size(font_size);
        set_alignment(alignment);
    }
    void RenderCanvasText::set_text(std::string text) {
        m_text = std::move(text);
        mark_dirty_geom();
    }
    void RenderCanvasText::set_font(ref_ptr<Font> font) {
        m_font = std::move(font);
        mark_dirty_geom();

        const auto& font_texture = m_font->get_texture();
        m_material->set_texture(SID("font"), font_texture);
    }
    void RenderCanvasText::set_font_size(float font_size) {
        m_font_size = font_size;
        mark_dirty_geom();
    }
    void RenderCanvasText::set_alignment(FontTextAlignment alignment) {
        m_alignment = alignment;
        mark_dirty_geom();
    }

    void RenderCanvasText::on_scene_enter() {
        WG_AUTO_PROFILE_RENDER("RenderCanvasText::on_scene_enter");

        RenderObject::on_scene_enter();

        rebuild_geom();
        create_vert_format();
        create_draw_params();
        compile_draw_cmds(m_n_indices);

        m_dirty_geom = false;
    }
    void RenderCanvasText::on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) {
        WG_AUTO_PROFILE_RENDER("RenderCanvasText::on_render_dynamic");

        RenderCanvasItem::on_render_dynamic(views, mask);

        if (m_is_visible && m_dirty_geom) {
            rebuild_geom();
            compile_draw_cmds(m_n_indices);
            m_dirty_geom = false;
        }

        for (int view_idx = 0; view_idx < int(views.size()); view_idx++) {
            RenderView* view = views[view_idx];

            if (!mask[view_idx]) {
                continue;
            }
            if (!view->is_overlay_view()) {
                continue;
            }

            for (int cmd_idx = 0; cmd_idx < int(m_cached_cmds.size()); cmd_idx++) {
                DrawCmdSortingKey key = m_cached_keys[cmd_idx];
                DrawCmd*          cmd = m_cached_cmds[cmd_idx];
                view->add_cmd(key, cmd, DrawPass::Overlay2dPass);
            }
        }
    }
    bool RenderCanvasText::need_update() const {
        return false;
    }
    bool RenderCanvasText::need_render_dynamic() const {
        return true;
    }

    void RenderCanvasText::rebuild_geom() {
        WG_AUTO_PROFILE_RENDER("RenderCanvasText::rebuild_geom");

        const int n = int(m_text.size());

        if (!n) {
            WG_LOG_ERROR("passed empty string to draw");
            return;
        }

        Vec2f shift{};

        switch (m_alignment) {
            case FontTextAlignment::Left:
                break;
            case FontTextAlignment::Center:
                shift = m_font->get_string_size(m_text, m_font_size) * (-0.5f);
            default:
                break;
        }

        const float scale      = m_font_size > 0 ? m_font_size / float(m_font->get_height()) : 1.0f;
        const auto& bitmap     = m_font->get_bitmap();
        const auto& sampler    = m_font->get_sampler();
        const auto& glyphs     = m_font->get_glyphs();
        const auto  null_glyph = glyphs.find(0)->second;

        float advance_x  = 0.0f;
        int   n_vertices = 0;
        int   n_indices  = 0;

        std::vector<GfxVF_Pos2Uv2Col4> vertices;
        std::vector<std::uint16_t>     indices;

        vertices.reserve(n * N_VERTICES_PER_GLYPH);
        indices.reserve(n * N_INDICES_PER_GLYPH);

        for (int i = 0; i < n; ++i) {
            auto c     = m_text[i];
            auto query = glyphs.find(int(c));

            FontGlyph font_glyph = null_glyph;
            if (query != glyphs.end()) font_glyph = query->second;

            float left   = advance_x + scale * float(font_glyph.bearing.x());
            float top    = scale * float(font_glyph.bearing.y());
            float right  = left + scale * float(font_glyph.size.x());
            float bottom = top - scale * float(font_glyph.size.y());

            auto& p0 = vertices.emplace_back();
            auto& p1 = vertices.emplace_back();
            auto& p2 = vertices.emplace_back();
            auto& p3 = vertices.emplace_back();

            p0.pos = shift + Vec2f(left, top);
            p1.pos = shift + Vec2f(left, bottom);
            p2.pos = shift + Vec2f(right, bottom);
            p3.pos = shift + Vec2f(right, top);

            p0.uv = font_glyph.bitmap_uv0;
            p1.uv = Vec2f(font_glyph.bitmap_uv0.x(), font_glyph.bitmap_uv1.y());
            p2.uv = font_glyph.bitmap_uv1;
            p3.uv = Vec2f(font_glyph.bitmap_uv1.x(), font_glyph.bitmap_uv0.y());

            p0.col = Color::WHITE4f;
            p1.col = Color::WHITE4f;
            p2.col = Color::WHITE4f;
            p3.col = Color::WHITE4f;

            indices.push_back(n_vertices + 0);
            indices.push_back(n_vertices + 1);
            indices.push_back(n_vertices + 2);
            indices.push_back(n_vertices + 2);
            indices.push_back(n_vertices + 3);
            indices.push_back(n_vertices + 0);

            advance_x += scale * float(font_glyph.advance.x());
            n_vertices += N_VERTICES_PER_GLYPH;
            n_indices += N_INDICES_PER_GLYPH;
        }

        const int vert_buffer_size  = int(sizeof(GfxVF_Pos2Uv2Col4) * n_vertices);
        const int index_buffer_size = int(sizeof(std::uint16_t) * n_indices);

        m_vert_buffer  = m_driver->make_vert_buffer(vert_buffer_size, GfxMemUsage::GpuLocal, m_name);
        m_index_buffer = m_driver->make_index_buffer(index_buffer_size, GfxMemUsage::GpuLocal, m_name);

        std::memcpy(m_driver->map_vert_buffer(m_vert_buffer), vertices.data(), vert_buffer_size);
        m_driver->unmap_vert_buffer(m_vert_buffer);

        std::memcpy(m_driver->map_index_buffer(m_index_buffer), indices.data(), index_buffer_size);
        m_driver->unmap_index_buffer(m_index_buffer);

        m_n_indices = n_indices;
    }
    void RenderCanvasText::mark_dirty_geom() {
        m_dirty_geom = true;
    }

}// namespace wmoge