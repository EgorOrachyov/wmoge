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

#ifndef WMOGE_FRUSTUM_HPP
#define WMOGE_FRUSTUM_HPP

#include "math/aabb.hpp"
#include "math/math_utils.hpp"
#include "math/plane.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @brief Enumeration for different frustum view sides
     */
    enum class FrustumSide {
        Left = 0,
        Right,
        Bottom,
        Top,
        Near,
        Far,
        Max
    };

    /**
     * @brief Enumeration for different frustum view points
     */
    enum class FrustumPoint {
        NearLeftUp = 0,
        NearLeftDown,
        NearRightUp,
        NearRightDown,
        FarLeftUp,
        FarLeftDown,
        FarRightUp,
        FarRightDown,
        Max
    };

    template<typename T>
    class TFrustum {
    public:
        using Vec   = TVecN<T, 3>;
        using Plane = TPlane<T>;

        static constexpr int PLANES_COUNT = 6;
        static constexpr int POINTS_COUNT = 8;

        TFrustum() = default;

        TFrustum(Vec pos, Vec dir, Vec up, T fov, T aspect, T near, T far) {
            const float tan_fh  = Math::tan(fov * 0.5f);
            const float hnear_h = tan_fh * near;
            const float hnear_w = hnear_h * aspect;
            const float hfar_h  = tan_fh * far;
            const float hfar_w  = hfar_h * aspect;

            up         = up.normalized();
            dir        = dir.normalized();
            auto right = Vec3f::cross(dir, up).normalized();
            up         = Vec3f::cross(right, dir).normalized();

            const Vec3f p_near_left_up    = pos + dir * near + right * (-hnear_w) + up * hnear_h;
            const Vec3f p_near_left_down  = pos + dir * near + right * (-hnear_w) + up * (-hnear_h);
            const Vec3f p_near_right_up   = pos + dir * near + right * hnear_w + up * hnear_h;
            const Vec3f p_near_right_down = pos + dir * near + right * hnear_w + up * (-hnear_h);
            const Vec3f p_far_left_up     = pos + dir * far + right * (-hfar_w) + up * hfar_h;
            const Vec3f p_far_left_down   = pos + dir * far + right * (-hfar_w) + up * (-hfar_h);
            const Vec3f p_far_right_up    = pos + dir * far + right * hfar_w + up * hfar_h;
            const Vec3f p_far_right_down  = pos + dir * far + right * hfar_w + up * (-hfar_h);

            planes[0] = Planef(p_near_left_up, p_near_left_down, p_far_left_down);    // Left
            planes[1] = Planef(p_near_right_up, p_far_right_down, p_near_right_down); // Right
            planes[2] = Planef(p_near_left_down, p_near_right_down, p_far_right_down);// Bottom
            planes[3] = Planef(p_near_right_up, p_near_left_up, p_far_right_up);      // Top
            planes[4] = Planef(p_near_right_up, p_near_right_down, p_near_left_down); // Near
            planes[5] = Planef(p_far_left_up, p_far_left_down, p_far_right_down);     // Far

            points[0] = p_near_left_up;
            points[1] = p_near_left_down;
            points[2] = p_near_right_up;
            points[3] = p_near_right_down;
            points[4] = p_far_left_up;
            points[5] = p_far_left_down;
            points[6] = p_far_right_up;
            points[7] = p_far_right_down;
        }

        [[nodiscard]] bool is_inside_or_intersects(const Aabbf& box) const {
            const auto c = box.center();
            const auto e = box.extent();

            for (const auto& p : planes) {
                const auto r = Vec3f::dot(e, p.norm.abs());
                const auto s = p.dot(c);

                if (s < -r) return false;
            }

            return true;
        }

        [[nodiscard]] const Plane& plane(int i) { return planes[i]; }
        [[nodiscard]] const Plane& plane(FrustumSide side) { return planes[int(side)]; }

        [[nodiscard]] const Plane& point(int i) { return points[i]; }
        [[nodiscard]] const Plane& point(FrustumPoint p) { return points[int(p)]; }

    private:
        Plane planes[PLANES_COUNT] = {};
        Vec   points[POINTS_COUNT] = {};
    };

    using Frustumf = TFrustum<float>;

}// namespace wmoge

#endif//WMOGE_FRUSTUM_HPP
