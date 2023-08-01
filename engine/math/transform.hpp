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

#ifndef WMOGE_TRANSFORM_HPP
#define WMOGE_TRANSFORM_HPP

#include "io/yaml.hpp"
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

        friend bool yaml_read(const YamlConstNodeRef& node, Transform2d& transform) {
            WG_YAML_READ_AS_OPT(node, "rotation", transform.m_rotation);
            WG_YAML_READ_AS_OPT(node, "translation", transform.m_translation);
            WG_YAML_READ_AS_OPT(node, "scale", transform.m_scale);
            return true;
        }
        friend bool yaml_write(YamlNodeRef node, const Transform2d& transform) {
            WG_YAML_MAP(node);
            WG_YAML_WRITE_AS(node, "rotation", transform.m_rotation);
            WG_YAML_WRITE_AS(node, "translation", transform.m_translation);
            WG_YAML_WRITE_AS(node, "scale", transform.m_scale);
            return true;
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

        void translate(const Vec3f& offset) { m_translation += offset; }
        void rotate(const Vec3f& axis, float rad) { m_rotation = Quatf(axis, rad) * m_rotation; }
        void rotate_x(float rad) { m_rotation = Quatf(Vec3f::axis_x(), rad) * m_rotation; }
        void rotate_y(float rad) { m_rotation = Quatf(Vec3f::axis_y(), rad) * m_rotation; }
        void rotate_z(float rad) { m_rotation = Quatf(Vec3f::axis_z(), rad) * m_rotation; }
        void scale(const Vec3f& scale) { m_scale *= scale; }

        [[nodiscard]] Mat4x4f get_transform() const {
            return Math3d::translate(m_translation) *
                   m_rotation.as_matrix() *
                   Math3d::scale(m_scale);
        }

        [[nodiscard]] Mat4x4f get_inverse_transform() const {
            return Math3d::scale(Vec3f(1.0f, 1.0f, 1.0f) / m_scale) *
                   m_rotation.inverse().as_matrix() *
                   Math3d::translate(-m_translation);
        }

        [[nodiscard]] const Quatf& get_rotation() const { return m_rotation; }
        [[nodiscard]] const Vec3f& get_translation() const { return m_translation; }
        [[nodiscard]] const Vec3f& get_scale() const { return m_scale; }

        friend bool yaml_read(const YamlConstNodeRef& node, Transform3d& transform) {
            WG_YAML_READ_AS_OPT(node, "rotation", transform.m_rotation);
            WG_YAML_READ_AS_OPT(node, "translation", transform.m_translation);
            WG_YAML_READ_AS_OPT(node, "scale", transform.m_scale);
            return true;
        }
        friend bool yaml_write(YamlNodeRef node, const Transform3d& transform) {
            WG_YAML_MAP(node);
            WG_YAML_WRITE_AS(node, "rotation", transform.m_rotation);
            WG_YAML_WRITE_AS(node, "translation", transform.m_translation);
            WG_YAML_WRITE_AS(node, "scale", transform.m_scale);
            return true;
        }

    private:
        Quatf m_rotation;
        Vec3f m_translation;
        Vec3f m_scale;
    };

    /**
     * @class TransformEdt
     * @brief Utility to manage 3d space transformations with euler angles
     */
    class TransformEdt {
    public:
        TransformEdt() {
            m_scale = Vec3f(1, 1, 1);
        }

        void translate(const Vec3f& offset) { m_translation += offset; }
        void rotate(const Vec3f& angles) { m_rotation += angles; }
        void scale(const Vec3f& scale) { m_scale *= scale; }

        [[nodiscard]] Mat4x4f get_transform() const {
            return Math3d::translate(m_translation) *
                   Quatf(m_rotation[0], m_rotation[1], m_rotation[2]).as_matrix() *
                   Math3d::scale(m_scale);
        }

        [[nodiscard]] Mat4x4f get_inverse_transform() const {
            return Math3d::scale(Vec3f(1.0f, 1.0f, 1.0f) / m_scale) *
                   Quatf(m_rotation[0], m_rotation[1], m_rotation[2]).inverse().as_matrix() *
                   Math3d::translate(-m_translation);
        }

        [[nodiscard]] const Vec3f& get_rotation() const { return m_rotation; }
        [[nodiscard]] const Vec3f& get_translation() const { return m_translation; }
        [[nodiscard]] const Vec3f& get_scale() const { return m_scale; }

        friend bool yaml_read(const YamlConstNodeRef& node, TransformEdt& transform) {
            WG_YAML_READ_AS_OPT(node, "rotation", transform.m_rotation);
            WG_YAML_READ_AS_OPT(node, "translation", transform.m_translation);
            WG_YAML_READ_AS_OPT(node, "scale", transform.m_scale);
            return true;
        }
        friend bool yaml_write(YamlNodeRef node, const TransformEdt& transform) {
            WG_YAML_MAP(node);
            WG_YAML_WRITE_AS(node, "rotation", transform.m_rotation);
            WG_YAML_WRITE_AS(node, "translation", transform.m_translation);
            WG_YAML_WRITE_AS(node, "scale", transform.m_scale);
            return true;
        }

    private:
        Vec3f m_rotation;
        Vec3f m_translation;
        Vec3f m_scale;
    };

}// namespace wmoge

#endif//WMOGE_TRANSFORM_HPP
