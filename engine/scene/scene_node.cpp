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

#include "scene_node.hpp"

#include "scene/scene_tree.hpp"
#include "scene/scene_tree_visitor.hpp"

#include <cassert>

namespace wmoge {

    SceneNode::SceneNode(SceneTree* tree) : m_tree(tree), m_uuid(UUID::generate()) {
        assert(tree);
    }

    bool SceneNode::visit(class SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    EcsArch SceneNode::get_arch() {
        return EcsArch{};
    }

    void SceneNode::set_transform(const TransformEdt& transform) {
        m_transform = transform;
        on_transformed();
    }
    void SceneNode::set_name(const StringId& name) {
        m_name = name;
        on_renamed();
    }

    void SceneNode::add_child(const Ref<SceneNode>& child) {
        assert(child);
        assert(child->m_parent == nullptr);

        m_children.push_back(child);
        child->m_parent = this;
        child->on_parented();
    }
    void SceneNode::remove_child(const Ref<SceneNode>& child) {
        assert(child);
        assert(child->m_parent == this);

        child->on_unparented();
        child->m_parent = nullptr;
        m_children.erase(std::find(m_children.begin(), m_children.end(), child));
    }

    bool SceneNode::on_yaml_read(const YamlConstNodeRef& node) {
        WG_YAML_READ_AS(node, "uuid", m_uuid);
        WG_YAML_READ_AS(node, "name", m_name);
        WG_YAML_READ_AS(node, "transform", m_transform);

        return true;
    }
    bool SceneNode::on_yaml_write(YamlNodeRef node) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "uuid", m_uuid);
        WG_YAML_WRITE_AS(node, "name", m_name);
        WG_YAML_WRITE_AS(node, "transform", m_transform);

        return true;
    }

    Mat4x4f SceneNode::get_lt() const {
        return m_transform.get_transform();
    }
    Mat4x4f SceneNode::get_lt_inverse() const {
        return m_transform.get_inverse_transform();
    }
    Mat4x4f SceneNode::get_l2w() const {
        const Mat4x4f matrix = get_lt();
        return m_parent ? m_parent->get_l2w() * matrix : matrix;
    }
    Mat4x4f SceneNode::get_w2l() const {
        const Mat4x4f matrix = get_lt_inverse();
        return m_parent ? matrix * m_parent->get_w2l() : matrix;
    }

    void SceneNode::register_class() {
        auto* cls = Class::register_class<SceneNode>();
        cls->add_field(ClassField(VarType::StringId, SID("name")), &SceneNode::m_name);
    }

}// namespace wmoge