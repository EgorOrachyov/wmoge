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
#include "math/math_utils3d.hpp"
#include "profiler/profiler.hpp"

#include <algorithm>
#include <utility>

namespace wmoge {

#define WG_PUSH_TRIANGLE_P3C4(p0, p1, p2, color) \
    vertices->pos = p0;                          \
    vertices->col = color;                       \
    vertices++;                                  \
    vertices->pos = p1;                          \
    vertices->col = color;                       \
    vertices++;                                  \
    vertices->pos = p2;                          \
    vertices->col = color;                       \
    vertices++;

#define WG_PUSH_TRIANGLE_P3C4UV2(p0, p1, p2, t0, t1, t2, color) \
    vertices->pos = Vec3f(p0, 0.0f);                            \
    vertices->uv  = t0;                                         \
    vertices->col = color;                                      \
    vertices++;                                                 \
    vertices->pos = Vec3f(p1, 0.0f);                            \
    vertices->uv  = t1;                                         \
    vertices->col = color;                                      \
    vertices++;                                                 \
    vertices->pos = Vec3f(p2, 0.0f);                            \
    vertices->uv  = t2;                                         \
    vertices->col = color;                                      \
    vertices++;

    static const int MAX_SPLIT_STEP_SPHERE   = 6;
    static const int MAX_SPLIT_STEP_CONE     = 8;
    static const int MAX_SPLIT_STEP_CYLINDER = 8;

    /** @brief Pipeline type to draw primitive */
    enum class AuxDrawPrimitiveType {
        Line           = 0,
        TrianglesSolid = 1,
        TrianglesWired = 2,
        Text           = 3
    };

    /** @brief Base class for an aux primitive to store and draw */
    struct AuxDrawPrimitive {
        virtual ~AuxDrawPrimitive() = default;

        struct FillParams {
            Mat4x4f mat_view_proj;
            Font*   font;
            Vec2f   screen_size;
        };

        [[nodiscard]] virtual AuxDrawPrimitiveType get_type() const                            = 0;
        [[nodiscard]] virtual int                  get_num_elements() const                    = 0;
        [[nodiscard]] virtual void*                fill(void* buffer, const FillParams&) const = 0;

        float lifetime = 0.0f;
    };

    /** @brief Stores info to draw set of lines */
    struct AuxDrawLines final : public AuxDrawPrimitive {
        struct Line {
            Vec3f   from;
            Vec3f   to;
            Color4f color;
        };

        buffered_vector<Line, 1> lines;

        ~AuxDrawLines() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return AuxDrawPrimitiveType::Line;
        }
        [[nodiscard]] int get_num_elements() const override {
            return int(lines.size());
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams&) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawLines::fill");

            auto* vertices = (GfxVF_Pos3Col4*) buffer;

            for (const auto& line : lines) {
                vertices->pos = line.from;
                vertices->col = line.color;
                vertices++;
                vertices->pos = line.to;
                vertices->col = line.color;
                vertices++;
            }

