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

#include "aux_draw_canvas.hpp"

#include "core/engine.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "math/math_utils2d.hpp"
#include "math/math_utils3d.hpp"
#include "render/render_engine.hpp"
#include "render/shader_manager.hpp"
#include "resource/config_file.hpp"
#include "shaders/generated/auto_aux_draw_canvas_reflection.hpp"

#include <cassert>

namespace wmoge {

    AuxDrawCanvas::AuxDrawCanvas() {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::AuxDrawCanvas");

        auto engine     = Engine::instance();
        auto gfx_driver = engine->gfx_driver();
        auto gfx_ctx    = engine->gfx_ctx();

        m_transform_stack.push_back(Math2d::translate_rotate_z(Vec2f(), 0.0f));

        m_default_sampler = gfx_driver->make_sampler(GfxSamplerDesc{}, SID("default"));
        m_default_texture = gfx_driver->make_texture_2d(1, 1, 1, GfxFormat::RGBA8, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, SID("default_white"));

        unsigned char white[] = {0xff, 0xff, 0xff, 0xff};
        gfx_ctx->update_texture_2d(m_default_texture, 0, Rect2i(0, 0, 1, 1), make_ref<Data>(white, sizeof(white)));

        // b0_Pos2Uv2Col4;
        {
            GfxVertElements elements;
            elements.add_element(SID("pos"), GfxFormat::RG32F, 0, offsetof(GfxVF_Pos2Uv2Col4, pos), sizeof(GfxVF_Pos2Uv2Col4));
            elements.add_element(SID("uv"), GfxFormat::RG32F, 0, offsetof(GfxVF_Pos2Uv2Col4, uv), sizeof(GfxVF_Pos2Uv2Col4));
            elements.add_element(SID("col"), GfxFormat::RGBA32F, 0, offsetof(GfxVF_Pos2Uv2Col4, col), sizeof(GfxVF_Pos2Uv2Col4));
            m_b0_Pos2Uv2Col4 = gfx_driver->make_vert_format(elements, SID("b0_Pos2Uv2Col4"));
        }
    }

    void AuxDrawCanvas::push(const Vec2f& translate, float rotate_rad, const Vec2f& scale) {
        push(Math2d::translate_rotate_z(translate, rotate_rad) * Math2d::scale(scale));
    }
    void AuxDrawCanvas::push(const Vec2f& translate, float rotate_rad) {
        push(Math2d::translate_rotate_z(translate, rotate_rad));
    }
    void AuxDrawCanvas::push(const Transform2d& transform) {
        push(transform.get_transform());
    }
    void AuxDrawCanvas::push(const Mat3x3f& matr) {
        assert(m_transform_stack.size() > 0);
        auto new_transform = m_transform_stack.back() * matr;
        m_transform_stack.push_back(new_transform);
    }
    void AuxDrawCanvas::pop() {
        assert(m_transform_stack.size() > 1);
        m_transform_stack.pop_back();
    }
    const Mat3x3f& AuxDrawCanvas::peek() {
        assert(m_transform_stack.size() >= 1);
        return m_transform_stack.back();
    }
    void AuxDrawCanvas::set_line_color(const Color4f& color) {
        m_color_line = color;
    }
    void AuxDrawCanvas::set_fill_color(const Color4f& color) {
        m_color_fill = color;
    }
    void AuxDrawCanvas::set_font_color(const Color4f& color) {
        m_color_font = color;
    }
    void AuxDrawCanvas::set_tint_color(const Color4f& color) {
        m_color_tint = color;
    }
    void AuxDrawCanvas::set_font(Ref<Font> font) {
        m_font = std::move(font);
    }

    void AuxDrawCanvas::draw_line(const Vec2f& from, const Vec2f& to, float width) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_line");

        float x = to.x() - from.x();
        float y = to.y() - from.y();

        float angle      = Math::atan2(y, x);
        float half_width = width * 0.5f;

