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

    void RenderCamera::set_proj_params(float fov, float aspect, float near, float far) {
        m_fov    = fov;
        m_aspect = aspect;
        m_near   = near;
        m_far    = far;
    }
    void RenderCamera::set_viewport(const Rect2i& viewport) {
        m_viewport = viewport;
    }
    void RenderCamera::look(const Vec3f& dir, const Vec3f& up) {
        m_direction = dir;
        m_up        = up;
    }
    void RenderCamera::move(const Vec3f& delta) {
        m_position += delta;
    }
    void RenderCamera::move_to(const Vec3f& point) {
        m_position = point;
    }

    void RenderCamera::validate() {
        update_matrices();
        update_frustum();
    }

    bool RenderCamera::is_inside_or_intersects(const Aabbf& box) const {
        return m_frustum.is_inside_or_intersects(box);
    }
    float RenderCamera::distance(const Aabbf& box) const {
        return box.distance(m_position);
    }

    void RenderCamera::update_matrices() {
        if (m_projection == CameraProjection::Perspective) {
            m_proj = Math3d::perspective(m_fov, m_aspect, m_near, m_far);
        }
        m_view      = Math3d::look_at(m_position, m_direction, m_up);
        m_proj_view = m_proj * m_view;
    }
    void RenderCamera::update_frustum() {
        m_frustum = Frustumf(m_position, m_direction, m_up, m_fov, m_aspect, m_near, m_far);
    }

    int RenderCameras::add_camera(CameraType type, const RenderCamera& camera, std::optional<RenderCamera> camera_prev) {
        const int index = int(m_cameras.size());

        RenderCameraData& data = m_cameras.emplace_back();
        data.camera            = camera;
        data.type              = type;
        data.proj              = camera.get_proj();
        data.proj_prev         = camera.get_proj();
        data.view              = camera.get_view();
        data.view_prev         = camera.get_view();
        data.proj_view         = camera.get_proj_view();
        data.proj_view_prev    = camera.get_proj_view();
        data.viewport          = camera.get_viewport();
        data.direction         = camera.get_direction();
        data.direction_prev    = camera.get_direction();
        data.position          = camera.get_position();
        data.position_prev     = camera.get_position();
        data.up                = camera.get_up();
        data.up_prev           = camera.get_up();

        if (camera_prev.has_value()) {
            data.proj_prev      = camera_prev->get_proj();
            data.view_prev      = camera_prev->get_view();
            data.proj_view_prev = camera_prev->get_proj_view();
            data.direction_prev = camera_prev->get_direction();
            data.position_prev  = camera_prev->get_position();
            data.up_prev        = camera_prev->get_up();
            data.movement       = data.position - data.position_prev;
        }

        return index;
    }

    const RenderCamera& RenderCameras::camera_at(int index) const {
        return m_cameras[index].camera;
    }

    const RenderCameraData& RenderCameras::data_at(int index) const {
        return m_cameras[index];
    }

    const RenderCameraData& RenderCameras::camera_main() const {
        return m_cameras[0];
    }

    void RenderCameras::clear() {
        m_cameras.clear();
    }

}// namespace wmoge