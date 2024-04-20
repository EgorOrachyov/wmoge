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
#include "core/buffered_vector.hpp"
#include "math/aabb.hpp"
#include "math/color.hpp"
#include "math/frustum.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/plane.hpp"
#include "math/vec.hpp"
#include "render/render_defs.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class Camera
     * @brief Camera primitive to setup rendering
     */
    class Camera final {
    public:
        Camera();

        void set_fov(float fov);
        void set_aspect(float aspect);
        void set_near_far(float near, float far);
        void set_viewport(const Rect2i& viewport);
        void set_color(const Color4f& color);
        void set_proj(CameraProjection proj);
        void set_name(Strid name);
        void set_transform(const Mat4x4f& transform);
        void look(const Vec3f& dir, const Vec3f& up);
        void move(const Vec3f& delta);
        void move_to(const Vec3f& point);

        void validate();

        [[nodiscard]] bool  is_inside_or_intersects(const Aabbf& box) const;
        [[nodiscard]] float distance(const Aabbf& box) const;

        [[nodiscard]] const Frustumf&  get_frustum() const { return m_frustum; }
        [[nodiscard]] const Mat4x4f&   get_proj() const { return m_proj; }
        [[nodiscard]] const Mat4x4f&   get_view() const { return m_view; }
        [[nodiscard]] const Mat4x4f&   get_proj_view() const { return m_proj_view; }
        [[nodiscard]] const Rect2i&    get_viewport() const { return m_viewport; }
        [[nodiscard]] float            get_fov() const { return m_fov; }
        [[nodiscard]] float            get_aspect() const { return m_aspect; }
        [[nodiscard]] float            get_near() const { return m_near; }
        [[nodiscard]] float            get_far() const { return m_far; }
        [[nodiscard]] Vec3f            get_position() const { return m_position; }
        [[nodiscard]] Vec3f            get_direction() const { return m_direction; }
        [[nodiscard]] Vec3f            get_up() const { return m_up; }
        [[nodiscard]] CameraProjection get_projection() const { return m_projection; }

    private:
        void update_matrices();
        void update_frustum();

    private:
        Frustumf         m_frustum;
        Mat4x4f          m_proj;
        Mat4x4f          m_view;
        Mat4x4f          m_proj_view;
        Rect2i           m_viewport{0, 0, 1280, 720};
        float            m_fov    = Math::deg_to_rad(45.0f);
        float            m_aspect = 16.0f / 9.0f;
        float            m_near   = 0.1f;
        float            m_far    = 1000.0f;
        Vec3f            m_position;
        Vec3f            m_direction = Vec3f::axis_z();
        Vec3f            m_up        = Vec3f::axis_y();
        Strid            m_name;
        Color4f          m_color      = Color::BLACK4f;
        CameraProjection m_projection = CameraProjection::Perspective;
    };

    /**
     * @class CameraData
     * @brief Camera data for rendering
     */
    struct CameraData {
        Camera     camera;
        CameraType type;
        Mat4x4f    proj;
        Mat4x4f    view;
        Mat4x4f    proj_view;
        Mat4x4f    proj_prev;
        Mat4x4f    view_prev;
        Mat4x4f    proj_view_prev;
        Rect2i     viewport;
        Vec3f      movement;
        Vec3f      position;
        Vec3f      direction;
        Vec3f      up;
        Vec3f      position_prev;
        Vec3f      direction_prev;
        Vec3f      up_prev;
    };

    /**
     * @class CameraList
     * @brief Cameras list for rendering
     */
    class CameraList final {
    public:
        CameraList() = default;

        int               add_camera(CameraType type, const Camera& camera, std::optional<Camera> camera_prev);
        void              set_prev_params(int index, const Camera& camera_prev);
        const Camera&     camera_at(int index) const;
        const CameraData& data_at(int index) const;
        const CameraData& camera_main() const;
        void              clear();

        [[nodiscard]] array_view<const CameraData> get_cameras() const { return m_cameras; }
        [[nodiscard]] std::size_t                  get_size() const { return m_cameras.size(); }
        [[nodiscard]] bool                         is_empty() const { return m_cameras.empty(); }

    private:
        buffered_vector<CameraData, RenderLimits::MAX_CAMERAS> m_cameras;
    };

}// namespace wmoge