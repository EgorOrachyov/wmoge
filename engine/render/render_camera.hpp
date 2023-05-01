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

#ifndef WMOGE_RENDER_CAMERA_HPP
#define WMOGE_RENDER_CAMERA_HPP

#include "math/aabb.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/plane.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class RenderCameraFrustum
     * @brief Enumeration for different camera view sides
     */
    enum class RenderCameraFrustum {
        Left   = 0,
        Right  = 1,
        Bottom = 2,
        Top    = 3,
        Near   = 4,
        Far    = 5,
        Max    = 6
    };

    /**
     * @class RenderCamera
     * @brief Camera class to encapsulate display parameters for rendering
     */
    class RenderCamera final {
    public:
        void set_transform(const Vec3f& position, const Vec3f& direction, const Vec3f& up);
        void set_viewport(int width, int height);
        void set_perspective(float fov, float near, float far);
        void build_perspective();

        bool  is_inside_or_intersects(const Aabbf& box) const;
        float calc_distance(const Aabbf& box) const;

        const Planef*  get_planes() const { return m_planes; }
        const Mat4x4f& get_proj() const { return m_proj; }
        const Mat4x4f& get_view() const { return m_view; }
        const Mat4x4f& get_proj_view() const { return m_proj_view; }
        const Rect2i&  get_viewport() const { return m_viewport; }
        float          get_fov() const { return m_fov; }
        float          get_width() const { return m_width; }
        float          get_height() const { return m_height; }
        float          get_aspect() const { return m_aspect; }
        float          get_near() const { return m_near; }
        float          get_far() const { return m_far; }
        const Vec3f&   get_position() const { return m_position; }
        const Vec3f&   get_direction() const { return m_direction; }
        const Vec3f&   get_up() const { return m_up; }
        const Vec3f&   get_right() const { return m_right; }
        bool           is_perspective() { return m_perspective; };
        bool           is_orthographic() { return m_orthographic; };

    private:
        Planef  m_planes[static_cast<int>(RenderCameraFrustum::Max)];
        Mat4x4f m_proj;
        Mat4x4f m_view;
        Mat4x4f m_proj_view;
        Rect2i  m_viewport;
        float   m_fov    = Math::deg_to_rad(45.0f);
        float   m_width  = 1280.0f;
        float   m_height = 720.0f;
        float   m_aspect = 16.0f / 9.0f;
        float   m_near   = 0.1f;
        float   m_far    = 1000.0f;
        Vec3f   m_position;
        Vec3f   m_direction    = Vec3f::axis_z();
        Vec3f   m_up           = Vec3f::axis_y();
        Vec3f   m_right        = Vec3f::cross(Vec3f::axis_z(), Vec3f::axis_y());
        bool    m_perspective  = false;
        bool    m_orthographic = false;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_CAMERA_HPP
