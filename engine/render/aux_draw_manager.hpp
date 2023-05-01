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

#ifndef WMOGE_AUX_DRAW_MANAGER_HPP
#define WMOGE_AUX_DRAW_MANAGER_HPP

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_texture.hpp"

#include "platform/window.hpp"
#include "resource/font.hpp"
#include "resource/shader.hpp"

#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class AuxDrawManager
     * @brief Utility-class for rendering debug geometry and text
     *
     * Aux draw manager provides a lwo-level possibility to draw basic primitives, shapes,
     * screen and world text for debug purposes onto final rendered image.
     */
    class AuxDrawManager final {
    public:
        AuxDrawManager();

        void draw_line(const Vec3f& from, const Vec3f& to, const Color3f& color);
        void draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color3f& color, bool solid);
        void draw_sphere(const Vec3f& pos, float radius, const Color3f& color, bool solid = true);
        void draw_cylinder(const Vec3f& pos, float radius, float height, const Color3f& color, const Quatf& rot, bool solid = true);
        void draw_cone(const Vec3f& pos, float radius, float height, const Color3f& color, const Quatf& rot, bool solid = true);
        void draw_box(const Vec3f& pos, const Vec3f& size, const Color3f& color, const Quatf& rot, bool solid = true);
        void draw_text_3d(const std::string& text, const Vec3f& pos, float size, const Color3f& color);
        void draw_text_2d(const std::string& text, const Vec2f& pos, float size, const Color3f& color);

        void set_window(const ref_ptr<Window>& window);
        void set_font(const ref_ptr<Font>& font);
        void set_projection(const Mat4x4f& projection);
        void set_view(const Mat4x4f& view);
        void set_viewport(const Rect2i& viewport);
        void set_screen_size(const Vec2f& size);

        void render();

    private:
        static const int MAX_SPLIT_STEP_SPHERE   = 6;
        static const int MAX_SPLIT_STEP_CONE     = 8;
        static const int MAX_SPLIT_STEP_CYLINDER = 8;

        struct AuxDrawLine {
            Vec3f   from;
            Vec3f   to;
            Color3f color;
        };
        struct AuxDrawTriangle {
            Vec3f   p[3];
            Color3f color;
        };
        struct AuxDrawGlyph {
            Vec2f   p[4];
            Vec2f   t[4];
            Color3f color;
        };

    private:
        ref_ptr<GfxVertBuffer>       m_gfx_triangles_solid;
        ref_ptr<GfxVertBuffer>       m_gfx_triangles_wire;
        ref_ptr<GfxVertBuffer>       m_gfx_lines;
        ref_ptr<GfxVertBuffer>       m_gfx_glyphs;
        std::vector<AuxDrawLine>     m_lines;
        std::vector<AuxDrawTriangle> m_triangles_solid;
        std::vector<AuxDrawTriangle> m_triangles_wire;
        std::vector<AuxDrawGlyph>    m_glyphs;

        ref_ptr<Shader>           m_shader_geom;
        ref_ptr<Shader>           m_shader_text;
        ref_ptr<GfxRenderPass>    m_render_pass;
        ref_ptr<GfxPipeline>      m_pipeline_glyphs;
        ref_ptr<GfxPipeline>      m_pipeline_line;
        ref_ptr<GfxPipeline>      m_pipeline_solid;
        ref_ptr<GfxPipeline>      m_pipeline_wireframe;
        ref_ptr<GfxUniformBuffer> m_constants;

        ref_ptr<Window> m_window;
        ref_ptr<Font>   m_debug_font;
        Mat4x4f         m_proj;
        Mat4x4f         m_view;
        Rect2i          m_viewport;
        Vec2f           m_screen_size;

        int m_gfx_capacity_triangles_solid = 0;
        int m_gfx_capacity_triangles_wire  = 0;
        int m_gfx_capacity_lines           = 0;
        int m_gfx_capacity_text            = 0;
    };

}// namespace wmoge

#endif//WMOGE_AUX_DRAW_MANAGER_HPP
