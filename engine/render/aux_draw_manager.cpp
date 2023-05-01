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

#include "aux_draw_manager.hpp"

#include "core/engine.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "math/math_utils3d.hpp"
#include "render/render_engine.hpp"
#include "render/shader_manager.hpp"
#include "resource/config_file.hpp"
#include "resource/resource_manager.hpp"

namespace wmoge {

    struct AuxDrawConstants {
        Mat4x4f clip_proj_view;
        Mat4x4f clip_proj_screen;
    };

    static_assert(sizeof(AuxDrawConstants) == (2 * 4 * 4) * sizeof(float), "unexpected size");

    AuxDrawManager::AuxDrawManager() {
        WG_AUTO_PROFILE_RENDER();

        auto engine           = Engine::instance();
        auto gfx_driver       = engine->gfx_driver();
        auto resource_manager = engine->resource_manager();
        auto config_engine    = engine->config_engine();
        auto shader_manager   = engine->render_engine()->get_shader_manager();

        m_shader_geom                 = shader_manager->get_shader_aux_geom();
        ShaderVariant* m_variant_geom = m_shader_geom->create_variant({});

        m_shader_text                 = shader_manager->get_shader_aux_text();
        ShaderVariant* m_variant_text = m_shader_text->create_variant({});

        std::string font_name = config_engine->get_string(SID("render.aux.font"), "res://fonts/consolas");

        m_debug_font  = resource_manager->load(SID(font_name)).cast<Font>();
        m_constants   = gfx_driver->make_uniform_buffer(sizeof(AuxDrawConstants), GfxMemUsage::GpuLocal, SID("aux_constants"));
        m_render_pass = gfx_driver->make_render_pass(GfxRenderPassType::AuxDraw, SID("aux_draw"));

        ref_ptr<GfxVertFormat> b0_Pos3Col3;
        {
            GfxVertElements elements;
            elements.add_element(SID("pos"), GfxFormat::RGB32F, 0, offsetof(GfxVF_Pos3Col3, pos), sizeof(GfxVF_Pos3Col3));
            elements.add_element(SID("col"), GfxFormat::RGB32F, 0, offsetof(GfxVF_Pos3Col3, col), sizeof(GfxVF_Pos3Col3));
            b0_Pos3Col3 = gfx_driver->make_vert_format(elements, SID("b0_Pos3Col3"));
        }

        ref_ptr<GfxVertFormat> b0_Pos2Uv2Col3;
        {
            GfxVertElements elements;
            elements.add_element(SID("pos"), GfxFormat::RG32F, 0, offsetof(GfxVF_Pos2Uv2Col3, pos), sizeof(GfxVF_Pos2Uv2Col3));
            elements.add_element(SID("uv"), GfxFormat::RG32F, 0, offsetof(GfxVF_Pos2Uv2Col3, uv), sizeof(GfxVF_Pos2Uv2Col3));
            elements.add_element(SID("col"), GfxFormat::RGB32F, 0, offsetof(GfxVF_Pos2Uv2Col3, col), sizeof(GfxVF_Pos2Uv2Col3));
            b0_Pos2Uv2Col3 = gfx_driver->make_vert_format(elements, SID("b0_Pos2Uv2Col3"));
        }

        GfxPipelineState pipeline_state;
        pipeline_state.pass = m_render_pass;

        pipeline_state.shader       = m_variant_geom->get_gfx_shader();
        pipeline_state.depth_enable = true;
        pipeline_state.depth_write  = true;
        pipeline_state.vert_format  = b0_Pos3Col3;
        pipeline_state.blending     = false;

        pipeline_state.cull_mode  = GfxPolyCullMode::Disabled;
        pipeline_state.front_face = GfxPolyFrontFace::CounterClockwise;
        pipeline_state.prim_type  = GfxPrimType::Lines;
        pipeline_state.poly_mode  = GfxPolyMode::Fill;
        m_pipeline_line           = gfx_driver->make_pipeline(pipeline_state, SID("aux_line"));

        pipeline_state.cull_mode  = GfxPolyCullMode::Back;
        pipeline_state.front_face = GfxPolyFrontFace::CounterClockwise;
        pipeline_state.prim_type  = GfxPrimType::Triangles;
        pipeline_state.poly_mode  = GfxPolyMode::Fill;
        m_pipeline_solid          = gfx_driver->make_pipeline(pipeline_state, SID("aux_solid"));

        pipeline_state.cull_mode  = GfxPolyCullMode::Disabled;
        pipeline_state.front_face = GfxPolyFrontFace::CounterClockwise;
        pipeline_state.prim_type  = GfxPrimType::Triangles;
        pipeline_state.poly_mode  = GfxPolyMode::Line;
        m_pipeline_wireframe      = gfx_driver->make_pipeline(pipeline_state, SID("aux_wireframe"));

        pipeline_state.depth_enable = false;
        pipeline_state.depth_write  = false;
        pipeline_state.cull_mode    = GfxPolyCullMode::Disabled;
        pipeline_state.front_face   = GfxPolyFrontFace::CounterClockwise;
        pipeline_state.prim_type    = GfxPrimType::Triangles;
        pipeline_state.poly_mode    = GfxPolyMode::Fill;

        pipeline_state.shader      = m_variant_text->get_gfx_shader();
        pipeline_state.vert_format = b0_Pos2Uv2Col3;
        pipeline_state.blending    = true;
        m_pipeline_glyphs          = gfx_driver->make_pipeline(pipeline_state, SID("aux_text"));
    }

