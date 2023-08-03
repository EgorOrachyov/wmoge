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
#include "math/frustum.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/plane.hpp"
#include "math/vec.hpp"

#include <vector>

namespace wmoge {

    /**
     * @brief Type of projection for cameras
     */
    enum class CameraProjection {
        Perspective = 0,
        Orthographic
    };

    /**
     * @class RenderCamera
     * @brief Camera data used for scene objects rendering
     */
    class RenderCamera final {
    public:
        void update_matrices();
        void update_frustum();

        [[nodiscard]] bool  is_inside_or_intersects(const Aabbf& box) const;
        [[nodiscard]] float distance(const Aabbf& box) const;

    public:
        Frustumf frustum;

        Mat4x4f proj;
        Mat4x4f view;
        Mat4x4f proj_view;
        Rect2i  viewport;

        float fov    = Math::deg_to_rad(45.0f);
        float width  = 1280.0f;
        float height = 720.0f;
        float aspect = 16.0f / 9.0f;
        float near   = 0.1f;
        float far    = 1000.0f;

        Vec3f position;
        Vec3f direction = Vec3f::axis_z();
        Vec3f up        = Vec3f::axis_y();

        CameraProjection projection = CameraProjection::Perspective;
    };

    /**
     * @brief Cameras list for rendering
     */
    using RenderCameras = std::vector<RenderCamera>;

}// namespace wmoge

#endif//WMOGE_RENDER_CAMERA_HPP