        push(from, angle);
        add_rect(Vec2f(0, -half_width), Vec2f(Vec2f(x, y).length(), width), m_color_line);
        pop();
    }
    void AuxDrawCanvas::draw_triangle(const Vec2f& p0, const Vec2f& p1, const Vec2f& p2, float border) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_triangle");

        draw_line(p0, p1, border);
        draw_line(p1, p2, border);
        draw_line(p2, p0, border);
    }
    void AuxDrawCanvas::draw_rect(const Vec2f& pos, const Vec2f& size, float border) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_rect");

        const Vec2f p0(pos.x(), pos.y() + size.y());
        const Vec2f p1(pos.x(), pos.y());
        const Vec2f p2(pos.x() + size.x(), pos.y());
        const Vec2f p3(pos.x() + size.x(), pos.y() + size.y());

        draw_line(p0, p1, border);
        draw_line(p1, p2, border);
        draw_line(p2, p3, border);
        draw_line(p3, p0, border);
    }
    void AuxDrawCanvas::draw_filled_triangle(const Vec2f& p0, const Vec2f& p1, const Vec2f& p2) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_filled_triangle");

        add_triangle(p0, p1, p2, m_color_fill);
    }
    void AuxDrawCanvas::draw_filled_rect(const Vec2f& pos, const Vec2f& size) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_filled_rect");

        const Vec2f p0(pos.x(), pos.y() + size.y());
        const Vec2f p1(pos.x(), pos.y());
        const Vec2f p2(pos.x() + size.x(), pos.y());
        const Vec2f p3(pos.x() + size.x(), pos.y() + size.y());

        draw_filled_triangle(p0, p1, p2);
        draw_filled_triangle(p2, p3, p0);
    }
    void AuxDrawCanvas::draw_texture(const Ref<Texture2d>& texture, const Vec2f& pos, const Vec2f& size, const Vec2f& uv_base, const Vec2f& uv_size) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_texture");

        assert(texture);

        const auto& mat         = m_transform_stack.back();
        const auto& gfx_texture = texture->get_texture();
        const auto& gfx_sampler = texture->get_sampler();

        Vec2f p[4];
        Vec2f t[4];

        p[0] = Math2d::transform(mat, Vec2f(pos.x(), pos.y() + size.y()));
        p[1] = Math2d::transform(mat, Vec2f(pos.x(), pos.y()));
        p[2] = Math2d::transform(mat, Vec2f(pos.x() + size.x(), pos.y()));
        p[3] = Math2d::transform(mat, Vec2f(pos.x() + size.x(), pos.y() + size.y()));

        t[0] = Vec2f(uv_base.x(), uv_base.y() + uv_size.y());
        t[1] = Vec2f(uv_base.x(), uv_base.y());
        t[2] = Vec2f(uv_base.x() + uv_size.x(), uv_base.y());
        t[3] = Vec2f(uv_base.x() + uv_size.x(), uv_base.y() + uv_size.y());

        auto& t1   = m_triangles.emplace_back();
        t1.p[0]    = p[0];
        t1.p[1]    = p[1];
        t1.p[2]    = p[2];
        t1.t[0]    = t[0];
        t1.t[1]    = t[1];
        t1.t[2]    = t[2];
        t1.color   = m_color_tint;
        t1.texture = gfx_texture;
        t1.sampler = gfx_sampler;
        t1.is_text = false;

        auto& t2   = m_triangles.emplace_back();
        t2.p[0]    = p[2];
        t2.p[1]    = p[3];
        t2.p[2]    = p[0];
        t2.t[0]    = t[2];
        t2.t[1]    = t[3];
        t2.t[2]    = t[0];
        t2.color   = m_color_tint;
        t2.texture = gfx_texture;
        t2.sampler = gfx_sampler;
        t2.is_text = false;
    }
    void AuxDrawCanvas::draw_text(const std::string& text, const Vec2f& pos, float size) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::draw_text");

        const int n = int(text.size());

        if (!n) {
            WG_LOG_WARNING("passed empty string to draw");
            return;
        }

        const auto& mat        = m_transform_stack.back();
        const float scale      = size > 0 ? size / float(m_font->get_height()) : 1.0f;
        const auto& bitmap     = m_font->get_bitmap();
        const auto& sampler    = m_font->get_sampler();
        const auto& glyphs     = m_font->get_glyphs();
        const auto  null_glyph = glyphs.find(0)->second;

        float advance_x = 0.0f;

        for (int i = 0; i < n; ++i) {
            auto c     = text[i];
            auto query = glyphs.find(int(c));

            FontGlyph font_glyph = null_glyph;
            if (query != glyphs.end()) font_glyph = query->second;

            float left   = advance_x + scale * float(font_glyph.bearing.x());
            float top    = scale * float(font_glyph.bearing.y());
            float right  = left + scale * float(font_glyph.size.x());
            float bottom = top - scale * float(font_glyph.size.y());

            Vec2f p[4];
            Vec2f t[4];

            p[0] = Math2d::transform(mat, pos + Vec2f(left, top));
            p[1] = Math2d::transform(mat, pos + Vec2f(left, bottom));
            p[2] = Math2d::transform(mat, pos + Vec2f(right, bottom));
            p[3] = Math2d::transform(mat, pos + Vec2f(right, top));

            t[0] = font_glyph.bitmap_uv0;
            t[1] = Vec2f(font_glyph.bitmap_uv0.x(), font_glyph.bitmap_uv1.y());
            t[2] = font_glyph.bitmap_uv1;
            t[3] = Vec2f(font_glyph.bitmap_uv1.x(), font_glyph.bitmap_uv0.y());

            auto& t1   = m_triangles.emplace_back();
            t1.p[0]    = p[0];
            t1.p[1]    = p[1];
            t1.p[2]    = p[2];
            t1.t[0]    = t[0];
            t1.t[1]    = t[1];
            t1.t[2]    = t[2];
            t1.color   = m_color_font;
            t1.texture = bitmap;
            t1.sampler = sampler;
            t1.is_text = true;

            auto& t2   = m_triangles.emplace_back();
            t2.p[0]    = p[2];
            t2.p[1]    = p[3];
            t2.p[2]    = p[0];
            t2.t[0]    = t[2];
            t2.t[1]    = t[3];
            t2.t[2]    = t[0];
            t2.color   = m_color_font;
            t2.texture = bitmap;
            t2.sampler = sampler;
            t2.is_text = true;

            advance_x += scale * float(font_glyph.advance.x());
        }
    }

    void AuxDrawCanvas::add_triangle(const Vec2f& p0, const Vec2f& p1, const Vec2f& p2, const Color4f& color) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::add_triangle");

        const auto& mat      = m_transform_stack.back();
        auto&       triangle = m_triangles.emplace_back();
        triangle.p[0]        = Math2d::transform(mat, p0);
        triangle.p[1]        = Math2d::transform(mat, p1);
        triangle.p[2]        = Math2d::transform(mat, p2);
        triangle.t[0]        = Vec2f(0, 0);
        triangle.t[1]        = Vec2f(1, 0);
        triangle.t[2]        = Vec2f(1, 1);
        triangle.color       = color;
        triangle.texture     = m_default_texture;
        triangle.sampler     = m_default_sampler;
    }
    void AuxDrawCanvas::add_rect(const Vec2f& pos, const Vec2f& size, const Color4f& color) {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::add_rect");

        const Vec2f p0(pos.x(), pos.y() + size.y());
        const Vec2f p1(pos.x(), pos.y());
        const Vec2f p2(pos.x() + size.x(), pos.y());
        const Vec2f p3(pos.x() + size.x(), pos.y() + size.y());

        add_triangle(p0, p1, p2, color);
        add_triangle(p2, p3, p0, color);
    }

    void AuxDrawCanvas::set_window(const Ref<Window>& window) {
        m_window = window;
    }
    void AuxDrawCanvas::set_viewport(const Rect2i& viewport) {
        m_viewport = viewport;
    }
    void AuxDrawCanvas::set_screen_size(const Vec2f& size) {
        m_screen_size = size;
    }

    void AuxDrawCanvas::render() {
        WG_AUTO_PROFILE_RENDER("AuxDrawCanvas::render");

        auto engine          = Engine::instance();
        auto gfx_driver      = engine->gfx_driver();
        auto gfx_ctx         = engine->gfx_ctx();
        auto shader_manager  = engine->shader_manager();
        auto gfx_dyn_vert    = gfx_driver->dyn_vert_buffer();
        auto gfx_dyn_uniform = gfx_driver->dyn_uniform_buffer();

        int num_of_verts_triangles = 3 * static_cast<int>(m_triangles.size());

        if (!num_of_verts_triangles) return;

        auto triangles = gfx_dyn_vert->allocate_n<GfxVF_Pos2Uv2Col4>(num_of_verts_triangles);

        if (num_of_verts_triangles > 0) {
            for (const auto& entry : m_triangles) {
                triangles.ptr->pos = entry.p[0];
                triangles.ptr->uv  = entry.t[0];
                triangles.ptr->col = entry.color;
                triangles.ptr++;
                triangles.ptr->pos = entry.p[1];
                triangles.ptr->uv  = entry.t[1];
                triangles.ptr->col = entry.color;
                triangles.ptr++;
                triangles.ptr->pos = entry.p[2];
                triangles.ptr->uv  = entry.t[2];
                triangles.ptr->col = entry.color;
                triangles.ptr++;
            }
        }

        gfx_dyn_vert->flush();

        auto constants                  = gfx_dyn_uniform->allocate<ShaderAuxDrawCanvas::Params>();
        constants.ptr->clip_proj_screen = (gfx_ctx->clip_matrix() * Math3d::orthographic(0.0f, m_screen_size.x(), 0, m_screen_size.y(), -1000.0f, 1000.0f)).transpose();
        constants.ptr->gamma            = 2.2f;
        constants.ptr->inverse_gamma    = 1.0f / 2.2f;

        gfx_dyn_uniform->flush();

        gfx_ctx->begin_render_pass(GfxRenderPassDesc{}, SID("aux_draw_canvas"));
        gfx_ctx->bind_target(m_window);
        gfx_ctx->viewport(m_viewport);

        GfxPipeline* prev_bound = nullptr;

        GfxPipelineState pipeline_state;
        pipeline_state.cull_mode    = GfxPolyCullMode::Disabled;
        pipeline_state.front_face   = GfxPolyFrontFace::CounterClockwise;
        pipeline_state.poly_mode    = GfxPolyMode::Fill;
        pipeline_state.depth_enable = false;
        pipeline_state.depth_write  = false;
        pipeline_state.prim_type    = GfxPrimType::Triangles;
        pipeline_state.vert_format  = m_b0_Pos2Uv2Col4;
        pipeline_state.blending     = true;

        pipeline_state.shader  = shader_manager->get_shader(SID("aux_draw_canvas"));
        auto pipeline_triangle = gfx_driver->make_pipeline(pipeline_state, SID("canvas_triangle"));

        pipeline_state.shader = shader_manager->get_shader(SID("aux_draw_canvas"), {"CANVAS_FONT_BITMAP"});
        auto pipeline_text    = gfx_driver->make_pipeline(pipeline_state, SID("canvas_text"));

        if (num_of_verts_triangles > 0) {
            int current_triangle_id = 0;
            while (current_triangle_id < m_triangles.size()) {
                const auto& current_triangle = m_triangles[current_triangle_id];
                int         next_triangle_id = current_triangle_id + 1;

                while (next_triangle_id < m_triangles.size()) {
                    const auto& next_triangle = m_triangles[next_triangle_id];

                    if (current_triangle.texture != next_triangle.texture ||
                        current_triangle.sampler != next_triangle.sampler ||
                        current_triangle.is_text != next_triangle.is_text) {
                        break;
                    }

                    next_triangle_id += 1;
                }

                int batch_size   = next_triangle_id - current_triangle_id;
                int start_vertex = 3 * current_triangle_id;
                int vertex_count = batch_size * 3;

                bool              is_text = current_triangle.is_text;
                Ref<GfxPipeline>& to_bind = is_text ? pipeline_text : pipeline_triangle;

                if (to_bind.get() == prev_bound || gfx_ctx->bind_pipeline(to_bind)) {
                    gfx_ctx->bind_vert_buffer(Ref<GfxVertBuffer>(triangles.buffer), 0, triangles.offset);
                    gfx_ctx->bind_uniform_buffer(ShaderAuxDrawCanvas::PARAMS_LOC, constants.offset, sizeof(ShaderAuxDrawCanvas::Params), Ref<GfxUniformBuffer>(constants.buffer));
                    gfx_ctx->bind_texture(ShaderAuxDrawCanvas::TEXTURE_LOC, 0, current_triangle.texture, current_triangle.sampler);
                    gfx_ctx->draw(vertex_count, start_vertex, 1);
                    prev_bound = to_bind.get();
                }

                current_triangle_id = next_triangle_id;
            }
        }

        gfx_ctx->end_render_pass();

        m_triangles.clear();

        gfx_dyn_vert->recycle();
        gfx_dyn_uniform->recycle();
    }

}// namespace wmoge
