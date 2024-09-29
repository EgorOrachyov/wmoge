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

#include "io/serialization.hpp"
#include "math/mat.hpp"
#include "math/math_utils2d.hpp"
#include "math/math_utils3d.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class Transform2d
     * @brief Utility to manage 2d space transformations
     */
    class Transform2d {
    public:
        Transform2d() {
            m_scale    = Vec2f(1, 1);
            m_rotation = 0.0f;
        }

        void set_translation(const Vec2f& translation) { m_translation = translation; }
        void set_rotation(float rad) { m_rotation = rad; }
        void set_scale(const Vec2f& scale) { m_scale = scale; }

        void translate(const Vec2f& translation) { m_translation += translation; }
        void rotate(float rad) { m_rotation += rad; }
        void scale(const Vec2f& scale) { m_scale *= scale; }

        const Vec2f& get_translation() const { return m_translation; };
        const Vec2f& get_scale() const { return m_scale; };
        float        get_rotation() const { return m_rotation; };

        Mat3x3f get_transform() const {
            return Math2d::translate(m_translation) *
                   Math2d::rotate_z(m_rotation) *
                   Math2d::scale(m_scale);
        }

        Mat3x3f get_inverse_transform() const {
            return Math2d::scale(Vec2f(1, 1) / m_scale) *
                   Math2d::rotate_z(-m_rotation) *
                   Math2d::translate(-m_translation);
        }

        friend Status tree_read(IoContext& context, IoTree& tree, Transform2d& transform) {
            WG_TREE_READ_AS_OPT(context, tree, "rotation", transform.m_rotation);
            WG_TREE_READ_AS_OPT(context, tree, "translation", transform.m_translation);
            WG_TREE_READ_AS_OPT(context, tree, "scale", transform.m_scale);
            return WG_OK;
        }
        friend Status tree_write(IoContext& context, IoTree& tree, const Transform2d& transform) {
            WG_TREE_MAP(tree);
            WG_TREE_WRITE_AS(context, tree, "rotation", transform.m_rotation);
            WG_TREE_WRITE_AS(context, tree, "translation", transform.m_translation);
            WG_TREE_WRITE_AS(context, tree, "scale", transform.m_scale);

            return WG_OK;
        }
        friend Status stream_read(IoContext& context, IoStream& stream, Transform2d& transform) {
            WG_ARCHIVE_READ(context, stream, transform.m_rotation);
            WG_ARCHIVE_READ(context, stream, transform.m_translation);
            WG_ARCHIVE_READ(context, stream, transform.m_scale);
            return WG_OK;
        }
        friend Status stream_write(IoContext& context, IoStream& stream, const Transform2d& transform) {
            WG_ARCHIVE_WRITE(context, stream, transform.m_rotation);
            WG_ARCHIVE_WRITE(context, stream, transform.m_translation);
            WG_ARCHIVE_WRITE(context, stream, transform.m_scale);
            return WG_OK;
        }

    private:
        Vec2f m_translation;
        Vec2f m_scale;
        float m_rotation;
    };

    /**
     * @class Transform3d
     * @brief Utility to manage 3d space transformations
     */
    class Transform3d {
    public:
        Transform3d() {
            m_scale = Vec3f(1, 1, 1);
        }

        void set_translation(const Vec3f& translation) { m_translation = translation; }
        void set_rotation(const Quatf& quat) { m_rotation = quat; }
        void set_scale(const Vec3f& s) { m_scale = s; }

        void translate(const Vec3f& offset) { m_translation += offset; }
        void rotate(const Vec3f& axis, float rad) { m_rotation = Quatf(axis, rad) * m_rotation; }
        void rotate_x(float rad) { m_rotation = Quatf(Vec3f::axis_x(), rad) * m_rotation; }
        void rotate_y(float rad) { m_rotation = Quatf(Vec3f::axis_y(), rad) * m_rotation; }
        void rotate_z(float rad) { m_rotation = Quatf(Vec3f::axis_z(), rad) * m_rotation; }
        void scale(const Vec3f& scale) { m_scale *= scale; }

        Transform3d inv() const {
            Transform3d t;
            t.set_translation(-m_translation);
            t.set_rotation(m_rotation.conjugate());
            t.set_scale(Vec3f(1.0f, 1.0f, 1.0f) / m_scale);
            return t;
        }

        Vec3f transform(const Vec3f& v) const {
            return m_translation + m_rotation.rotate(m_scale * v);
        }

        [[nodiscard]] Mat4x4f to_mat4x4() const {
            return Math3d::translate(m_translation) *
                   m_rotation.as_matrix() *
                   Math3d::scale(m_scale);
        }

        [[nodiscard]] Mat4x4f to_inv_mat4x4() const {
            return Math3d::scale(Vec3f(1.0f, 1.0f, 1.0f) / m_scale) *
                   m_rotation.inverse().as_matrix() *
                   Math3d::translate(-m_translation);
        }

        [[nodiscard]] const Quatf& get_rotation() const { return m_rotation; }
        [[nodiscard]] const Vec3f& get_translation() const { return m_translation; }
        [[nodiscard]] const Vec3f& get_scale() const { return m_scale; }

        friend Status tree_read(IoContext& context, IoTree& tree, Transform3d& transform) {
            WG_TREE_READ_AS_OPT(context, tree, "rotation", transform.m_rotation);
            WG_TREE_READ_AS_OPT(context, tree, "translation", transform.m_translation);
            WG_TREE_READ_AS_OPT(context, tree, "scale", transform.m_scale);
            return WG_OK;
        }
        friend Status tree_write(IoContext& context, IoTree& tree, const Transform3d& transform) {
            WG_TREE_MAP(tree);
            WG_TREE_WRITE_AS(context, tree, "rotation", transform.m_rotation);
            WG_TREE_WRITE_AS(context, tree, "translation", transform.m_translation);
            WG_TREE_WRITE_AS(context, tree, "scale", transform.m_scale);

            return WG_OK;
        }
        friend Status stream_read(IoContext& context, IoStream& stream, Transform3d& transform) {
            WG_ARCHIVE_READ(context, stream, transform.m_rotation);
            WG_ARCHIVE_READ(context, stream, transform.m_translation);
            WG_ARCHIVE_READ(context, stream, transform.m_scale);
            return WG_OK;
        }
        friend Status stream_write(IoContext& context, IoStream& stream, const Transform3d& transform) {
            WG_ARCHIVE_WRITE(context, stream, transform.m_rotation);
            WG_ARCHIVE_WRITE(context, stream, transform.m_translation);
            WG_ARCHIVE_WRITE(context, stream, transform.m_scale);
            return WG_OK;
        }

    private:
        Quatf m_rotation;
        Vec3f m_translation;
        Vec3f m_scale;
    };

    /**
     * @class TransformEdt
     * @brief Utility to manage 3d space transformations with euler angles (roll, yaw, pitch)
     */
    class TransformEdt {
    public:
        TransformEdt() {
            m_scale = Vec3f(1, 1, 1);
        }

        void translate(const Vec3f& offset) { m_translation += offset; }
        void rotate(const Vec3f& angles) { m_rotation += angles; }
        void scale(const Vec3f& scale) { m_scale *= scale; }

        [[nodiscard]] Transform3d to_transform3d() const {
            Transform3d t;
            t.set_translation(m_translation);
            t.set_scale(m_scale);
            t.set_rotation(Quatf(m_rotation.x(), m_rotation.y(), m_rotation.z()));
            return t;
        }

        [[nodiscard]] Mat4x4f to_mat4x4() const {
            return Math3d::translate(m_translation) *
                   Quatf(m_rotation[0], m_rotation[1], m_rotation[2]).as_matrix() *
                   Math3d::scale(m_scale);
        }

        [[nodiscard]] Mat4x4f to_inv_mat4x4() const {
            return Math3d::scale(Vec3f(1.0f, 1.0f, 1.0f) / m_scale) *
                   Quatf(m_rotation[0], m_rotation[1], m_rotation[2]).inverse().as_matrix() *
                   Math3d::translate(-m_translation);
        }

        [[nodiscard]] const Vec3f& get_rotation() const { return m_rotation; }
        [[nodiscard]] const Vec3f& get_translation() const { return m_translation; }
        [[nodiscard]] const Vec3f& get_scale() const { return m_scale; }

        friend Status tree_read(IoContext& context, IoTree& tree, TransformEdt& transform) {
            Vec3f rotation_deg;

            WG_TREE_READ_AS_OPT(context, tree, "rotation", rotation_deg);
            WG_TREE_READ_AS_OPT(context, tree, "translation", transform.m_translation);
            WG_TREE_READ_AS_OPT(context, tree, "scale", transform.m_scale);

            transform.m_rotation.values[0] = Math::deg_to_rad(rotation_deg[0]);
            transform.m_rotation.values[1] = Math::deg_to_rad(rotation_deg[1]);
            transform.m_rotation.values[2] = Math::deg_to_rad(rotation_deg[2]);

            return WG_OK;
        }
        friend Status tree_write(IoContext& context, IoTree& tree, const TransformEdt& transform) {
            Vec3f rotation_deg;

            rotation_deg[0] = Math::rad_to_deg(transform.m_rotation.values[0]);
            rotation_deg[1] = Math::rad_to_deg(transform.m_rotation.values[1]);
            rotation_deg[2] = Math::rad_to_deg(transform.m_rotation.values[2]);

            WG_TREE_MAP(tree);
            WG_TREE_WRITE_AS(context, tree, "rotation", rotation_deg);
            WG_TREE_WRITE_AS(context, tree, "translation", transform.m_translation);
            WG_TREE_WRITE_AS(context, tree, "scale", transform.m_scale);

            return WG_OK;
        }
        friend Status stream_read(IoContext& context, IoStream& stream, TransformEdt& transform) {
            WG_ARCHIVE_READ(context, stream, transform.m_rotation);
            WG_ARCHIVE_READ(context, stream, transform.m_translation);
            WG_ARCHIVE_READ(context, stream, transform.m_scale);
            return WG_OK;
        }
        friend Status stream_write(IoContext& context, IoStream& stream, const TransformEdt& transform) {
            WG_ARCHIVE_WRITE(context, stream, transform.m_rotation);
            WG_ARCHIVE_WRITE(context, stream, transform.m_translation);
            WG_ARCHIVE_WRITE(context, stream, transform.m_scale);
            return WG_OK;
        }

    private:
        Vec3f m_rotation;
        Vec3f m_translation;
        Vec3f m_scale;
    };

}// namespace wmoge