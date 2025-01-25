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

#include "asset/asset_manager.hpp"
#include "core/buffered_vector.hpp"
#include "core/log.hpp"
#include "gfx/gfx_driver.hpp"
#include "gpu/gpu_utils.hpp"
#include "math/math_utils3d.hpp"
#include "rdg/rdg_profiling.hpp"
#include "rdg/rdg_utils.hpp"
#include "render/shader_table.hpp"

#include <algorithm>
#include <utility>

namespace wmoge {

    void AuxDrawDevice::draw_line(const Vec3f& from, const Vec3f& to, const Color4f& color) {
        add_line(from, to, color);
        add_elem();
    }

    void AuxDrawDevice::draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color) {
        add_triangle(p0, p1, p2, color);
        add_elem();
    }

    void AuxDrawDevice::draw_triangle_solid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color) {
        add_triangle_solid(p0, p1, p2, color);
        add_elem_solid();
    }

    void AuxDrawDevice::draw_mesh(array_view<const Vec3f> points, const Color4f& color) {
        const int num_triangles = static_cast<int>(points.size() / 3);
        for (int i = 0; i < num_triangles; i++) {
            const int t = i * 3;
            add_triangle(points[t + 0], points[t + 1], points[t + 2], color);
        }
        add_elem();
    }

    void AuxDrawDevice::draw_mesh_solid(array_view<const Vec3f> points, const Color4f& color) {
        const int num_triangles = static_cast<int>(points.size() / 3);
        for (int i = 0; i < num_triangles; i++) {
            const int t = i * 3;
            add_triangle_solid(points[t + 0], points[t + 1], points[t + 2], color);
        }
        add_elem_solid();
    }

    void AuxDrawDevice::draw_mesh_faces(array_view<const Vec3f> pos, array_view<const Vec3u> faces, const Mat3x4f& mat, const Color4f& color) {
        for (const auto& face : faces) {
            Vec3f p0, p1, p2;

            p0 = Math3d::transform(mat, pos[face[0]]);
            p1 = Math3d::transform(mat, pos[face[1]]);
            p2 = Math3d::transform(mat, pos[face[2]]);

            add_triangle(p0, p1, p2, color);
        }
        add_elem_solid();
    }

    void AuxDrawDevice::draw_mesh_faces_solid(array_view<const Vec3f> pos, array_view<const Vec3u> faces, const Mat3x4f& mat, const Color4f& color) {
        for (const auto& face : faces) {
            Vec3f p0, p1, p2;

            p0 = Math3d::transform(mat, pos[face[0]]);
            p1 = Math3d::transform(mat, pos[face[1]]);
            p2 = Math3d::transform(mat, pos[face[2]]);

            add_triangle_solid(p0, p1, p2, color);
        }
        add_elem_solid();
    }

    void AuxDrawDevice::draw_sphere(const Vec3f& pos, float radius, const Color4f& color) {
        static const int steps_v = MAX_SPLIT_STEP_SPHERE;
        static const int steps_h = MAX_SPLIT_STEP_SPHERE;
        static const int total_v = steps_v + 1;
        static const int total_h = steps_h + 1;
        static const int nv      = total_v * total_h;
        static const int nt      = total_v * total_h;

        auto da_v = Math::PIf / float(steps_v);
        auto da_h = 2.0f * Math::PIf / float(steps_h);

        Vec3f points[nv];
        int   vertex_id = 0;

        for (int i = 0; i < total_v; ++i) {
            auto a_v = da_v * float(i) - Math::HALF_PIf;
            for (int j = 0; j < total_h; ++j) {
                auto a_h = da_h * float(j);

                auto r_xz = radius * Math::cos(a_v);
                auto x    = r_xz * Math::cos(a_h);
                auto z    = r_xz * Math::sin(a_h);
                auto y    = radius * Math::sin(a_v);

                points[vertex_id++] = pos + Vec3f(x, y, z);
            }
        }

        for (int i = 0; i < steps_v; ++i) {
            for (int j = 0; j < steps_h; ++j) {
                add_line(points[i * total_h + j + 1], points[i * total_h + j + 0], color);
                add_line(points[i * total_h + j + 0], points[i * total_h + j + total_h], color);
                add_line(points[i * total_h + j + total_h], points[i * total_h + j + total_h + 1], color);
                add_line(points[i * total_h + j + total_h + 1], points[i * total_h + j + 1], color);
            }
        }

        add_elem();
    }

    void AuxDrawDevice::draw_sphere_solid(const Vec3f& pos, float radius, const Color4f& color) {
        static const int steps_v = MAX_SPLIT_STEP_SPHERE;
        static const int steps_h = MAX_SPLIT_STEP_SPHERE;
        static const int total_v = steps_v + 1;
        static const int total_h = steps_h + 1;
        static const int nv      = total_v * total_h;
        static const int nt      = total_v * total_h;

        auto da_v = Math::PIf / float(steps_v);
        auto da_h = 2.0f * Math::PIf / float(steps_h);

        Vec3f points[nv];
        int   vertex_id = 0;

        for (int i = 0; i < total_v; ++i) {
            auto a_v = da_v * float(i) - Math::HALF_PIf;
            for (int j = 0; j < total_h; ++j) {
                auto a_h = da_h * float(j);

                auto r_xz = radius * Math::cos(a_v);
                auto x    = r_xz * Math::cos(a_h);
                auto z    = r_xz * Math::sin(a_h);
                auto y    = radius * Math::sin(a_v);

                points[vertex_id++] = pos + Vec3f(x, y, z);
            }
        }

        for (int i = 0; i < steps_v; ++i) {
            for (int j = 0; j < steps_h; ++j) {
                add_triangle_solid(points[i * total_h + j + 1], points[i * total_h + j + 0], points[i * total_h + j + total_h], color);
                add_triangle_solid(points[i * total_h + j + total_h], points[i * total_h + j + total_h + 1], points[i * total_h + j + 1], color);
            }
        }

        add_elem_solid();
    }

    void AuxDrawDevice::draw_cylinder(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot) {
        static const int nv            = MAX_SPLIT_STEP_CYLINDER * 2 + 2;
        static const int v_center_down = MAX_SPLIT_STEP_CYLINDER * 2 + 0;
        static const int v_center_top  = MAX_SPLIT_STEP_CYLINDER * 2 + 1;

        Vec3f points[nv];

        float dangle = Math::PIf / float(MAX_SPLIT_STEP_CYLINDER / 2.0f);
        float angle  = 0.0f;
        for (int i = 0; i < MAX_SPLIT_STEP_CYLINDER; ++i) {
            float rx = radius * Math::cos(angle);
            float rz = radius * Math::sin(angle);

            points[i]                           = pos + rot.rotate(Vec3f(rx, -height * 0.5f, rz));
            points[i + MAX_SPLIT_STEP_CYLINDER] = pos + rot.rotate(Vec3f(rx, height * 0.5f, rz));

            angle += dangle;
        }

        points[v_center_down] = pos + rot.rotate(Vec3f(0.0f, -height * 0.5f, 0.0f));
        points[v_center_top]  = pos + rot.rotate(Vec3f(0.0f, height * 0.5f, 0.0f));

        for (int i = 0; i < MAX_SPLIT_STEP_CYLINDER; ++i) {
            int v_dr = (i + 0) % MAX_SPLIT_STEP_CYLINDER;
            int v_dl = (i + 1) % MAX_SPLIT_STEP_CYLINDER;
            int v_tr = MAX_SPLIT_STEP_CYLINDER + (i + 0) % MAX_SPLIT_STEP_CYLINDER;
            int v_tl = MAX_SPLIT_STEP_CYLINDER + (i + 1) % MAX_SPLIT_STEP_CYLINDER;

            add_triangle(points[v_dr], points[v_dl], points[v_center_down], color);
            add_line(points[v_dl], points[v_dr], color);
            add_line(points[v_dr], points[v_tr], color);
            add_line(points[v_tr], points[v_tl], color);
            add_line(points[v_tl], points[v_dl], color);
            add_triangle(points[v_tl], points[v_tr], points[v_center_top], color);
        }

        add_elem();
    }

    void AuxDrawDevice::draw_cylinder_solid(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot) {
        static const int nv            = MAX_SPLIT_STEP_CYLINDER * 2 + 2;
        static const int v_center_down = MAX_SPLIT_STEP_CYLINDER * 2 + 0;
        static const int v_center_top  = MAX_SPLIT_STEP_CYLINDER * 2 + 1;

        Vec3f points[nv];

        float dangle = Math::PIf / float(MAX_SPLIT_STEP_CYLINDER / 2.0f);
        float angle  = 0.0f;
        for (int i = 0; i < MAX_SPLIT_STEP_CYLINDER; ++i) {
            float rx = radius * Math::cos(angle);
            float rz = radius * Math::sin(angle);

            points[i]                           = pos + rot.rotate(Vec3f(rx, -height * 0.5f, rz));
            points[i + MAX_SPLIT_STEP_CYLINDER] = pos + rot.rotate(Vec3f(rx, height * 0.5f, rz));

            angle += dangle;
        }

        points[v_center_down] = pos + rot.rotate(Vec3f(0.0f, -height * 0.5f, 0.0f));
        points[v_center_top]  = pos + rot.rotate(Vec3f(0.0f, height * 0.5f, 0.0f));

        for (int i = 0; i < MAX_SPLIT_STEP_CYLINDER; ++i) {
            int v_dr = (i + 0) % MAX_SPLIT_STEP_CYLINDER;
            int v_dl = (i + 1) % MAX_SPLIT_STEP_CYLINDER;
            int v_tr = MAX_SPLIT_STEP_CYLINDER + (i + 0) % MAX_SPLIT_STEP_CYLINDER;
            int v_tl = MAX_SPLIT_STEP_CYLINDER + (i + 1) % MAX_SPLIT_STEP_CYLINDER;

            add_triangle_solid(points[v_dr], points[v_dl], points[v_center_down], color);
            add_triangle_solid(points[v_dl], points[v_dr], points[v_tr], color);
            add_triangle_solid(points[v_tr], points[v_tl], points[v_dl], color);
            add_triangle_solid(points[v_tl], points[v_tr], points[v_center_top], color);
        }

        add_elem_solid();
    }

    void AuxDrawDevice::draw_cone(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot) {
        static const int nv       = MAX_SPLIT_STEP_CONE + 2;
        static const int v_center = MAX_SPLIT_STEP_CONE + 0;
        static const int v_top    = MAX_SPLIT_STEP_CONE + 1;

        Vec3f points[nv];

        float dangle = Math::PIf / float(MAX_SPLIT_STEP_CONE / 2.0f);
        float angle  = 0.0f;
        for (int i = 0; i < MAX_SPLIT_STEP_CONE; ++i) {
            float rx = radius * Math::cos(angle);
            float rz = radius * Math::sin(angle);

            points[i] = pos + rot.rotate(Vec3f(rx, -height * 0.5f, rz));

            angle += dangle;
        }

        points[v_center] = pos + rot.rotate(Vec3f(0.0f, -height * 0.5f, 0.0f));
        points[v_top]    = pos + rot.rotate(Vec3f(0.0f, height * 0.5f, 0.0f));

        for (int i = 0; i < MAX_SPLIT_STEP_CONE; ++i) {
            int v_dr = (i + 0) % MAX_SPLIT_STEP_CONE;
            int v_dl = (i + 1) % MAX_SPLIT_STEP_CONE;

            add_triangle(points[v_dr], points[v_dl], points[v_center], color);
            add_triangle(points[v_dl], points[v_dr], points[v_top], color);
        }

        add_elem();
    }

    void AuxDrawDevice::draw_cone_solid(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot) {
        static const int nv       = MAX_SPLIT_STEP_CONE + 2;
        static const int v_center = MAX_SPLIT_STEP_CONE + 0;
        static const int v_top    = MAX_SPLIT_STEP_CONE + 1;

        Vec3f points[nv];

        float dangle = Math::PIf / float(MAX_SPLIT_STEP_CONE / 2.0f);
        float angle  = 0.0f;
        for (int i = 0; i < MAX_SPLIT_STEP_CONE; ++i) {
            float rx = radius * Math::cos(angle);
            float rz = radius * Math::sin(angle);

            points[i] = pos + rot.rotate(Vec3f(rx, -height * 0.5f, rz));

            angle += dangle;
        }

        points[v_center] = pos + rot.rotate(Vec3f(0.0f, -height * 0.5f, 0.0f));
        points[v_top]    = pos + rot.rotate(Vec3f(0.0f, height * 0.5f, 0.0f));

        for (int i = 0; i < MAX_SPLIT_STEP_CONE; ++i) {
            int v_dr = (i + 0) % MAX_SPLIT_STEP_CONE;
            int v_dl = (i + 1) % MAX_SPLIT_STEP_CONE;

            add_triangle_solid(points[v_dr], points[v_dl], points[v_center], color);
            add_triangle_solid(points[v_dl], points[v_dr], points[v_top], color);
        }

        add_elem_solid();
    }

    void AuxDrawDevice::draw_box(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot) {
        const int nv = 8;

        Vec3f points[nv];

        float hx = size[0] * 0.5f;
        float hy = size[1] * 0.5f;
        float hz = size[2] * 0.5f;

        points[0] = pos + rot.rotate(Vec3f(-hx, hy, hz));
        points[1] = pos + rot.rotate(Vec3f(-hx, -hy, hz));
        points[2] = pos + rot.rotate(Vec3f(hx, -hy, hz));
        points[3] = pos + rot.rotate(Vec3f(hx, hy, hz));
        points[4] = pos + rot.rotate(Vec3f(-hx, hy, -hz));
        points[5] = pos + rot.rotate(Vec3f(-hx, -hy, -hz));
        points[6] = pos + rot.rotate(Vec3f(hx, -hy, -hz));
        points[7] = pos + rot.rotate(Vec3f(hx, hy, -hz));

        add_square(points[0], points[1], points[2], points[3], color);

        add_square(points[3], points[2], points[6], points[7], color);

        add_square(points[7], points[6], points[5], points[4], color);

        add_square(points[4], points[5], points[1], points[0], color);

        add_square(points[4], points[0], points[3], points[7], color);

        add_square(points[1], points[5], points[6], points[2], color);

        add_elem();
    }

    void AuxDrawDevice::draw_box_solid(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot) {
        const int nv = 8;

        Vec3f points[nv];

        float hx = size[0] * 0.5f;
        float hy = size[1] * 0.5f;
        float hz = size[2] * 0.5f;

        points[0] = pos + rot.rotate(Vec3f(-hx, hy, hz));
        points[1] = pos + rot.rotate(Vec3f(-hx, -hy, hz));
        points[2] = pos + rot.rotate(Vec3f(hx, -hy, hz));
        points[3] = pos + rot.rotate(Vec3f(hx, hy, hz));
        points[4] = pos + rot.rotate(Vec3f(-hx, hy, -hz));
        points[5] = pos + rot.rotate(Vec3f(-hx, -hy, -hz));
        points[6] = pos + rot.rotate(Vec3f(hx, -hy, -hz));
        points[7] = pos + rot.rotate(Vec3f(hx, hy, -hz));

        add_triangle_solid(points[0], points[1], points[2], color);
        add_triangle_solid(points[2], points[3], points[0], color);

        add_triangle_solid(points[3], points[2], points[7], color);
        add_triangle_solid(points[7], points[2], points[6], color);

        add_triangle_solid(points[7], points[6], points[5], color);
        add_triangle_solid(points[5], points[4], points[7], color);

        add_triangle_solid(points[4], points[5], points[0], color);
        add_triangle_solid(points[0], points[5], points[1], color);

        add_triangle_solid(points[4], points[0], points[7], color);
        add_triangle_solid(points[7], points[0], points[3], color);

        add_triangle_solid(points[1], points[5], points[2], color);
        add_triangle_solid(points[2], points[5], points[6], color);

        add_elem_solid();
    }

    void AuxDrawDevice::draw_text(const std::string& text, const Vec3f& pos, float size, const Color4f& color, bool project) {
        const int   n          = int(text.size());
        const auto  screen_pos = project ? Math3d::project_to_screen(m_mat_vp, m_screen_size, pos) : Vec2f(pos);
        const float scale      = size > 0 ? size / float(m_font->get_height()) : 1.0f;
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

            Vec3f glyph_p[4];
            Vec2f glyph_t[4];

            glyph_p[0] = Vec3f(screen_pos + Vec2f(left, top), 0.0f);
            glyph_t[0] = font_glyph.bitmap_uv0;

            glyph_p[1] = Vec3f(screen_pos + Vec2f(left, bottom), 0.0f);
            glyph_t[1] = Vec2f(font_glyph.bitmap_uv0.x(), font_glyph.bitmap_uv1.y());

            glyph_p[2] = Vec3f(screen_pos + Vec2f(right, bottom), 0.0f);
            glyph_t[2] = font_glyph.bitmap_uv1;

            glyph_p[3] = Vec3f(screen_pos + Vec2f(right, top), 0.0f);
            glyph_t[3] = Vec2f(font_glyph.bitmap_uv1.x(), font_glyph.bitmap_uv0.y());

            add_triangle_solid(glyph_p[0], glyph_p[1], glyph_p[2], glyph_t[0], glyph_t[1], glyph_t[2], color);
            add_triangle_solid(glyph_p[2], glyph_p[3], glyph_p[0], glyph_t[2], glyph_t[3], glyph_t[0], color);

            advance_x += scale * float(font_glyph.advance.x());
        }

        add_elem_font();
    }

    void AuxDrawDevice::draw_text_3d(const std::string& text, const Vec3f& pos, float size, const Color4f& color) {
        draw_text(text, pos, size, color, false);
    }

    void AuxDrawDevice::draw_text_2d(const std::string& text, const Vec2f& pos, float size, const Color4f& color) {
        draw_text(text, Vec3f(pos, 0.0f), size, color, true);
    }

    void AuxDrawDevice::set_font(Ref<Font> font) {
        m_font = std::move(font);
    }

    void AuxDrawDevice::set_mat_vp(const Mat4x4f& mat) {
        m_mat_vp = mat;
    }

    void AuxDrawDevice::set_screen_size(const Vec2f& size) {
        m_screen_size = size;
    }

    void AuxDrawDevice::render(RdgGraph& graph, RdgTexture* color, RdgTexture* depth, const Rect2i& viewport, float gamma, ShaderTable* shader_table, TextureManager* texture_manager) {
        WG_PROFILE_CPU_RENDER("AuxDrawDevice::render");
        WG_PROFILE_RDG_SCOPE("AuxDrawDevice::render", graph);

        const ShaderAuxDraw* aux_draw = shader_table->aux_draw();

        buffered_vector<Ref<Texture>> textures;
        textures.emplace_back(texture_manager->get_texture(DefaultTexture::White));
        textures.emplace_back(m_font->get_texture());

        auto upload_data = [&](AuxData& aux_data) -> RdgVertBuffer* {
            if (aux_data.vtx_offset == 0) {
                return nullptr;
            }
            aux_data.verts.reserve(graph.get_driver());
            RdgVertBuffer* buffer = GpuUtils::import_vert_buffer(graph, aux_data.verts);
            GpuUtils::update_buffer(graph, aux_data.verts, buffer);
            return buffer;
        };

        auto draw_elements = [&](AuxData& aux_data, const Mat4x4f& vp, RdgVertBuffer* buffer, const Strid& pass_name) {
            if (aux_data.vtx_offset == 0) {
                return;
            }

            assert(buffer);

            buffered_vector<Ref<ShaderParamBlock>> params_blocks;
            for (const Ref<Texture>& texture : textures) {
                auto param_block = graph.make_param_block(aux_draw->shader, 0, SIDDBG("params_" + pass_name.str()));
                param_block->set_var(aux_draw->pb_default.clipprojview, graph.get_driver()->clip_matrix() * vp);
                param_block->set_var(aux_draw->pb_default.inversegamma, 1.0f / (gamma > 0.0f ? gamma : 2.2f));
                param_block->set_var(aux_draw->pb_default.imagetexture, texture->get_texture());
                params_blocks.push_back(param_block);
            }

            graph.add_graphics_pass(SIDDBG("draw_batch_" + pass_name.str()), {RdgPassFlag::Manual})
                    .color_target(color)
                    .depth_target(depth)
                    .reading(buffer)
                    .bind([viewport, params_blocks, elems = aux_data.elems, buffer, aux_draw, pass_name](RdgPassContext& context) {
                        const GfxVertAttribs    attribs       = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f, GfxVertAttrib::Uv02f};
                        const ShaderPermutation permutation   = *aux_draw->shader->permutation(aux_draw->tq_default.name, pass_name, {aux_draw->tq_default.options.out_mode_linear}, attribs);
                        const GfxVertElements   vert_elements = GfxVertElements::make(attribs);

                        for (auto& param_block : params_blocks) {
                            context.validate_param_block(param_block);
                        }

                        context.begin_render_pass();
                        context.viewport(viewport);
                        context.bind_pso_graphics(aux_draw->shader, permutation, vert_elements);
                        context.bind_vert_buffer(buffer->get_buffer(), 0, 0);

                        std::optional<int> prev_texture;

                        for (const AuxDrawElem& elem : elems) {
                            if (!prev_texture || *prev_texture != elem.texture_idx) {
                                prev_texture = elem.texture_idx;
                                context.bind_param_block(params_blocks[elem.texture_idx]);
                            }
                            context.draw(elem.vtx_count, elem.vtx_offset, 1);
                        }

                        context.end_render_pass();

                        return WG_OK;
                    });
        };

        RdgVertBuffer* buffer_solid = upload_data(m_aux_data[Solid]);
        RdgVertBuffer* buffer_wire  = upload_data(m_aux_data[Wire]);
        RdgVertBuffer* buffer_text  = upload_data(m_aux_data[Text]);

        draw_elements(m_aux_data[Solid], m_mat_vp, buffer_solid, aux_draw->tq_default.ps_solid.pass_name);
        draw_elements(m_aux_data[Wire], m_mat_vp, buffer_wire, aux_draw->tq_default.ps_wire.pass_name);
        draw_elements(m_aux_data[Text], Math3d::orthographic(0, m_screen_size.x(), 0, m_screen_size.y(), -100.0f, 100.0f), buffer_text, aux_draw->tq_default.ps_text.pass_name);
    }

    void AuxDrawDevice::clear() {
        WG_PROFILE_CPU_RENDER("AuxDrawDevice::clear");

        auto clear_data = [](AuxData& data) {
            data.elems.clear();
            data.vtx_offset = 0;
        };

        for (int i = 0; i < Count; i++) {
            clear_data(m_aux_data[i]);
        }
    }

    void AuxDrawDevice::add_triangle_solid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec4f& col) {
        add_vert(p0, col);
        add_vert(p1, col);
        add_vert(p2, col);
    }

    void AuxDrawDevice::add_triangle_solid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec2f& uv0, const Vec2f& uv1, const Vec2f& uv2, const Vec4f& col) {
        add_vert(p0, uv0, col);
        add_vert(p1, uv1, col);
        add_vert(p2, uv2, col);
    }

    void AuxDrawDevice::add_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec4f& col) {
        add_vert(p0, col);
        add_vert(p1, col);
        add_vert(p1, col);
        add_vert(p2, col);
        add_vert(p2, col);
        add_vert(p0, col);
    }

    void AuxDrawDevice::add_line(const Vec3f& p0, const Vec3f& p1, const Vec4f& col) {
        add_vert(p0, col);
        add_vert(p1, col);
    }

    void AuxDrawDevice::add_square(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const Vec4f& col) {
        add_line(p0, p1, col);
        add_line(p1, p2, col);
        add_line(p2, p3, col);
        add_line(p3, p0, col);
    }

    void AuxDrawDevice::add_vert(const Vec3f& pos, const Vec4f& col) {
        AuxDrawVert vert;
        vert.pos = pos;
        vert.col = col;
        add_vert(vert);
    }

    void AuxDrawDevice::add_vert(const Vec3f& pos, const Vec2f& uv, const Vec4f& col) {
        AuxDrawVert vert;
        vert.pos = pos;
        vert.col = col;
        vert.uv  = uv;
        add_vert(vert);
    }

    void AuxDrawDevice::add_vert(const AuxDrawVert& vert) {
        m_verts.push_back(vert);
    }

    void AuxDrawDevice::add_elem() {
        add_elem(Wire, 0);
    }

    void AuxDrawDevice::add_elem_solid() {
        add_elem(Solid, 0);
    }

    void AuxDrawDevice::add_elem_font() {
        add_elem(Text, 1);
    }

    void AuxDrawDevice::add_elem(AuxDataType type, int texture_idx) {
        AuxData&  data      = m_aux_data[type];
        const int offset    = data.vtx_offset;
        const int num_verts = static_cast<int>(m_verts.size());

        if (data.verts.size() < (offset + num_verts)) {
            data.verts.resize(offset + num_verts);
        }

        for (int i = 0; i < num_verts; i++) {
            data.verts[i + offset] = m_verts[i];
        }

        m_verts.clear();
        data.vtx_offset += num_verts;

        AuxDrawElem& elem = data.elems.emplace_back();
        elem.vtx_count    = num_verts;
        elem.vtx_offset   = offset;
        elem.texture_idx  = texture_idx;
    }

    /** @brief Pipeline type to draw primitive */
    enum class AuxDrawPrimitiveType {
        Solid = 0,
        Wire  = 1
    };

    /** @brief Base class for an aux primitive to store and draw */
    struct AuxDrawPrimitive {
        virtual ~AuxDrawPrimitive()              = default;
        virtual void draw(AuxDrawDevice& device) = 0;

        [[nodiscard]] bool is_solid() const { return type == AuxDrawPrimitiveType::Solid; }
        [[nodiscard]] bool is_wire() const { return type == AuxDrawPrimitiveType::Wire; }

        Color4f              color;
        float                lifetime = 0.0f;
        AuxDrawPrimitiveType type;
    };

    /** @brief Stores info to draw line */
    struct AuxDrawLine final : public AuxDrawPrimitive {
        Vec3f from;
        Vec3f to;

        void draw(AuxDrawDevice& device) override {
            device.draw_line(from, to, color);
        }
    };

    /** @brief Stores info to draw trianlge */
    struct AuxDrawTriangle final : public AuxDrawPrimitive {
        Vec3f pos[3];

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_triangle_solid(pos[0], pos[1], pos[2], color);
            } else {
                device.draw_triangle(pos[0], pos[1], pos[2], color);
            }
        }
    };

    /** @brief Stores info to draw set of triangles */
    struct AuxDrawMesh final : public AuxDrawPrimitive {
        std::vector<Vec3f> points;

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_mesh_solid(points, color);
            } else {
                device.draw_mesh(points, color);
            }
        }
    };

    /** @brief Stores info to draw set of triangles */
    struct AuxDrawMeshFaces final : public AuxDrawPrimitive {
        std::vector<Vec3f> pos;
        std::vector<Vec3u> faces;
        Mat3x4f            mat;

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_mesh_faces_solid(pos, faces, mat, color);
            } else {
                device.draw_mesh_faces(pos, faces, mat, color);
            }
        }
    };

    /** @brief Stores info to draw a sphere */
    struct AuxDrawSphere final : public AuxDrawPrimitive {
        Vec3f pos;
        float radius;

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_sphere_solid(pos, radius, color);
            } else {
                device.draw_sphere(pos, radius, color);
            }
        }
    };

    /** @brief Stores info to draw a cylinder */
    struct AuxDrawCylinder final : public AuxDrawPrimitive {
        Vec3f pos;
        Quatf rot;
        float radius;
        float height;

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_cylinder_solid(pos, radius, height, color, rot);
            } else {
                device.draw_cylinder(pos, radius, height, color, rot);
            }
        }
    };

    /** @brief Stores info to draw a cone */
    struct AuxDrawCone final : public AuxDrawPrimitive {
        Vec3f pos;
        Quatf rot;
        float radius;
        float height;

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_cone_solid(pos, radius, height, color, rot);
            } else {
                device.draw_cone(pos, radius, height, color, rot);
            }
        }
    };

    /** @brief Stores info to draw an oriented box */
    struct AuxDrawBox final : public AuxDrawPrimitive {
        Vec3f pos;
        Vec3f size;
        Quatf rot;

        void draw(AuxDrawDevice& device) override {
            if (is_solid()) {
                device.draw_box_solid(pos, size, color, rot);
            } else {
                device.draw_box(pos, size, color, rot);
            }
        }
    };

    /** @brief Stores infor to draw 2d or 3d text */
    struct AuxDrawText final : public AuxDrawPrimitive {
        std::string text;
        Vec3f       pos;
        float       size;
        bool        project = false;

        void draw(AuxDrawDevice& device) override {
            device.draw_text(text, pos, size, color, project);
        }
    };

    AuxDrawManager::AuxDrawManager()  = default;
    AuxDrawManager::~AuxDrawManager() = default;

    void AuxDrawManager::draw_line(const Vec3f& from, const Vec3f& to, const Color4f& color, std::optional<float> lifetime) {
        auto line      = std::make_unique<AuxDrawLine>();
        line->from     = from;
        line->to       = to;
        line->color    = color;
        line->lifetime = lifetime.value_or(0.0f);

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(line));
    }

    void AuxDrawManager::draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color, bool solid, std::optional<float> lifetime) {
        auto triangle      = std::make_unique<AuxDrawTriangle>();
        triangle->pos[0]   = p0;
        triangle->pos[1]   = p1;
        triangle->pos[2]   = p2;
        triangle->color    = color;
        triangle->lifetime = lifetime.value_or(0.0f);
        triangle->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(triangle));
    }

    void AuxDrawManager::draw_mesh(array_view<const Vec3f> points, const Color4f& color, bool solid, std::optional<float> lifetime) {
        auto mesh      = std::make_unique<AuxDrawMesh>();
        mesh->color    = color;
        mesh->lifetime = lifetime.value_or(0.0f);
        mesh->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;
        mesh->points.resize(points.size());
        std::copy(points.begin(), points.end(), mesh->points.begin());

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(mesh));
    }

    void AuxDrawManager::draw_mesh_faces(array_view<const Vec3f> pos, array_view<const Vec3u> faces, const Mat3x4f& mat, const Color4f& color, bool solid, std::optional<float> lifetime) {
        auto mesh      = std::make_unique<AuxDrawMeshFaces>();
        mesh->color    = color;
        mesh->lifetime = lifetime.value_or(0.0f);
        mesh->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;
        mesh->mat      = mat;
        mesh->pos.resize(pos.size());
        mesh->faces.resize(faces.size());
        std::copy(pos.begin(), pos.end(), mesh->pos.begin());
        std::copy(faces.begin(), faces.end(), mesh->faces.begin());

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(mesh));
    }

    void AuxDrawManager::draw_sphere(const Vec3f& pos, float radius, const Color4f& color, bool solid, std::optional<float> lifetime) {
        auto sphere      = std::make_unique<AuxDrawSphere>();
        sphere->pos      = pos;
        sphere->radius   = radius;
        sphere->color    = color;
        sphere->lifetime = lifetime.value_or(0.0f);
        sphere->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(sphere));
    }

    void AuxDrawManager::draw_cylinder(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid, std::optional<float> lifetime) {
        auto cylinder      = std::make_unique<AuxDrawCylinder>();
        cylinder->pos      = pos;
        cylinder->radius   = radius;
        cylinder->height   = height;
        cylinder->color    = color;
        cylinder->rot      = rot;
        cylinder->lifetime = lifetime.value_or(0.0f);
        cylinder->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(cylinder));
    }

    void AuxDrawManager::draw_cone(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid, std::optional<float> lifetime) {
        auto cone      = std::make_unique<AuxDrawCone>();
        cone->pos      = pos;
        cone->radius   = radius;
        cone->height   = height;
        cone->color    = color;
        cone->rot      = rot;
        cone->lifetime = lifetime.value_or(0.0f);
        cone->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(cone));
    }

    void AuxDrawManager::draw_box(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot, bool solid, std::optional<float> lifetime) {
        auto box      = std::make_unique<AuxDrawBox>();
        box->pos      = pos;
        box->size     = size;
        box->color    = color;
        box->rot      = rot;
        box->lifetime = lifetime.value_or(0.0f);
        box->type     = solid ? AuxDrawPrimitiveType::Solid : AuxDrawPrimitiveType::Wire;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(box));
    }

    void AuxDrawManager::draw_text_3d(std::string text, const Vec3f& pos, float size, const Color4f& color, std::optional<float> lifetime) {
        if (text.empty()) {
            return;
        }

        auto primitive      = std::make_unique<AuxDrawText>();
        primitive->text     = std::move(text);
        primitive->pos      = pos;
        primitive->size     = size;
        primitive->color    = color;
        primitive->lifetime = lifetime.value_or(0.0f);
        primitive->project  = true;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(primitive));
    }

    void AuxDrawManager::draw_text_2d(std::string text, const Vec2f& pos, float size, const Color4f& color, std::optional<float> lifetime) {
        if (text.empty()) {
            return;
        }

        auto primitive      = std::make_unique<AuxDrawText>();
        primitive->text     = std::move(text);
        primitive->pos      = Vec3f(pos, 0);
        primitive->size     = size;
        primitive->color    = color;
        primitive->lifetime = lifetime.value_or(0.0f);
        primitive->project  = false;

        std::lock_guard guard(m_mutex);
        m_added.push_back(std::move(primitive));
    }

    void AuxDrawManager::render(RdgGraph& graph, RdgTexture* color, RdgTexture* depth, const Rect2i& viewport, float gamma, const Mat4x4f& proj_view, ShaderTable* shader_table, TextureManager* texture_manager) {
        WG_PROFILE_CPU_RENDER("AuxDrawManager::render");
        WG_PROFILE_RDG_SCOPE("AuxDrawManager::render", graph);

        assert(m_font);
        assert(color);
        assert(depth);
        assert(shader_table);
        assert(texture_manager);

        std::lock_guard guard(m_mutex);

        m_device.set_font(m_font);
        m_device.set_mat_vp(proj_view);
        m_device.set_screen_size(m_screen_size);

        for (const auto& primitiver_ptr : m_storage) {
            primitiver_ptr->draw(m_device);
        }

        m_device.render(graph, color, depth, viewport, gamma, shader_table, texture_manager);
        m_device.clear();
    }

    void AuxDrawManager::flush(float delta_time) {
        WG_PROFILE_CPU_RENDER("AuxDrawManager::flush");

        std::lock_guard guard(m_mutex);

        for (auto& element : m_storage) {
            element->lifetime -= delta_time;
        }

        while (!m_storage.empty() && m_storage.front()->lifetime <= 0.0f) {
            m_storage.pop_front();
        }

        for (auto& element : m_added) {
            m_storage.push_back(std::move(element));
        }

        std::sort(m_storage.begin(), m_storage.end(), [](const auto& left, const auto& right) {
            return left->lifetime < right->lifetime;
        });

        m_added.clear();
    }

    void AuxDrawManager::set_font(Ref<Font> font) {
        m_font = font;
    }

    void AuxDrawManager::set_screen_size(const Vec2f& size) {
        m_screen_size = size;
    }

}// namespace wmoge