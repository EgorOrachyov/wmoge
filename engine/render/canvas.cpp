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

#include "canvas.hpp"

#include "core/string_utils.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/texture_manager.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

#include <cassert>

namespace wmoge {

    CanvasSharedData::CanvasSharedData() {
        Engine*    engine = Engine::instance();
        GfxDriver* driver = engine->gfx_driver();
        // ShaderManager* shader_manager = engine->shader_manager();

        GfxVertAttribs attribs_vert = {GfxVertAttrib::Pos2f, GfxVertAttrib::Col04f, GfxVertAttrib::Uv02f};
        GfxVertAttribs attribs_inst = {GfxVertAttrib::PrimitiveIdi};
        GfxVertAttribs attribs_full = attribs_vert | attribs_inst;

        GfxVertElements elements;
        elements.add_vert_attribs(attribs_vert, 0, false);
        elements.add_vert_attribs(attribs_inst, 1, true);

        GfxPsoStateGraphics pso_state{};
        pso_state.vert_format  = driver->make_vert_format(elements, SID("[pos2, col0, uv0, primitiveId]"));
        pso_state.depth_enable = false;
        pso_state.depth_write  = false;
        pso_state.blending     = true;

        // pso_state.shader = shader_manager->get_shader(SID("canvas"), attribs_full, {"OUT_SRGB"});
        // pipeline_srgb = driver->make_pso_graphics(pso_state, SID("canvas_srgb"));

        // pso_state.shader = shader_manager->get_shader(SID("canvas"), attribs_full, {});
        // pipeline_linear = driver->make_pso_graphics(pso_state, SID("canvas_linear"));
    }

    void CanvasSharedData::compile() {
        WG_AUTO_PROFILE_RENDER("CanvasSharedData::compile");

        Engine*         engine      = Engine::instance();
        GfxDriver*      gfx_drvier  = engine->gfx_driver();
        TextureManager* tex_manager = engine->texture_manager();

        // Expecting to fit all textures in single set (mathces most use cases)
        const int textures_count         = int(tex_buffer.size());
        const int textures_count_aligned = int(Math::align(textures_count, MAX_CANVAS_IMAGES));
        const int texture_gpoups         = textures_count_aligned / MAX_CANVAS_IMAGES;

        tex_sets.resize(texture_gpoups);

        for (int i = 0; i < texture_gpoups; i++) {
            GfxDescSetResources resources;

            for (int j = 0; j < MAX_CANVAS_IMAGES; j++) {
                const int textureIdx = i * MAX_CANVAS_IMAGES + j;

                auto& r     = resources.emplace_back();
                auto& point = r.first;
                auto& value = r.second;

                point.type          = GfxBindingType::SampledTexture;
                point.binding       = j;
                point.array_element = 0;

                if (textureIdx < tex_buffer.size()) {
                    const Ref<Texture2d> texture = tex_buffer[textureIdx];
                    value.resource               = texture->get_texture().as<GfxResource>();
                    value.sampler                = texture->get_sampler();
                } else {
                    value.resource = tex_manager->get_texture(DefaultTexture::White).as<GfxResource>();
                    value.sampler  = tex_manager->get_sampler(DefaultSampler::Default);
                }
            }

            const std::string name = "canvas idx=" + StringUtils::from_int(i);
            //  tex_sets[i]            = gfx_drvier->make_desc_set(resources, SID(name));
        }
    }

    void CanvasSharedData::clear() {
        tex_sets.clear();
        tex_buffer.clear();
    }

    Canvas::Canvas() : Canvas(std::make_shared<CanvasSharedData>()) {
    }

    Canvas::Canvas(std::shared_ptr<CanvasSharedData> shared) {
        assert(shared);

        m_shared = std::move(shared);
        m_vtx_buffer.set_name(SID("canvas_vtx"));
        m_idx_buffer.set_name(SID("canvas_idx"));
        m_prx_buffer.set_name(SID("canvas_primitives"));
        m_gpu_cmd_buffer.set_name(SID("canvas_cmds"));

        Engine*    engine = Engine::instance();
        GfxDriver* driver = engine->gfx_driver();
        // ShaderManager* shader_manager = engine->shader_manager();

        m_params = driver->make_uniform_buffer(int(sizeof(ShaderCanvas::Params)), GfxMemUsage::GpuLocal, SID("canvas_params"));

        clear();
    }

