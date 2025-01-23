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

#pragma once

#include "core/array_view.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "gpu/gpu_buffer.hpp"
#include "grc/font.hpp"
#include "grc/texture_manager.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"
#include "rdg/rdg_graph.hpp"
#include "render/interop.hpp"

#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class AuxDrawDevice
     * @brief Device to draw simple primitives to flush later from buffer
     * 
     * Immediately captuters primitves vertex data and fills buffers, which 
     * can be later uploaded to gpu and rendered in batched fashion at once.
     */
    class AuxDrawDevice final {
    public:
        AuxDrawDevice() = default;

        void draw_line(const Vec3f& from, const Vec3f& to, const Color4f& color);
        void draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color);
        void draw_triangle_solid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color);
        void draw_mesh(array_view<const Vec3f> points, const Color4f& color);
        void draw_mesh_solid(array_view<const Vec3f> points, const Color4f& color);
        void draw_mesh_faces(array_view<const Vec3f> pos, array_view<const Vec3u> faces, const Mat3x4f& mat, const Color4f& color);
        void draw_mesh_faces_solid(array_view<const Vec3f> pos, array_view<const Vec3u> faces, const Mat3x4f& mat, const Color4f& color);
        void draw_sphere(const Vec3f& pos, float radius, const Color4f& color);
        void draw_sphere_solid(const Vec3f& pos, float radius, const Color4f& color);
        void draw_cylinder(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot);
        void draw_cylinder_solid(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot);
        void draw_cone(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot);
        void draw_cone_solid(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot);
        void draw_box(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot);
        void draw_box_solid(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot);
        void draw_text(const std::string& text, const Vec3f& pos, float size, const Color4f& color, bool project);
        void draw_text_3d(const std::string& text, const Vec3f& pos, float size, const Color4f& color);
        void draw_text_2d(const std::string& text, const Vec2f& pos, float size, const Color4f& color);

        void set_font(Ref<Font> font);
        void set_mat_vp(const Mat4x4f& mat);
        void set_screen_size(const Vec2f& size);

        void render(RdgGraph& graph, RdgTexture* color, RdgTexture* depth, const Rect2i& viewport, float gamma, class ShaderTable* shader_table, class TextureManager* texture_manager);
        void clear();

    private:
        struct AuxDrawVert {
            Vec3f pos;
            Vec4f col;
            Vec2f uv;
        };

        struct AuxDrawElem {
            int texture_idx = 0;
            int vtx_offset  = 0;
            int vtx_count   = 0;
        };

        struct AuxData {
            std::vector<AuxDrawElem>   elems;
            GpuVertBuffer<AuxDrawVert> verts;
            int                        vtx_offset = 0;
        };

        enum AuxDataType {
            Solid = 0,
            Wire  = 1,
            Text  = 2,
            Count = 3
        };

        void add_triangle_solid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec4f& col);
        void add_triangle_solid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec2f& uv0, const Vec2f& uv1, const Vec2f& uv2, const Vec4f& col);
        void add_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec4f& col);
        void add_line(const Vec3f& p0, const Vec3f& p1, const Vec4f& col);
        void add_square(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const Vec4f& col);
        void add_vert(const Vec3f& pos, const Vec4f& col);
        void add_vert(const Vec3f& pos, const Vec2f& uv, const Vec4f& col);
        void add_vert(const AuxDrawVert& vert);
        void add_elem(AuxDataType type, int texture_idx);
        void add_elem();
        void add_elem_solid();
        void add_elem_font();

        static constexpr int MAX_SPLIT_STEP_SPHERE   = 6;
        static constexpr int MAX_SPLIT_STEP_CONE     = 8;
        static constexpr int MAX_SPLIT_STEP_CYLINDER = 8;

    private:
        AuxData                  m_aux_data[Count];
        std::vector<AuxDrawVert> m_verts;
        Ref<Font>                m_font;
        Mat4x4f                  m_mat_vp;
        Vec2f                    m_screen_size;
    };

    /**
     * @class AuxDrawManager
     * @brief Utility-class for rendering debug geometry and text
     *
     * Aux draw manager provides a low-level possibility to draw basic primitives, shapes,
     * screen and world text for debug purposes onto final rendered image. Also supports
     * persistent primitives with desired life-time.
     *
     * @note thread-safe
     */
    class AuxDrawManager final {
    public:
        AuxDrawManager();
        ~AuxDrawManager();

        void draw_line(const Vec3f& from, const Vec3f& to, const Color4f& color, std::optional<float> lifetime);
        void draw_triangle(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Color4f& color, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_mesh(array_view<const Vec3f> points, const Color4f& color, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_mesh_faces(array_view<const Vec3f> pos, array_view<const Vec3u> faces, const Mat3x4f& mat, const Color4f& color, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_sphere(const Vec3f& pos, float radius, const Color4f& color, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_cylinder(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_cone(const Vec3f& pos, float radius, float height, const Color4f& color, const Quatf& rot, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_box(const Vec3f& pos, const Vec3f& size, const Color4f& color, const Quatf& rot, bool solid = true, std::optional<float> lifetime = std::nullopt);
        void draw_text_3d(std::string text, const Vec3f& pos, float size, const Color4f& color, std::optional<float> lifetime = std::nullopt);
        void draw_text_2d(std::string text, const Vec2f& pos, float size, const Color4f& color, std::optional<float> lifetime = std::nullopt);

        void render(RdgGraph& graph, RdgTexture* color, RdgTexture* depth, const Rect2i& viewport, float gamma, const Mat4x4f& proj_view, class ShaderTable* shader_table, class TextureManager* texture_manager);
        void flush(float delta_time);

        void set_font(Ref<Font> font);
        void set_screen_size(const Vec2f& size);

    private:
        AuxDrawDevice                                         m_device;
        std::vector<std::unique_ptr<struct AuxDrawPrimitive>> m_added;
        std::deque<std::unique_ptr<struct AuxDrawPrimitive>>  m_storage;
        Ref<Font>                                             m_font;
        Vec2f                                                 m_screen_size;
        SpinMutex                                             m_mutex;
    };

}// namespace wmoge