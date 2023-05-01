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

#ifndef WMOGE_MATH_UTILS2D_HPP
#define WMOGE_MATH_UTILS2D_HPP

#include "math/mat.hpp"
#include "math/math_utils.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"

namespace wmoge {

    class Math2d {
    public:
        static Mat2x2f identity2x2() {
            return {1, 0,
                    0, 1};
        }

        static Mat3x3f identity3x3() {
            return {1, 0, 0,
                    0, 1, 0,
                    0, 0, 1};
        }

        static Mat4x4f from3x3to4x4(const Mat3x3f& mat) {
            return Mat4x4f{mat.values[0], mat.values[1], 0, mat.values[2],
                           mat.values[3], mat.values[4], 0, mat.values[5],
                           mat.values[6], mat.values[7], 1, mat.values[8],
                           0, 0, 0, 1};
        }

        /** Counter clockwise rotation around imaginable Z axis for 2D space  */
        static Mat3x3f rotate_z(float angle_rad) {
            auto s = Math::sin(angle_rad);
            auto c = Math::cos(angle_rad);

            return Mat3x3f{c, -s, 0,
                           s, c, 0,
                           0, 0, 1};
        }

        static Mat3x3f scale(const Vec2f& scale) {
            return Mat3x3f{scale.x(), 0, 0,
                           0, scale.y(), 0,
                           0, 0, 1};
        }

        static Mat3x3f translate(const Vec2f& translation) {
            return Mat3x3f{1, 0, translation.x(),
                           0, 1, translation.y(),
                           0, 0, 1};
        }

        static Mat3x3f translate_rotate_z(const Vec2f& translation, float angle_rad) {
            auto s = Math::sin(angle_rad);
            auto c = Math::cos(angle_rad);

            return Mat3x3f{c, -s, translation.x(),
                           s, c, translation.y(),
                           0, 0, 1};
        }

        static Vec2f transform(const Mat3x3f& mat, const Vec2f& point) {
            return Vec2f(mat * Vec3f(point, 1));
        }

        static bool intersects(const Vec2f& rect1, const Vec2f& rect2,
                               const Vec2f& pivot1, const Vec2f& pivot2,
                               const Mat3x3f& mat1, const Mat3x3f& mat2,
                               const Mat3x3f& inv_mat1, const Mat3x3f& inv_mat2) {
            Vec2f points1[4];
            points1[0] = Vec2f(0, 0) - pivot1;
            points1[1] = Vec2f(0, rect1.y()) - pivot1;
            points1[2] = Vec2f(rect1.x(), 0) - pivot1;
            points1[3] = Vec2f(rect1.x(), rect1.y()) - pivot1;

            Vec2f points2[4];
            points2[0] = Vec2f(0, 0) - pivot2;
            points2[1] = Vec2f(0, rect2.y()) - pivot2;
            points2[2] = Vec2f(rect2.x(), 0) - pivot2;
            points2[3] = Vec2f(rect2.x(), rect2.y()) - pivot2;

            for (auto& p : points1) {
                p = Math2d::transform(mat1, p);
            }
            for (auto& p : points2) {
                p = Math2d::transform(mat2, p);
            }

            for (auto p : points1) {
                p = Math2d::transform(inv_mat2, p) + pivot2;
                if (0 <= p.x() && 0 <= p.y() && p.x() <= rect2.x() && p.y() <= rect2.y()) {
                    return true;
                }
            }
            for (auto p : points2) {
                p = Math2d::transform(inv_mat1, p) + pivot1;
                if (0 <= p.x() && 0 <= p.y() && p.x() <= rect1.x() && p.y() <= rect1.y()) {
                    return true;
                }
            }

            return false;
        }
    };

}// namespace wmoge

#endif//WMOGE_MATH_UTILS2D_HPP