    void Canvas::push_clip_rect(const Vec4f& clip_rect) {
        m_clip_rect_stack.push_back(clip_rect);

        commit_draw_cmd();
    }
    void Canvas::pop_clip_rect() {
        m_clip_rect_stack.pop_back();

        commit_draw_cmd();
    }
    const Vec4f& Canvas::peek_clip_rect() {
        return m_clip_rect_stack.back();
    }

    void Canvas::push_transform(const Vec2f& translate, float rotate_rad, const Vec2f& scale) {
        push_transform(Math2d::translate_rotate_z(translate, rotate_rad) * Math2d::scale(scale));
    }
    void Canvas::push_transform(const Vec2f& translate, float rotate_rad) {
        push_transform(Math2d::translate_rotate_z(translate, rotate_rad));
    }
    void Canvas::push_transform(const Transform2d& transform) {
        push_transform(transform.get_transform());
    }
    void Canvas::push_transform(const Mat3x3f& matr) {
        const Mat3x3f prev = m_transform_stack.back();
        const Mat3x3f top  = prev * matr;
        m_transform_stack.push_back(top);

        commit_draw_cmd();
    }
    void Canvas::pop_transform() {
        m_transform_stack.pop_back();

        commit_draw_cmd();
    }
    const Mat3x3f& Canvas::peek_transform() {
        return m_transform_stack.back();
    }

