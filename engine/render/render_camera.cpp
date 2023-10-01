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

    void RenderCamera::update_matrices() {
        if (projection == CameraProjection::Perspective) {
            proj = Math3d::perspective(fov, aspect, near, far);
        }

        view      = Math3d::look_at(position, direction, up);
        proj_view = proj * view;
    }
    void RenderCamera::update_frustum() {
        frustum = Frustumf(position, direction, up, fov, aspect, near, far);
    }

    bool RenderCamera::is_inside_or_intersects(const Aabbf& box) const {
        return frustum.is_inside_or_intersects(box);
    }
    float RenderCamera::distance(const Aabbf& box) const {
        return box.distance(position);
    }

    int RenderCameras::add_camera(const RenderCamera& camera) {
        const int index = int(m_cameras.size());
        m_cameras.push_back(camera);
        return index;
    }
    const RenderCamera& RenderCameras::at(int index) {
        return m_cameras[index];
    }
    void RenderCameras::clear() {
        m_cameras.clear();
    }

}// namespace wmoge