    void AuxDrawManager::draw_line(const Vec3f& from, const Vec3f& to, const Color3f& color) {
        auto& line = m_lines.emplace_back();
        line.from  = from;
        line.to    = to;
        line.color = color;
    }
    void AuxDrawManager::draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color3f& color, bool solid) {
        if (solid) {
            auto& triangle = m_triangles_solid.emplace_back();
            triangle.p[0]  = p0;
            triangle.p[1]  = p1;
            triangle.p[2]  = p2;
            triangle.color = color;
        } else {
            auto& triangle = m_triangles_wire.emplace_back();
            triangle.p[0]  = p0;
            triangle.p[1]  = p1;
            triangle.p[2]  = p2;
            triangle.color = color;
        }
    }
    void AuxDrawManager::draw_sphere(const Vec3f& pos, float radius, const Color3f& color, bool solid) {
        WG_AUTO_PROFILE_RENDER();

        const int steps_v = MAX_SPLIT_STEP_SPHERE;
        const int steps_h = MAX_SPLIT_STEP_SPHERE;
        const int total_v = steps_v + 1;
        const int total_h = steps_h + 1;
        const int nv      = total_v * total_h;

        auto da_v = Math::PIf / float(steps_v);
        auto da_h = 2.0f * Math::PIf / float(steps_h);

        Vec3f vertices[nv];
        int   vertex_id = 0;

        for (int i = 0; i < total_v; ++i) {
            auto a_v = da_v * float(i) - Math::HALF_PIf;
            for (int j = 0; j < total_h; ++j) {
                auto a_h = da_h * float(j);

                auto r_xz = radius * Math::cos(a_v);
                auto x    = r_xz * Math::cos(a_h);
                auto z    = r_xz * Math::sin(a_h);
                auto y    = radius * Math::sin(a_v);

                vertices[vertex_id++] = pos + Vec3f(x, y, z);
            }
        }

        for (int i = 0; i < steps_v; ++i) {
            for (int j = 0; j < steps_h; ++j) {
                draw_triangle(vertices[i * total_h + j + 1], vertices[i * total_h + j + 0], vertices[i * total_h + j + total_h], color, solid);
                draw_triangle(vertices[i * total_h + j + total_h], vertices[i * total_h + j + total_h + 1], vertices[i * total_h + j + 1], color, solid);
            }
        }
    }
    void AuxDrawManager::draw_cylinder(const Vec3f& pos, float radius, float height, const Color3f& color, const Quatf& rot, bool solid) {
        WG_AUTO_PROFILE_RENDER();

        const int nv            = MAX_SPLIT_STEP_CYLINDER * 2 + 2;
        const int v_center_down = MAX_SPLIT_STEP_CYLINDER * 2 + 0;
        const int v_center_top  = MAX_SPLIT_STEP_CYLINDER * 2 + 1;

        Vec3f vertices[nv];

        float dangle = Math::PIf / float(MAX_SPLIT_STEP_CYLINDER / 2);
        float angle  = 0.0f;
        for (int i = 0; i < MAX_SPLIT_STEP_CYLINDER; ++i) {
            float rx = radius * Math::cos(angle);
            float rz = radius * Math::sin(angle);

            vertices[i]                           = pos + rot.rotate(Vec3f(rx, -height * 0.5f, rz));
            vertices[i + MAX_SPLIT_STEP_CYLINDER] = pos + rot.rotate(Vec3f(rx, height * 0.5f, rz));

            angle += dangle;
        }

        vertices[v_center_down] = pos + rot.rotate(Vec3f(0.0f, -height * 0.5f, 0.0f));
        vertices[v_center_top]  = pos + rot.rotate(Vec3f(0.0f, height * 0.5f, 0.0f));

        for (int i = 0; i < MAX_SPLIT_STEP_CYLINDER; ++i) {
            int v_dr = (i + 0) % MAX_SPLIT_STEP_CYLINDER;
            int v_dl = (i + 1) % MAX_SPLIT_STEP_CYLINDER;
            int v_tr = MAX_SPLIT_STEP_CYLINDER + (i + 0) % MAX_SPLIT_STEP_CYLINDER;
            int v_tl = MAX_SPLIT_STEP_CYLINDER + (i + 1) % MAX_SPLIT_STEP_CYLINDER;

            draw_triangle(vertices[v_dr], vertices[v_dl], vertices[v_center_down], color, solid);
            draw_triangle(vertices[v_dl], vertices[v_dr], vertices[v_tr], color, solid);
            draw_triangle(vertices[v_tr], vertices[v_tl], vertices[v_dl], color, solid);
            draw_triangle(vertices[v_tl], vertices[v_tr], vertices[v_center_top], color, solid);
        }
    }
    void AuxDrawManager::draw_cone(const Vec3f& pos, float radius, float height, const Color3f& color, const Quatf& rot, bool solid) {
        WG_AUTO_PROFILE_RENDER();

        const int nv       = MAX_SPLIT_STEP_CONE + 2;
        const int v_center = MAX_SPLIT_STEP_CONE + 0;
        const int v_top    = MAX_SPLIT_STEP_CONE + 1;

        Vec3f vertices[nv];

        float dangle = Math::PIf / float(MAX_SPLIT_STEP_CONE / 2);
        float angle  = 0.0f;
        for (int i = 0; i < MAX_SPLIT_STEP_CONE; ++i) {
            float rx = radius * Math::cos(angle);
            float rz = radius * Math::sin(angle);

            vertices[i] = pos + rot.rotate(Vec3f(rx, -height * 0.5f, rz));

            angle += dangle;
        }

        vertices[v_center] = pos + rot.rotate(Vec3f(0.0f, -height * 0.5f, 0.0f));
        vertices[v_top]    = pos + rot.rotate(Vec3f(0.0f, height * 0.5f, 0.0f));

        for (int i = 0; i < MAX_SPLIT_STEP_CONE; ++i) {
            int v_dr = (i + 0) % MAX_SPLIT_STEP_CONE;
            int v_dl = (i + 1) % MAX_SPLIT_STEP_CONE;

            draw_triangle(vertices[v_dr], vertices[v_dl], vertices[v_center], color, solid);
            draw_triangle(vertices[v_dl], vertices[v_dr], vertices[v_top], color, solid);
        }
    }
    void AuxDrawManager::draw_box(const Vec3f& pos, const Vec3f& size, const Color3f& color, const Quatf& rot, bool solid) {
        WG_AUTO_PROFILE_RENDER();

        const int nv = 8;

        Vec3f vertices[nv];

        float hx = size[0] * 0.5f;
        float hy = size[1] * 0.5f;
        float hz = size[2] * 0.5f;

        vertices[0] = pos + rot.rotate(Vec3f(-hx, hy, hz));
        vertices[1] = pos + rot.rotate(Vec3f(-hx, -hy, hz));
        vertices[2] = pos + rot.rotate(Vec3f(hx, -hy, hz));
        vertices[3] = pos + rot.rotate(Vec3f(hx, hy, hz));
        vertices[4] = pos + rot.rotate(Vec3f(-hx, hy, -hz));
        vertices[5] = pos + rot.rotate(Vec3f(-hx, -hy, -hz));
        vertices[6] = pos + rot.rotate(Vec3f(hx, -hy, -hz));
        vertices[7] = pos + rot.rotate(Vec3f(hx, hy, -hz));

        draw_triangle(vertices[0], vertices[1], vertices[2], color, solid);
        draw_triangle(vertices[2], vertices[3], vertices[0], color, solid);

        draw_triangle(vertices[3], vertices[2], vertices[7], color, solid);
        draw_triangle(vertices[7], vertices[2], vertices[6], color, solid);

        draw_triangle(vertices[7], vertices[6], vertices[5], color, solid);
        draw_triangle(vertices[5], vertices[4], vertices[7], color, solid);

        draw_triangle(vertices[4], vertices[5], vertices[0], color, solid);
        draw_triangle(vertices[0], vertices[5], vertices[1], color, solid);

        draw_triangle(vertices[4], vertices[0], vertices[7], color, solid);
        draw_triangle(vertices[7], vertices[0], vertices[3], color, solid);

        draw_triangle(vertices[1], vertices[5], vertices[2], color, solid);
        draw_triangle(vertices[2], vertices[5], vertices[6], color, solid);
    }
    void AuxDrawManager::draw_text_3d(const std::string& text, const Vec3f& pos, float size, const Color3f& color) {
        WG_AUTO_PROFILE_RENDER();

        auto world_pos  = m_proj * m_view * Vec4f(pos, 1.0f);
        auto ndc_pos    = world_pos / world_pos.w();
        auto screen_pos = m_screen_size * (Vec2f(ndc_pos.x(), ndc_pos.y()) * 0.5f + Vec2f(0.5f, 0.5f));

        draw_text_2d(text, screen_pos, size, color);
    }
    void AuxDrawManager::draw_text_2d(const std::string& text, const Vec2f& pos, float size, const Color3f& color) {
        WG_AUTO_PROFILE_RENDER();

        const int n = int(text.size());

        if (!n) {
            WG_LOG_WARNING("passed empty string to draw");
            return;
        }

        const float scale      = size > 0 ? size / float(m_debug_font->get_height()) : 1.0f;
        const auto& glyphs     = m_debug_font->get_glyphs();
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

            AuxDrawGlyph glyph;
            glyph.p[0]  = pos + Vec2f(left, top);
            glyph.t[0]  = font_glyph.bitmap_uv0;
            glyph.p[1]  = pos + Vec2f(left, bottom);
            glyph.t[1]  = Vec2f(font_glyph.bitmap_uv0.x(), font_glyph.bitmap_uv1.y());
            glyph.p[2]  = pos + Vec2f(right, bottom);
            glyph.t[2]  = font_glyph.bitmap_uv1;
            glyph.p[3]  = pos + Vec2f(right, top);
            glyph.t[3]  = Vec2f(font_glyph.bitmap_uv1.x(), font_glyph.bitmap_uv0.y());
            glyph.color = color;

            m_glyphs.push_back(glyph);

            advance_x += scale * float(font_glyph.advance.x());
        }
    }

    void AuxDrawManager::set_window(const ref_ptr<Window>& window) {
        m_window = window;
    }
    void AuxDrawManager::set_font(const ref_ptr<Font>& font) {
        m_debug_font = font;
    }
    void AuxDrawManager::set_projection(const Mat4x4f& projection) {
        m_proj = projection;
    }
    void AuxDrawManager::set_view(const Mat4x4f& view) {
        m_view = view;
    }
    void AuxDrawManager::set_viewport(const Rect2i& viewport) {
        m_viewport = viewport;
    }
    void AuxDrawManager::set_screen_size(const Vec2f& size) {
        m_screen_size = size;
    }

    void AuxDrawManager::render() {
        WG_AUTO_PROFILE_RENDER();

        auto gfx = Engine::instance()->gfx_driver();

        int num_of_verts_lines           = 2 * static_cast<int>(m_lines.size());
        int num_of_verts_triangles_solid = 3 * static_cast<int>(m_triangles_solid.size());
        int num_of_verts_triangles_wire  = 3 * static_cast<int>(m_triangles_wire.size());
        int num_of_verts_glyphs          = 6 * static_cast<int>(m_glyphs.size());

        if (m_gfx_capacity_lines < num_of_verts_lines) {
            m_gfx_lines          = gfx->make_vert_buffer(num_of_verts_lines * sizeof(GfxVF_Pos3Col3), GfxMemUsage::GpuLocal, SID("aux_lines"));
            m_gfx_capacity_lines = num_of_verts_lines;
        }
        if (m_gfx_capacity_triangles_solid < num_of_verts_triangles_solid) {
            m_gfx_triangles_solid          = gfx->make_vert_buffer(num_of_verts_triangles_solid * sizeof(GfxVF_Pos3Col3), GfxMemUsage::GpuLocal, SID("aux_tria_solid"));
            m_gfx_capacity_triangles_solid = num_of_verts_triangles_solid;
        }
        if (m_gfx_capacity_triangles_wire < num_of_verts_triangles_wire) {
            m_gfx_triangles_wire          = gfx->make_vert_buffer(num_of_verts_triangles_wire * sizeof(GfxVF_Pos3Col3), GfxMemUsage::GpuLocal, SID("aux_tria_wire"));
            m_gfx_capacity_triangles_wire = num_of_verts_triangles_wire;
        }
        if (m_gfx_capacity_text < num_of_verts_glyphs) {
            m_gfx_glyphs        = gfx->make_vert_buffer(num_of_verts_glyphs * sizeof(GfxVF_Pos2Uv2Col3), GfxMemUsage::GpuLocal, SID("aux_text"));
            m_gfx_capacity_text = num_of_verts_glyphs;
        }

        if (num_of_verts_lines > 0) {
            auto* p_lines = reinterpret_cast<GfxVF_Pos3Col3*>(gfx->map_vert_buffer(m_gfx_lines));
            for (const auto& entry : m_lines) {
                p_lines->pos = entry.from;
                p_lines->col = entry.color;
                p_lines++;
                p_lines->pos = entry.to;
                p_lines->col = entry.color;
                p_lines++;
            }
            gfx->unmap_vert_buffer(m_gfx_lines);
        }
        if (num_of_verts_triangles_solid > 0) {
            auto* p_tria_solid = reinterpret_cast<GfxVF_Pos3Col3*>(gfx->map_vert_buffer(m_gfx_triangles_solid));
            for (const auto& entry : m_triangles_solid) {
                p_tria_solid->pos = entry.p[0];
                p_tria_solid->col = entry.color;
                p_tria_solid++;
                p_tria_solid->pos = entry.p[1];
                p_tria_solid->col = entry.color;
                p_tria_solid++;
                p_tria_solid->pos = entry.p[2];
                p_tria_solid->col = entry.color;
                p_tria_solid++;
            }
            gfx->unmap_vert_buffer(m_gfx_triangles_solid);
        }
        if (num_of_verts_triangles_wire > 0) {
            auto* p_tria_wire = reinterpret_cast<GfxVF_Pos3Col3*>(gfx->map_vert_buffer(m_gfx_triangles_wire));
            for (const auto& entry : m_triangles_wire) {
                p_tria_wire->pos = entry.p[0];
                p_tria_wire->col = entry.color;
                p_tria_wire++;
                p_tria_wire->pos = entry.p[1];
                p_tria_wire->col = entry.color;
                p_tria_wire++;
                p_tria_wire->pos = entry.p[2];
                p_tria_wire->col = entry.color;
                p_tria_wire++;
            }
            gfx->unmap_vert_buffer(m_gfx_triangles_wire);
        }
        if (num_of_verts_glyphs > 0) {
            auto* p_glyphs = reinterpret_cast<GfxVF_Pos2Uv2Col3*>(gfx->map_vert_buffer(m_gfx_glyphs));
            for (const auto& entry : m_glyphs) {
                p_glyphs->pos = entry.p[0];
                p_glyphs->uv  = entry.t[0];
                p_glyphs->col = entry.color;
                p_glyphs++;
                p_glyphs->pos = entry.p[1];
                p_glyphs->uv  = entry.t[1];
                p_glyphs->col = entry.color;
                p_glyphs++;
                p_glyphs->pos = entry.p[2];
                p_glyphs->uv  = entry.t[2];
                p_glyphs->col = entry.color;
                p_glyphs++;
                p_glyphs->pos = entry.p[2];
                p_glyphs->uv  = entry.t[2];
                p_glyphs->col = entry.color;
                p_glyphs++;
                p_glyphs->pos = entry.p[3];
                p_glyphs->uv  = entry.t[3];
                p_glyphs->col = entry.color;
                p_glyphs++;
                p_glyphs->pos = entry.p[0];
                p_glyphs->uv  = entry.t[0];
                p_glyphs->col = entry.color;
                p_glyphs++;
            }
            gfx->unmap_vert_buffer(m_gfx_glyphs);
        }

        m_lines.clear();
        m_triangles_solid.clear();
        m_triangles_wire.clear();
        m_glyphs.clear();

        auto ptr              = reinterpret_cast<AuxDrawConstants*>(gfx->map_uniform_buffer(m_constants));
        ptr->clip_proj_view   = (gfx->clip_matrix() * m_proj * m_view).transpose();
        ptr->clip_proj_screen = (gfx->clip_matrix() * Math3d::orthographic(0.0f, m_screen_size.x(), 0, m_screen_size.y(), -1000.0f, 1000.0f)).transpose();
        gfx->unmap_uniform_buffer(m_constants);

        static const StringId PARAM_CONSTANTS   = SID("Constants");
        static const StringId PARAM_FONT_BITMAP = SID("FontBitmap");

        gfx->begin_render_pass(m_render_pass);
        gfx->bind_target(m_window);
        gfx->viewport(m_viewport);
        gfx->clear(1.0f, 0);// todo: clear in scene rendering
        gfx->clear(0, Vec4f(0, 0, 0, 1));

        if (num_of_verts_lines > 0 && gfx->bind_pipeline(m_pipeline_line)) {
            gfx->bind_vert_buffer(m_gfx_lines, 0);
            gfx->bind_uniform_buffer(PARAM_CONSTANTS, 0, sizeof(AuxDrawConstants), m_constants);
            gfx->draw(num_of_verts_lines, 0, 1);
        }
        if (num_of_verts_triangles_solid > 0 && gfx->bind_pipeline(m_pipeline_solid)) {
            gfx->bind_vert_buffer(m_gfx_triangles_solid, 0);
            gfx->bind_uniform_buffer(PARAM_CONSTANTS, 0, sizeof(AuxDrawConstants), m_constants);
            gfx->draw(num_of_verts_triangles_solid, 0, 1);
        }
        if (num_of_verts_triangles_wire > 0 && gfx->bind_pipeline(m_pipeline_wireframe)) {
            gfx->bind_vert_buffer(m_gfx_triangles_wire, 0);
            gfx->bind_uniform_buffer(PARAM_CONSTANTS, 0, sizeof(AuxDrawConstants), m_constants);
            gfx->draw(num_of_verts_triangles_wire, 0, 1);
        }
        if (num_of_verts_glyphs > 0 && gfx->bind_pipeline(m_pipeline_glyphs)) {
            gfx->bind_vert_buffer(m_gfx_glyphs, 0);
            gfx->bind_uniform_buffer(PARAM_CONSTANTS, 0, sizeof(AuxDrawConstants), m_constants);
            gfx->bind_texture(PARAM_FONT_BITMAP, 0, m_debug_font->get_bitmap(), m_debug_font->get_sampler());
            gfx->draw(num_of_verts_glyphs, 0, 1);
        }

        gfx->end_render_pass();
    }

}// namespace wmoge