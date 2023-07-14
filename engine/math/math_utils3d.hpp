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

#ifndef WMOGE_MATH_UTILS3D_HPP
#define WMOGE_MATH_UTILS3D_HPP

#include "mat.hpp"
#include "math_utils.hpp"
#include "vec.hpp"

namespace wmoge {

    class Math3d {
    public:
        static Mat4x4f identity() {
            Mat4x4f mat;
            mat[0][0] = 1.0f;
            mat[1][1] = 1.0f;
            mat[2][2] = 1.0f;
            mat[3][3] = 1.0f;
            return mat;
        }

        static Mat4x4f scale(const Vec3f& scale) {
            Mat4x4f mat = identity();
            mat[0][0]   = scale[0];
            mat[1][1]   = scale[1];
            mat[2][2]   = scale[2];
            return mat;
        }

        static Mat4x4f translate(const Vec3f& translation) {
            Mat4x4f mat = identity();
            mat[0][3]   = translation[0];
            mat[1][3]   = translation[1];
            mat[2][3]   = translation[2];
            return mat;
        }

        /** @brief Clockwise around axis rotation */
        static Mat4x4f rotate_x(float angle) {
            auto s = Math::sin(angle);
            auto c = Math::cos(angle);

            return Mat4x4f(1, 0, 0, 0,
                           0, c, -s, 0,
                           0, s, c, 0,
                           0, 0, 0, 1);
        }

        /** @brief Clockwise around axis rotation */
        static Mat4x4f rotate_y(float angle) {
            auto s = Math::sin(angle);
            auto c = Math::cos(angle);

            return Mat4x4f(c, 0, s, 0,
                           0, 1, 0, 0,
                           -s, 0, c, 0,
                           0, 0, 0, 1);
        }

        /** @brief Clockwise around axis rotation */
        static Mat4x4f rotate_z(float angle) {
            auto s = Math::sin(angle);
            auto c = Math::cos(angle);

            return Mat4x4f(c, -s, 0, 0,
                           s, c, 0, 0,
                           0, 0, 1, 0,
                           0, 0, 0, 1);
        }

        /** @brief Clockwise rotation around an arbitrary axis */
        static Mat4x4f rotate(const Vec3f& axis, float angle) {
            auto Ax        = axis.normalized();
            auto s         = Math::sin(angle);
            auto c         = Math::cos(angle);
            auto one_min_c = 1 - c;

            return Mat4x4f(
                    // 0 row
                    c + Ax[0] * Ax[0] * one_min_c,
                    Ax[0] * Ax[1] * one_min_c - Ax[2] * s,
                    Ax[0] * Ax[2] * one_min_c + Ax[1] * s,
                    0,

                    // 1 row
                    Ax[1] * Ax[0] * one_min_c + Ax[2] * s,
                    c + Ax[1] * Ax[1] * one_min_c,
                    Ax[1] * Ax[2] * one_min_c - Ax[0] * s,
                    0,

                    // 2 row
                    Ax[2] * Ax[0] * one_min_c - Ax[1] * s,
                    Ax[2] * Ax[1] * one_min_c + Ax[0] * s,
                    c + Ax[2] * Ax[2] * one_min_c,
                    0,

                    // 3 row
                    0, 0, 0, 1);
        }

        static Vec3f transform(const Mat4x4f& mat, const Vec3f& vec) {
            return Vec3f(mat * Vec4f(vec, 1.0f));
        }

        static Vec3f transform_w0(const Mat4x4f& mat, const Vec3f& vec) {
            return Vec3f(mat * Vec4f(vec, 0.0f));
        }

        /**
         *      | y
         *      |
         *      |_____ x
         *     /
         *    /
         *   /z
         *
         * @brief Look at view matrix for camera (in OpenGL style)
         * @note Final are is located in the negative z space
         *
         * @param eye Current viewer position
         * @param direction Direction vector of viewing
         * @param up Up vector to define orientation
         * @return Look matrix to transform world position to the View space
         */
        static Mat4x4f look_at(const Vec3f& eye, const Vec3f& direction, const Vec3f& up) {
            auto Z = (-direction).normalized();
            auto X = Vec3f::cross(up, Z).normalized();
            auto Y = Vec3f::cross(Z, X);

            return Mat4x4f(X[0], X[1], X[2], -Vec3f::dot(X, eye),
                           Y[0], Y[1], Y[2], -Vec3f::dot(Y, eye),
                           Z[0], Z[1], Z[2], -Vec3f::dot(Z, eye),
                           0.0f, 0.0f, 0.0f, 1.0f);
        }

        /**
         * @brief Perspective projection
         *
         * Perspective projection to range [-1..1]x[-1..1]x[-1..1].
         * Note that z axis value are in range [-1..1] (as in OpenGL).
         *
         * @warning fov should be me more than 0
         * @warning aspect should be more than 0
         *
         * @param fov   Angle between top and bottom sides in radians
         * @param aspect Width / height ratio
         * @param near   Near clip plane
         * @param far    Far clip plane
         *
         * @return Perspective matrix
         */
        static Mat4x4f perspective(float fov, float aspect, float near, float far) {
            float ctg_angle = 1.0f / Math::tan(fov / 2.0f);

            return Mat4x4f(ctg_angle / aspect, 0.0f, 0.0f, 0.0f,
                           0.0f, ctg_angle, 0.0f, 0.0f,
                           0.0f, 0.0f, (far + near) / (near - far), (2 * far * near) / (near - far),
                           0.0f, 0.0f, -1.0f, 0.0f);
        }

        /**
         * @brief Orthographic projection
         *
         * Orthographic projection to range [-1..1]x[-1..1]x[-1..1].
         * Note that z axis value are in range [-1..1] (as in OpenGL).
         *
         * @note Left < right
         * @note Bottom < top
         * @note Near < far
         *
         * @return Orthographic matrix
         */
        static Mat4x4f orthographic(float left, float right, float bottom, float top, float near, float far) {
            return Mat4x4f(2.0f / (right - left), 0.0f, 0.0f, (right + left) / (left - right),
                           0.0f, 2.0f / (top - bottom), 0.0f, (top + bottom) / (bottom - top),
                           0.0f, 0.0f, 2.0f / (far - near), (far + near) / (near - far),
                           0.0f, 0.0f, 0.0f, 1.0f);
        }

        static Vec2f project_to_screen(const Mat4x4f& mat, const Vec2f& area, const Vec3f& point) {
            const Vec4f world_space = mat * Vec4f(point, 1.0f);
            const Vec4f ndc         = world_space / world_space.w();
            return area * (Vec2f(ndc.x(), ndc.y()) * 0.5f + Vec2f(0.5f, 0.5f));
        }
    };

}// namespace wmoge

#endif//WMOGE_MATH_UTILS3D_HPP