            return vertices;
        }
    };

    /** @brief Stores info to draw set of triangles */
    struct AuxDrawTriangles final : public AuxDrawPrimitive {
        struct Triangle {
            Vec3f   pos[3];
            Color4f color;
        };

        buffered_vector<Triangle, 1> triangles;
        AuxDrawPrimitiveType         type = AuxDrawPrimitiveType::TrianglesSolid;

        ~AuxDrawTriangles() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return type;
        }
        [[nodiscard]] int get_num_elements() const override {
            return int(triangles.size());
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams&) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawTriangles::fill");

            auto* vertices = (GfxVF_Pos3Col4*) buffer;

            for (const auto& triangle : triangles) {
                WG_PUSH_TRIANGLE_P3C4(triangle.pos[0], triangle.pos[1], triangle.pos[2], triangle.color);
            }

            return vertices;
        }
    };

    /** @brief Stores info to draw a sphere */
    struct AuxDrawSphere final : public AuxDrawPrimitive {
        Vec3f                pos;
        Color4f              color;
        float                radius;
        AuxDrawPrimitiveType type = AuxDrawPrimitiveType::TrianglesSolid;

        ~AuxDrawSphere() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return type;
        }
        [[nodiscard]] int get_num_elements() const override {
            return MAX_SPLIT_STEP_SPHERE * MAX_SPLIT_STEP_SPHERE;
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams&) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawSphere::fill");

            auto* vertices = (GfxVF_Pos3Col4*) buffer;

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
                    WG_PUSH_TRIANGLE_P3C4(points[i * total_h + j + 1], points[i * total_h + j + 0], points[i * total_h + j + total_h], color);
                    WG_PUSH_TRIANGLE_P3C4(points[i * total_h + j + total_h], points[i * total_h + j + total_h + 1], points[i * total_h + j + 1], color);
                }
            }

            return vertices;
        }
    };

    /** @brief Stores info to draw a cylinder */
    struct AuxDrawCylinder final : public AuxDrawPrimitive {
        Vec3f                pos;
        Color4f              color;
        Quatf                rot;
        float                radius;
        float                height;
        AuxDrawPrimitiveType type = AuxDrawPrimitiveType::TrianglesSolid;

        ~AuxDrawCylinder() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return type;
        }
        [[nodiscard]] int get_num_elements() const override {
            return MAX_SPLIT_STEP_CYLINDER * 4;
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams&) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawCylinder::fill");

            auto* vertices = (GfxVF_Pos3Col4*) buffer;

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

                WG_PUSH_TRIANGLE_P3C4(points[v_dr], points[v_dl], points[v_center_down], color);
                WG_PUSH_TRIANGLE_P3C4(points[v_dl], points[v_dr], points[v_tr], color);
                WG_PUSH_TRIANGLE_P3C4(points[v_tr], points[v_tl], points[v_dl], color);
                WG_PUSH_TRIANGLE_P3C4(points[v_tl], points[v_tr], points[v_center_top], color);
            }

            return vertices;
        }
    };

    /** @brief Stores info to draw a cone */
    struct AuxDrawCone final : public AuxDrawPrimitive {
        Vec3f                pos;
        Color4f              color;
        Quatf                rot;
        float                radius;
        float                height;
        AuxDrawPrimitiveType type = AuxDrawPrimitiveType::TrianglesSolid;

        ~AuxDrawCone() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return type;
        }
        [[nodiscard]] int get_num_elements() const override {
            return MAX_SPLIT_STEP_CONE * 2;
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams&) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawCone::fill");

            auto* vertices = (GfxVF_Pos3Col4*) buffer;

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

                WG_PUSH_TRIANGLE_P3C4(points[v_dr], points[v_dl], points[v_center], color);
                WG_PUSH_TRIANGLE_P3C4(points[v_dl], points[v_dr], points[v_top], color);
            }

            return vertices;
        }
    };

    /** @brief Stores info to draw an oriented box */
    struct AuxDrawBox final : public AuxDrawPrimitive {
        Vec3f                pos;
        Vec3f                size;
        Color4f              color;
        Quatf                rot;
        AuxDrawPrimitiveType type = AuxDrawPrimitiveType::TrianglesSolid;

        ~AuxDrawBox() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return type;
        }
        [[nodiscard]] int get_num_elements() const override {
            return 2 * 6;
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams&) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawBox::fill");

            auto* vertices = (GfxVF_Pos3Col4*) buffer;

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

            WG_PUSH_TRIANGLE_P3C4(points[0], points[1], points[2], color);
            WG_PUSH_TRIANGLE_P3C4(points[2], points[3], points[0], color);

            WG_PUSH_TRIANGLE_P3C4(points[3], points[2], points[7], color);
            WG_PUSH_TRIANGLE_P3C4(points[7], points[2], points[6], color);

            WG_PUSH_TRIANGLE_P3C4(points[7], points[6], points[5], color);
            WG_PUSH_TRIANGLE_P3C4(points[5], points[4], points[7], color);

            WG_PUSH_TRIANGLE_P3C4(points[4], points[5], points[0], color);
            WG_PUSH_TRIANGLE_P3C4(points[0], points[5], points[1], color);

            WG_PUSH_TRIANGLE_P3C4(points[4], points[0], points[7], color);
            WG_PUSH_TRIANGLE_P3C4(points[7], points[0], points[3], color);

            WG_PUSH_TRIANGLE_P3C4(points[1], points[5], points[2], color);
            WG_PUSH_TRIANGLE_P3C4(points[2], points[5], points[6], color);

            return vertices;
        }
    };

    /** @brief Stores infor to draw 2d or 3d text */
    struct AuxDrawText final : public AuxDrawPrimitive {
        std::string text;
        Vec3f       pos;
        Color4f     color;
        float       size;
        bool        project = false;

        ~AuxDrawText() override = default;

        [[nodiscard]] AuxDrawPrimitiveType get_type() const override {
            return AuxDrawPrimitiveType::Text;
        }
        [[nodiscard]] int get_num_elements() const override {
            return int(text.length());
        }
        [[nodiscard]] void* fill(void* buffer, const FillParams& params) const override {
            WG_AUTO_PROFILE_RENDER("AuxDrawText::fill");

            auto* vertices = (GfxVF_Pos3Col4Uv2*) buffer;

            const int   n          = int(text.size());
            const auto  screen_pos = project ? Math3d::project_to_screen(params.mat_view_proj, params.screen_size, pos) : Vec2f(pos);
            const float scale      = size > 0 ? size / float(params.font->get_height()) : 1.0f;
            const auto& glyphs     = params.font->get_glyphs();
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

                Vec2f glyph_p[4];
                Vec2f glyph_t[4];

                glyph_p[0] = screen_pos + Vec2f(left, top);
                glyph_t[0] = font_glyph.bitmap_uv0;
                glyph_p[1] = screen_pos + Vec2f(left, bottom);
                glyph_t[1] = Vec2f(font_glyph.bitmap_uv0.x(), font_glyph.bitmap_uv1.y());
                glyph_p[2] = screen_pos + Vec2f(right, bottom);
                glyph_t[2] = font_glyph.bitmap_uv1;
                glyph_p[3] = screen_pos + Vec2f(right, top);
                glyph_t[3] = Vec2f(font_glyph.bitmap_uv1.x(), font_glyph.bitmap_uv0.y());

                WG_PUSH_TRIANGLE_P3C4UV2(glyph_p[0], glyph_p[1], glyph_p[2], glyph_t[0], glyph_t[1], glyph_t[2], color);
                WG_PUSH_TRIANGLE_P3C4UV2(glyph_p[2], glyph_p[3], glyph_p[0], glyph_t[2], glyph_t[3], glyph_t[0], color);

                advance_x += scale * float(font_glyph.advance.x());
            }

            return vertices;
        }
    };

    AuxDrawManager::AuxDrawManager() {
        std::string font_name   = "assets/fonts/consolas";
        m_screen_size.values[0] = 1280.0f;
        m_screen_size.values[1] = 720.0f;

        // m_font = asset_manager->load(SID(font_name)).cast<Font>();

        m_lines.set_name(SID("aux_lines"));
        m_tria_solid.set_name(SID("aux_tria_solid"));
        m_tria_wired.set_name(SID("aux_tria_wired"));
        m_text.set_name(SID("aux_text"));
    }

    void AuxDrawManager::draw_line(const Vec3f& from, const Vec3f& to, const Color4f& color, float lifetime) {
        std::lock_guard guard(m_mutex);

        auto lines = std::make_unique<AuxDrawLines>();
        lines->lines.push_back({from, to, color});
        lines->lifetime = lifetime;

        m_added.push_back(std::move(lines));
    }
    void AuxDrawManager::draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color, bool solid, float lifetime) {
        std::lock_guard guard(m_mutex);

        auto triangles = std::make_unique<AuxDrawTriangles>();
        triangles->triangles.push_back(AuxDrawTriangles::Triangle{{p0, p1, p2}, color});
        triangles->type = solid ? AuxDrawPrimitiveType::TrianglesSolid : AuxDrawPrimitiveType::TrianglesWired;

        m_added.push_back(std::move(triangles));
    }
    void AuxDrawManager::draw_sphere(const Vec3f& pos, float radius, const Color4f& color, bool solid, float lifetime) {
        std::lock_guard guard(m_mutex);

        auto sphere      = std::make_unique<AuxDrawSphere>();
        sphere->pos      = pos;
        sphere->radius   = radius;
        sphere->color    = color;
        sphere->lifetime = lifetime;
        sphere->type     = solid ? AuxDrawPrimitiveType::TrianglesSolid : AuxDrawPrimitiveType::TrianglesWired;

        m_added.push_back(std::move(sphere));
    }
    void AuxDrawManager::draw_cylinder(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid, float lifetime) {
        std::lock_guard guard(m_mutex);

        auto cylinder      = std::make_unique<AuxDrawCylinder>();
        cylinder->pos      = pos;
        cylinder->radius   = radius;
        cylinder->height   = height;
        cylinder->color    = color;
        cylinder->rot      = rot;
        cylinder->lifetime = lifetime;
        cylinder->type     = solid ? AuxDrawPrimitiveType::TrianglesSolid : AuxDrawPrimitiveType::TrianglesWired;

        m_added.push_back(std::move(cylinder));
    }
    void AuxDrawManager::draw_cone(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid, float lifetime) {
        std::lock_guard guard(m_mutex);

        auto cone      = std::make_unique<AuxDrawCone>();
        cone->pos      = pos;
        cone->radius   = radius;
        cone->height   = height;
        cone->color    = color;
        cone->rot      = rot;
        cone->lifetime = lifetime;
        cone->type     = solid ? AuxDrawPrimitiveType::TrianglesSolid : AuxDrawPrimitiveType::TrianglesWired;

        m_added.push_back(std::move(cone));
    }
    void AuxDrawManager::draw_box(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot, bool solid, float lifetime) {
        std::lock_guard guard(m_mutex);

        auto box      = std::make_unique<AuxDrawBox>();
        box->pos      = pos;
        box->size     = size;
        box->color    = color;
        box->rot      = rot;
        box->lifetime = lifetime;
        box->type     = solid ? AuxDrawPrimitiveType::TrianglesSolid : AuxDrawPrimitiveType::TrianglesWired;

        m_added.push_back(std::move(box));
    }
    void AuxDrawManager::draw_text_3d(std::string text, const Vec3f& pos, float size, const Color4f& color, float lifetime) {
        if (text.empty()) {
            WG_LOG_WARNING("passed empty string to draw");
            return;
        }

        std::lock_guard guard(m_mutex);

        auto primitive      = std::make_unique<AuxDrawText>();
        primitive->text     = std::move(text);
        primitive->pos      = pos;
        primitive->size     = size;
        primitive->color    = color;
        primitive->lifetime = lifetime;
        primitive->project  = true;

        m_added.push_back(std::move(primitive));
    }
    void AuxDrawManager::draw_text_2d(std::string text, const Vec2f& pos, float size, const Color4f& color, float lifetime) {
        if (text.empty()) {
            WG_LOG_WARNING("passed empty string to draw");
            return;
        }

        std::lock_guard guard(m_mutex);

        auto primitive      = std::make_unique<AuxDrawText>();
        primitive->text     = std::move(text);
        primitive->pos      = Vec3f(pos, 0);
        primitive->size     = size;
        primitive->color    = color;
        primitive->lifetime = lifetime;
        primitive->project  = false;

        m_added.push_back(std::move(primitive));
    }

    void AuxDrawManager::render(const Ref<Window>& window, const Rect2i& viewport, const Mat4x4f& mat_proj_view) {
        WG_AUTO_PROFILE_RENDER("AuxDrawManager::render");

        if (is_empty()) {
            return;
        }

        // auto engine     = Engine::instance();
        // auto gfx_driver = engine->gfx_driver();
        // auto gfx_ctx    = engine->gfx_ctx();

        // HgfxPassBase pass_lines;
        // pass_lines.name          = SID("aux_draw_lines");
        // pass_lines.out_srgb      = true;
        // pass_lines.mat_proj_view = mat_proj_view;
        // pass_lines.prim_type     = GfxPrimType::Lines;
        // pass_lines.attribs_full  = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f};
        // pass_lines.attribs_req   = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f};
        // pass_lines.compile(gfx_ctx);

        // HgfxPassBase pass_triangles_solid;
        // pass_triangles_solid.name          = SID("aux_draw_triangles_solid");
        // pass_triangles_solid.out_srgb      = true;
        // pass_triangles_solid.mat_proj_view = mat_proj_view;
        // pass_triangles_solid.prim_type     = GfxPrimType::Triangles;
        // pass_triangles_solid.poly_mode     = GfxPolyMode::Fill;
        // pass_triangles_solid.attribs_full  = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f};
        // pass_triangles_solid.attribs_req   = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f};
        // pass_triangles_solid.compile(gfx_ctx);

        // HgfxPassBase pass_triangles_wire;
        // pass_triangles_wire.name          = SID("aux_draw_triangles_wire");
        // pass_triangles_wire.out_srgb      = true;
        // pass_triangles_wire.mat_proj_view = mat_proj_view;
        // pass_triangles_wire.prim_type     = GfxPrimType::Triangles;
        // pass_triangles_wire.poly_mode     = GfxPolyMode::Line;
        // pass_triangles_wire.attribs_full  = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f};
        // pass_triangles_wire.attribs_req   = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f};
        // pass_triangles_wire.compile(gfx_ctx);

        // HgfxPassText pass_text;
        // pass_text.name         = SID("aux_draw_text");
        // pass_text.out_srgb     = true;
        // pass_text.screen_size  = m_screen_size;
        // pass_text.font_texture = m_font->get_bitmap();
        // pass_text.font_sampler = m_font->get_sampler();
        // pass_text.compile(gfx_ctx);

        const int num_types                           = 4;
        int       num_elements[num_types]             = {0, 0, 0, 0};
        int       num_vertices[num_types]             = {0, 0, 0, 0};
        int       num_vertices_pre_element[num_types] = {2, 3, 3, 6};

        // count total number of elements for each type of primitive
        for (const auto& primitive : m_storage) {
            const int primitive_type_index      = int(primitive->get_type());
            const int num_elements_in_primitive = primitive->get_num_elements();
            num_elements[primitive_type_index] += num_elements_in_primitive;
            assert(num_elements_in_primitive > 0);
        }

        // count vertices
        for (int i = 0; i < num_types; i++) {
            num_vertices[i] = num_elements[i] * num_vertices_pre_element[i];
        }

        // reserve space in vert buffers
        m_lines.resize(num_vertices[int(AuxDrawPrimitiveType::Line)]);
        m_tria_solid.resize(num_vertices[int(AuxDrawPrimitiveType::TrianglesSolid)]);
        m_tria_wired.resize(num_vertices[int(AuxDrawPrimitiveType::TrianglesWired)]);
        m_text.resize(num_vertices[int(AuxDrawPrimitiveType::Text)]);

        void* allocation_per_type[num_types] = {
                m_lines.get_mem(),
                m_tria_solid.get_mem(),
                m_tria_wired.get_mem(),
                m_text.get_mem()};

        const Ref<GfxVertBuffer>* vert_buffers[num_types] = {
                &m_lines.get_buffer(),
                &m_tria_solid.get_buffer(),
                &m_tria_wired.get_buffer(),
                &m_text.get_buffer()};

        AuxDrawPrimitive::FillParams params;
        params.mat_view_proj = mat_proj_view;
        params.screen_size   = m_screen_size;
        params.font          = m_font.get();

        // fill vertex data for all elements
        for (const auto& primitive : m_storage) {
            const int primitive_type_index = int(primitive->get_type());
            auto&     allocation           = allocation_per_type[primitive_type_index];
            allocation                     = primitive->fill(allocation, params);
        }

        // flush data
        // m_lines.flush(gfx_ctx);
        // m_tria_solid.flush(gfx_ctx);
        // m_tria_wired.flush(gfx_ctx);
        // m_text.flush(gfx_ctx);

        // HgfxPass* passes[num_types] = {&pass_lines, &pass_triangles_solid, &pass_triangles_wire, &pass_text};

        //gfx_ctx->execute([&]() {
        //    gfx_ctx->begin_render_pass({}, SID("AuxDrawManager::render"));
        //    gfx_ctx->bind_target(window);
        //    gfx_ctx->viewport(viewport);

        //    for (int i = 0; i < num_types; i++) {
        //        // if it has primitives and configured pass - do draw
        //        if (num_elements[i] > 0 && passes[i] && passes[i]->configure(thread_ctx)) {
        //            thread_ctx->bind_vert_buffer(*vert_buffers[i], 0, 0);
        //            thread_ctx->draw(num_vertices[i], 0, 1);
        //        }
        //    }

        //    gfx_ctx->end_render_pass();
        //});
    }
    void AuxDrawManager::flush(float delta_time) {
        WG_AUTO_PROFILE_RENDER("AuxDrawManager::flush");

        std::lock_guard guard(m_mutex);

        for (auto& element : m_storage) {
            if (element->lifetime != LIFETIME_INFINITY && element->lifetime != LIFETIME_ONE_FRAME) {
                element->lifetime -= delta_time;
            }
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

    bool AuxDrawManager::is_empty() const {
        std::lock_guard guard(m_mutex);

        return m_storage.empty();
    }
    int AuxDrawManager::get_size() const {
        std::lock_guard guard(m_mutex);

        return int(m_storage.size());
    }

}// namespace wmoge