    void Canvas::add_line(const Vec2f& p1, const Vec2f& p2, const Color4f& color, float thickness) {
        Vec2f points[] = {p1, p2};
        add_polyline(array_view<Vec2f>(points, 2), color, thickness);
    }
    void Canvas::add_rect(const Vec2f& p_min, const Vec2f& p_max, const Color4f& color, float rounding, CanvasFlags flags, float thickness) {
        if (!need_rounding(rounding, flags)) {
            path_line_to(Vec2f(p_min.x(), p_min.y()));
            path_line_to(Vec2f(p_min.x(), p_max.y()));
            path_line_to(Vec2f(p_max.x(), p_max.y()));
            path_line_to(Vec2f(p_max.x(), p_min.y()));
            path_line_to(Vec2f(p_min.x(), p_min.y()));
            path_stroke(color, thickness);
        } else {
            const bool  rounding_all = flags.get(CanvasFlag::Rounded);
            const float rounding_lu  = rounding_all || flags.get(CanvasFlag::RoundedCornerLU) ? rounding : 0.0f;
            const float rounding_ru  = rounding_all || flags.get(CanvasFlag::RoundedCornerRU) ? rounding : 0.0f;
            const float rounding_lb  = rounding_all || flags.get(CanvasFlag::RoundedCornerLB) ? rounding : 0.0f;
            const float rounding_rb  = rounding_all || flags.get(CanvasFlag::RoundedCornerRB) ? rounding : 0.0f;

            path_arc_to(Vec2f(p_min.x() + rounding_lu, p_max.y() - rounding_lu), rounding_lu, Math::PIf, Math::HALF_PIf);
            path_arc_to(Vec2f(p_max.x() - rounding_ru, p_max.y() - rounding_ru), rounding_ru, Math::HALF_PIf, 0);
            path_arc_to(Vec2f(p_max.x() - rounding_lb, p_min.y() + rounding_lb), rounding_lb, 0, -Math::HALF_PIf);
            path_arc_to(Vec2f(p_min.x() + rounding_rb, p_min.y() + rounding_rb), rounding_rb, -Math::HALF_PIf, Math::PIf);
            path_stroke(color, thickness);
        }
    }
    void Canvas::add_rect_filled(const Vec2f& p_min, const Vec2f& p_max, const Color4f& color, float rounding, CanvasFlags flags) {
        if (!need_rounding(rounding, flags)) {
            path_line_to(Vec2f(p_min.x(), p_min.y()));
            path_line_to(Vec2f(p_min.x(), p_max.y()));
            path_line_to(Vec2f(p_max.x(), p_max.y()));
            path_line_to(Vec2f(p_max.x(), p_min.y()));
            path_stroke_filled(color);
        } else {
            const bool  rounding_all = flags.get(CanvasFlag::Rounded);
            const float rounding_lu  = rounding_all || flags.get(CanvasFlag::RoundedCornerLU) ? rounding : 0.0f;
            const float rounding_ru  = rounding_all || flags.get(CanvasFlag::RoundedCornerRU) ? rounding : 0.0f;
            const float rounding_lb  = rounding_all || flags.get(CanvasFlag::RoundedCornerLB) ? rounding : 0.0f;
            const float rounding_rb  = rounding_all || flags.get(CanvasFlag::RoundedCornerRB) ? rounding : 0.0f;

            path_arc_to(Vec2f(p_min.x() + rounding_lu, p_max.y() - rounding_lu), rounding_lu, Math::PIf, Math::HALF_PIf);
            path_arc_to(Vec2f(p_max.x() - rounding_ru, p_max.y() - rounding_ru), rounding_ru, Math::HALF_PIf, 0);
            path_arc_to(Vec2f(p_max.x() - rounding_lb, p_min.y() + rounding_lb), rounding_lb, 0, -Math::HALF_PIf);
            path_arc_to(Vec2f(p_min.x() + rounding_rb, p_min.y() + rounding_rb), rounding_rb, -Math::HALF_PIf, Math::PIf);
            path_stroke_filled(color);
        }
    }
    void Canvas::add_triangle(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3, const Color4f& color, float thickness) {
    }
    void Canvas::add_triangle_filled(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3, const Color4f& color) {
    }
    void Canvas::add_polyline(const array_view<Vec2f>& points, const Color4f& color, float thickness) {
        set_texture();

        assert(points.size() > 1);
        const int n_lines = int(points.size()) - 1;

        reserve(n_lines * 4, n_lines * 2 * 3);

        for (int i = 0; i < n_lines; i++) {
            const Vec2f& from = points[i];
            const Vec2f& to   = points[i + 1];

            const Vec2f dir        = to - from;
            const float angle      = Math::atan2(dir.y(), dir.x());
            const float half_width = thickness * 0.5f;

            const Vec2f h = Vec2f(-Math::cos(angle), Math::sin(angle));

            const int start = m_vxt_current;

            write_vtx(from + h * half_width, Vec2f(0, 0), color);
            write_vtx(from + h * (-half_width), Vec2f(0, 0), color);
            write_vtx(to + h * half_width, Vec2f(0, 0), color);
            write_vtx(to + h * (-half_width), Vec2f(0, 0), color);

            write_idx(start + 0, start + 1, start + 3);
            write_idx(start + 3, start + 2, start + 0);
        }
    }
    void Canvas::add_polygone(const array_view<Vec2f>& points, const Color4f& color) {
        set_texture();

        assert(points.size() >= 3);

        const Vec2f& pivot = points[0];
        const int    n_vtx = int(points.size());
        const int    n_tri = n_vtx - 2;

        reserve(n_vtx, n_tri * 3);

        const int start = m_vxt_current;

        for (int i = 0; i < n_vtx; i++) {
            write_vtx(points[i], Vec2f(0, 0), color);
        }

        for (int i = 0; i < n_tri; i++) {
            write_idx(start + 0, start + i + 1, start + i + 2);
        }
    }
    void Canvas::add_text(const std::string& text, const Ref<Font>& font, float font_size, const Vec2f& p, const Color4f& color) {
        assert(font);

        if (text.empty()) { return; }

        set_texture(font->get_texture());

        const int   n          = int(text.size());
        const float scale      = font_size > 0 ? font_size / float(font->get_height()) : 1.0f;
        const auto& glyphs     = font->get_glyphs();
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

            reserve(4, 6);

            const int start = m_vxt_current;

            write_vtx(p + Vec2f(left, top), font_glyph.bitmap_uv0, color);
            write_vtx(p + Vec2f(left, bottom), Vec2f(font_glyph.bitmap_uv0.x(), font_glyph.bitmap_uv1.y()), color);
            write_vtx(p + Vec2f(right, bottom), font_glyph.bitmap_uv1, color);
            write_vtx(p + Vec2f(right, top), Vec2f(font_glyph.bitmap_uv1.x(), font_glyph.bitmap_uv0.y()), color);

            write_idx(start + 0, start + 1, start + 2);
            write_idx(start + 2, start + 3, start + 0);

            advance_x += scale * float(font_glyph.advance.x());
        }
    }
    void Canvas::add_image(const Ref<Texture2d>& image, const Vec2f& p_min, const Vec2f& p_max, const Vec2f& uv_min, const Vec2f& uv_max, const Color4f& color) {
        set_texture(image);

        Vec2f points[4];
        points[0] = Vec2f(p_min.x(), p_min.y());
        points[1] = Vec2f(p_min.x(), p_max.y());
        points[2] = Vec2f(p_max.x(), p_max.y());
        points[3] = Vec2f(p_max.x(), p_min.y());

        Vec2f uvs[4];
        uvs[0] = Vec2f(uv_min.x(), uv_min.y());
        uvs[1] = Vec2f(uv_min.x(), uv_max.y());
        uvs[2] = Vec2f(uv_max.x(), uv_max.y());
        uvs[3] = Vec2f(uv_max.x(), uv_min.y());

        reserve(4, 6);

        const int start = m_idx_current;

        for (int i = 0; i < 4; i++) {
            write_vtx(points[i], uvs[i], color);
        }

        write_idx(start + 0, start + 1, start + 2);
        write_idx(start + 2, start + 3, start + 0);
    }

    void Canvas::path_line_to(const Vec2f& p) {
        m_path.push_back(p);
    }
    void Canvas::path_arc_to(const Vec2f& p, float r, float start_rad, float end_rad, int num_segments) {
        assert(num_segments >= 0);

        if (r == 0.0f) {
            path_line_to(p);
            return;
        }

        const float angle = end_rad - start_rad;
        const int   n_seg = tessellate(angle, num_segments);
        const int   n_vtx = n_seg + 1;

        const float angle_diff = angle / float(n_seg);
        float       angle_curr = start_rad;

        for (int i = 1; i < n_vtx; i++) {
            path_line_to(Vec2f(p.x() + r * Math::cos(angle_curr), p.y() + r * Math::sin(angle_curr)));
            angle_curr += angle_diff;
        }
    }
    void Canvas::path_stroke(const Color4f& color, float thickness) {
        add_polyline(array_view<Vec2f>(m_path.data(), int(m_path.size())), color, thickness);
        path_clear();
    }
    void Canvas::path_stroke_filled(const Color4f& color) {
        add_polygone(array_view<Vec2f>(m_path.data(), int(m_path.size())), color);
        path_clear();
    }
    void Canvas::path_clear() {
        m_path.clear();
    }

    void Canvas::clear(bool clear_shared_data) {
        if (clear_shared_data) {
            m_shared->clear();
        }

        m_cmd_buffer.clear();
        m_vtx_buffer.clear();
        m_idx_buffer.clear();
        m_prx_buffer.clear();
        m_clip_rect_stack.clear();
        m_transform_stack.clear();

        m_vxt_current = 0;
        m_idx_current = 0;

        m_path.clear();
        m_clip_rect_stack.push_back(Vec4f(-20000.0f, -20000.0f, 20000.0f, 20000.0f));
        m_transform_stack.push_back(Math2d::identity3x3());

        commit_draw_cmd();
    }

    void Canvas::compile(bool compile_shared_data) {
        WG_AUTO_PROFILE_RENDER("Canvas::compile");

        Engine*    engine     = Engine::instance();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();
        GfxDriver* gfx_drvier = engine->gfx_driver();

        m_gpu_cmd_buffer.resize(m_cmd_buffer.size());

        for (int i = 0; i < int(m_cmd_buffer.size()); i++) {
            const CanvasDrawCmd& src_cmd = m_cmd_buffer[i];
            GPUCanvasDrawCmd&    dst_cmd = m_gpu_cmd_buffer[i];

            const Mat3x3f& t = src_cmd.transform;

            dst_cmd.ClipRect   = src_cmd.clip_rect;
            dst_cmd.TextureIdx = src_cmd.texture_idx;
            dst_cmd.Transform0 = Vec4f(t.col(0), 0.0f);
            dst_cmd.Transform1 = Vec4f(t.col(1), 0.0f);
            dst_cmd.Transform2 = Vec4f(t.col(2), 0.0f);
        }

        m_gpu_cmd_buffer.flush(gfx_ctx);
        m_vtx_buffer.flush(gfx_ctx);
        m_idx_buffer.flush(gfx_ctx);
        m_prx_buffer.flush(gfx_ctx);

        GfxDescSetResources resources;
        {
            {
                auto& asset    = resources.emplace_back();
                auto& point    = asset.first;
                point.type     = GfxBindingType::UniformBuffer;
                point.binding  = ShaderCanvas::PARAMS_SLOT;
                auto& value    = asset.second;
                value.resource = m_params.as<GfxResource>();
                value.offset   = 0;
                value.range    = m_params->size();
            }
            {
                auto& asset    = resources.emplace_back();
                auto& point    = asset.first;
                point.type     = GfxBindingType::StorageBuffer;
                point.binding  = ShaderCanvas::DRAWCMDSDATA_SLOT;
                auto& value    = asset.second;
                value.resource = m_gpu_cmd_buffer.get_buffer().as<GfxResource>();
                value.offset   = 0;
                value.range    = m_gpu_cmd_buffer.get_buffer()->size();
            }
        }
        // m_params_set = gfx_drvier->make_desc_set(resources, SID("canvas_params"));

        if (compile_shared_data) {
            m_shared->compile();
        }
    }

    void Canvas::render(const Ref<Window>& window, const Rect2i& viewport, const Vec4f& area, float gamma) {
        WG_AUTO_PROFILE_RENDER("Canvas::render");

        if (m_cmd_buffer.empty() || (m_cmd_buffer.size() == 1 && m_cmd_buffer[0].elements == 0)) {
            return;
        }

        Engine*    engine     = Engine::instance();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();
        GfxDriver* gfx_drvier = engine->gfx_driver();

        ShaderCanvas::Params& params = *((ShaderCanvas::Params*) gfx_ctx->map_uniform_buffer(m_params));
        {
            params.ClipProjView = (gfx_drvier->clip_matrix() * Math3d::orthographic(area.x(), area.z(), area.y(), area.w(), -1.0f, 1.0f)).transpose();
            params.InverseGamma = 1.0f / gamma;
        }
        gfx_ctx->unmap_uniform_buffer(m_params);

        gfx_ctx->execute([&](GfxCtx* thread_ctx) {
            thread_ctx->begin_render_pass({}, SID("Canvas::render"));
            thread_ctx->bind_target(window);
            thread_ctx->clear(0, Color::BLACK4f);// todo: remove
            thread_ctx->viewport(viewport);

            if (false /* thread_ctx->bind_pso(m_shared->pipeline_srgb) */) {
                thread_ctx->bind_desc_set(m_params_set, ShaderCanvas::PARAMS_SET);

                const int num_cmds = int(m_gpu_cmd_buffer.get_size());
                for (int cmd_id = 0; cmd_id < num_cmds; cmd_id++) {
                    const CanvasDrawCmd& cmd = m_cmd_buffer[cmd_id];

                    if (cmd.elements > 0) {
                        const int tex_set_idx = cmd.texture_idx >= 0 ? cmd.texture_idx / MAX_CANVAS_IMAGES : 0;

                        thread_ctx->bind_desc_set(m_shared->tex_sets[tex_set_idx], ShaderCanvas::CANVASIMAGE0_SET);
                        thread_ctx->bind_vert_buffer(m_vtx_buffer.get_buffer(), 0);
                        thread_ctx->bind_vert_buffer(m_prx_buffer.get_buffer(), 1, int(cmd_id * sizeof(int)));
                        thread_ctx->bind_index_buffer(m_idx_buffer.get_buffer(), GfxIndexType::Uint32, int(cmd.idx_offset * sizeof(std::uint32_t)));
                        thread_ctx->draw_indexed(cmd.elements * 3, 0, 1);
                    }
                }
            }

            thread_ctx->end_render_pass();
        });
    }

    void Canvas::set_texture() {
        const int texture_idx = -1;

        CanvasDrawCmd& cmd = m_cmd_buffer.back();
        if (cmd.texture_idx != texture_idx) {
            commit_draw_cmd();
            m_cmd_buffer.back().texture_idx = texture_idx;
        }
    }
    void Canvas::set_texture(const Ref<Texture2d>& tex) {
        assert(tex);

        int texture_idx = -1;

        auto it = m_shared->tex_map.find(tex);
        if (it != m_shared->tex_map.end()) {
            texture_idx = it->second;
        } else {
            texture_idx = int(m_shared->tex_buffer.size());
            m_shared->tex_buffer.push_back(tex);
            m_shared->tex_map[tex] = texture_idx;
        }

        CanvasDrawCmd& cmd = m_cmd_buffer.back();
        if (cmd.texture_idx != texture_idx) {
            commit_draw_cmd();
            m_cmd_buffer.back().texture_idx = texture_idx;
        }
    }
    void Canvas::commit_draw_cmd() {
        if (m_cmd_buffer.empty() || m_cmd_buffer.back().elements != 0) {
            m_prx_buffer.push_back(int(m_cmd_buffer.size()));
            m_cmd_buffer.emplace_back();
        }

        const int cmd_idx = int(m_cmd_buffer.size()) - 1;

        CanvasDrawCmd& cmd = m_cmd_buffer.back();
        cmd.idx_offset     = m_idx_current;
        cmd.vtx_offset     = m_vxt_current;
        cmd.clip_rect      = m_clip_rect_stack.back();
        cmd.transform      = m_transform_stack.back();
    }
    void Canvas::reserve(int num_vtx, int num_idx) {
        assert(num_vtx >= 0);
        assert(num_idx >= 0);

        m_vtx_buffer.resize(m_vtx_buffer.get_size() + num_vtx);
        m_idx_buffer.resize(m_idx_buffer.get_size() + num_idx);
    }
    void Canvas::write_vtx(const Vec2f& pos, const Vec2f& uv, const Vec4f& color) {
        CanvasVert vert;
        vert.pos                      = pos;
        vert.uv                       = uv;
        vert.color                    = color;
        m_vtx_buffer[m_vxt_current++] = vert;
    }
    void Canvas::write_idx(int i1, int i2, int i3) {
        m_cmd_buffer.back().elements += 1;
        m_idx_buffer[m_idx_current++] = std::uint32_t(i1);
        m_idx_buffer[m_idx_current++] = std::uint32_t(i2);
        m_idx_buffer[m_idx_current++] = std::uint32_t(i3);
    }
    int Canvas::tessellate(float angle, int num_segments) {
        if (num_segments == 0) {
            num_segments = int(Math::ceil(Math::abs(angle) / Math::PIf * float(NUM_SEGMENTS_PER_PI)));
        }
        return Math::max(num_segments, 1);
    }

    bool Canvas::need_rounding(float rounding, CanvasFlags flags) {
        CanvasFlags rounding_mask = {
                CanvasFlag::Rounded,
                CanvasFlag::RoundedCornerLU,
                CanvasFlag::RoundedCornerRU,
                CanvasFlag::RoundedCornerLB,
                CanvasFlag::RoundedCornerRB,
        };

        return rounding > 0.5f && (rounding_mask.bits & flags.bits).any();
    }

}// namespace wmoge