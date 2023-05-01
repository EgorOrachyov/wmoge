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

#include "spatial_2d.hpp"

#include "scene/scene_object.hpp"

namespace wmoge {

    void Spatial2d::update_transform(const Transform2d& transform) {
        m_transform = transform;
        flush();
    }
    void Spatial2d::translate(const Vec2f& translation) {
        m_transform.translate(translation);
    }
    void Spatial2d::rotate(float angle_rad) {
        m_transform.rotate(angle_rad);
    }
    void Spatial2d::scale(const Vec2f& scale) {
        m_transform.scale(scale);
    }
    void Spatial2d::flush() {
        get_scene_object()->on_transform_updated();
    }

    Mat3x3f Spatial2d::get_matr_local() {
        return m_transform.get_transform();
    }
    Mat3x3f Spatial2d::get_matr_local_inv() {
        return m_transform.get_inverse_transform();
    }
    const Mat3x3f& Spatial2d::get_matr_global() {
        return m_matr_global;
    }
    const Mat3x3f& Spatial2d::get_matr_global_inv() {
        return m_matr_global_inv;
    }
    const Transform2d& Spatial2d::get_transform() {
        return m_transform;
    }

    void Spatial2d::on_scene_enter() {
        SceneComponent::on_scene_enter();

        auto parent = get_scene_object()->get_parent();

        if (parent) {
            assert(parent->get<Spatial2d>());
            m_parent = parent->get_or_create<Spatial2d>();
        }

        on_transform_updated();
    }
    void Spatial2d::on_transform_updated() {
        SceneComponent::on_transform_updated();

        m_matr_global     = get_matr_local();
        m_matr_global_inv = get_matr_local_inv();

        if (m_parent) {
            m_matr_global     = m_parent->m_matr_global * m_matr_global;
            m_matr_global_inv = m_matr_global_inv * m_parent->m_matr_global_inv;
        }
    }
    bool Spatial2d::on_load_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneComponent::on_load_from_yaml(node)) {
            return false;
        }

        if (node.has_child("pos")) {
            m_transform.translate(Yaml::read_vec2f(node["pos"]));
        }
        if (node.has_child("scale")) {
            m_transform.scale(Yaml::read_vec2f(node["scale"]));
        }
        if (node.has_child("angle")) {
            m_transform.rotate(Yaml::read_float(node["angle"]));
        }

        return true;
    }

}// namespace wmoge