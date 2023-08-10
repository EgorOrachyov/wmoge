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

    SceneNode::SceneNode(SceneTree* tree) : m_uuid(UUID::generate()) {
        assert(tree);
    }

    void SceneNode::set_name(const StringId& name) {
        m_name = name;
    }

    void SceneNode::add_child(const Ref<SceneNode>& child) {
        assert(child);
        assert(child->m_parent == nullptr);

        m_children.push_back(child);
        child->m_parent = this;
    }
    void SceneNode::remove_child(const Ref<SceneNode>& child) {
        assert(child);
        assert(child->m_parent == this);

        child->m_parent = nullptr;
        m_children.erase(std::find(m_children.begin(), m_children.end(), child));
    }

    bool SceneNode::on_yaml_read(const YamlConstNodeRef& node) {
        WG_YAML_READ_AS_OPT(node, "uuid", m_uuid);
        WG_YAML_READ_AS_OPT(node, "name", m_name);

        if (!m_uuid) {
            m_uuid = UUID::generate();
        }

        if (m_name.empty()) {
            m_name = SID(class_ptr()->name().str() + "_" + m_uuid.to_str());
        }

        if (node.has_child("children")) {
            for (auto child_itr = node["children"].first_child(); child_itr.valid(); child_itr = child_itr.next_sibling()) {
                StringId class_name;

                WG_YAML_READ_AS(child_itr, "class", class_name);

                Class* class_ptr = Class::class_ptr(class_name);

                if (!class_ptr) {
                    WG_LOG_ERROR("no such class " << class_name << " to instantiate child of node " << m_name);
                    return false;
                }

                Ref<SceneNode> child = class_ptr->instantiate().cast<SceneNode>();

                if (!child) {
                    WG_LOG_ERROR("failed to instantiate child of " << m_name);
                    return false;
                }

                if (!child->on_yaml_read(child_itr)) {
                    WG_LOG_ERROR("failed to parse child of " << m_name);
                    return false;
                }

                m_children.push_back(child);
            }
        }

        return true;
    }
    bool SceneNode::on_yaml_write(YamlNodeRef node) const {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "class", class_ptr()->name());
        WG_YAML_WRITE_AS(node, "uuid", m_uuid);
        WG_YAML_WRITE_AS(node, "name", m_name);
        WG_YAML_WRITE_AS_OPT(node, "children", !m_children.empty(), m_children);

        return true;
    }
    bool SceneNode::on_visit(class SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    void SceneNode::copy_to(SceneNode& other) const {
        other.m_name = m_name;
        other.m_uuid = m_uuid;

        for (auto& child : m_children) {
            Ref<SceneNode> child_copy = child->class_ptr()->instantiate().cast<SceneNode>();
            other.m_children.push_back(child_copy);

            child_copy->m_parent = &other;
            child->copy_to(*child_copy);
        }
    }

    bool yaml_read(const YamlConstNodeRef& node, SceneNode& scene_node) {
        return scene_node.on_yaml_read(node);
    }
    bool yaml_write(YamlNodeRef node, const SceneNode& scene_node) {
        return scene_node.on_yaml_write(node);
    }

    void SceneNode::register_class() {
        auto* cls = Class::register_class<SceneNode>();
        cls->add_field(ClassField(VarType::StringId, SID("name")), &SceneNode::m_name);
    }

}// namespace wmoge