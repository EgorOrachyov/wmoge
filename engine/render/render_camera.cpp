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

#include "render_camera.hpp"

namespace wmoge {

    void RenderCamera::set_transform(const Vec3f& position, const Vec3f& direction, const Vec3f& up) {
        m_position  = position;
        m_direction = direction;
        m_up        = up;
        m_right     = Vec3f::cross(m_direction, m_up);
    }
    void RenderCamera::set_viewport(int width, int height) {
        m_viewport = Rect2i(0, 0, width, height);
        m_aspect   = height > 0.0f ? float(width) / float(height) : 1.0f;
    }
    void RenderCamera::set_perspective(float fov, float near, float far) {
        m_fov  = fov;
        m_near = near;
        m_far  = far;
    }
    void RenderCamera::build_perspective() {
        m_proj        = Math3d::perspective(m_fov, m_aspect, m_near, m_far);
        m_view        = Math3d::look_at(m_position, m_direction, m_up);
        m_proj_view   = m_proj * m_view;
        m_perspective = true;

        const float tan_fh  = Math::tan(m_fov * 0.5f);
        const float hnear_h = tan_fh * m_near;
        const float hnear_w = hnear_h * m_aspect;
        const float hfar_h  = tan_fh * m_far;
        const float hfar_w  = hfar_h * m_aspect;

        const Vec3f p_near_left_up    = m_position + m_direction * m_near + m_right * (-hnear_w) + m_up * hnear_h;
        const Vec3f p_near_left_down  = m_position + m_direction * m_near + m_right * (-hnear_w) + m_up * (-hnear_h);
        const Vec3f p_near_right_up   = m_position + m_direction * m_near + m_right * hnear_w + m_up * hnear_h;
        const Vec3f p_near_right_down = m_position + m_direction * m_near + m_right * hnear_w + m_up * (-hnear_h);
        const Vec3f p_far_left_up     = m_position + m_direction * m_far + m_right * (-hfar_w) + m_up * hfar_h;
        const Vec3f p_far_left_down   = m_position + m_direction * m_far + m_right * (-hfar_w) + m_up * (-hfar_h);
        const Vec3f p_far_right_up    = m_position + m_direction * m_far + m_right * hfar_w + m_up * hfar_h;
        const Vec3f p_far_right_down  = m_position + m_direction * m_far + m_right * hfar_w + m_up * (-hfar_h);

        m_planes[0] = Planef(p_near_left_up, p_near_left_down, p_far_left_down);
        m_planes[1] = Planef(p_far_right_down, p_near_right_down, p_near_right_up);
        m_planes[2] = Planef(p_near_left_down, p_near_right_down, p_far_right_down);
        m_planes[3] = Planef(p_far_right_up, p_near_right_up, p_near_left_up);
        m_planes[4] = Planef(p_near_right_up, p_near_right_down, p_near_left_down);
        m_planes[5] = Planef(p_far_left_up, p_far_left_down, p_far_right_down);
    }

    bool RenderCamera::is_inside_or_intersects(const Aabbf& box) const {
        const auto c = box.center();
        const auto e = box.extent();

        for (const auto& p : m_planes) {
            const auto r = Vec3f::dot(e, p.norm.abs());
            const auto s = p.dot(c);

            if (s < -r) return false;
        }

        return true;
    }
    float RenderCamera::calc_distance(const Aabbf& box) const {
        return (box.center() - m_position).length();
    }

}// namespace wmoge