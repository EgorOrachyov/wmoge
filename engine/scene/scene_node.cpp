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

    Status SceneNode::read_from_yaml(const YamlConstNodeRef& node) {
        WG_YAML_READ_AS_OPT(node, "uuid", m_uuid);
        WG_YAML_READ_AS_OPT(node, "name", m_name);
        WG_YAML_READ_AS_OPT(node, "children", m_children);

        if (!m_uuid) {
            m_uuid = UUID::generate();
        }

        if (m_name.empty()) {
            m_name = SID(class_ptr()->name().str() + "_" + m_uuid.to_str());
        }

        for (auto& child : m_children) {
            child->m_parent = this;
        }

        return StatusCode::Ok;
    }
    Status SceneNode::write_to_yaml(YamlNodeRef node) const {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "class", class_ptr()->name());
        WG_YAML_WRITE_AS(node, "uuid", m_uuid);
        WG_YAML_WRITE_AS(node, "name", m_name);
        WG_YAML_WRITE_AS_OPT(node, "children", !m_children.empty(), m_children);

        return StatusCode::Ok;
    }
    Status SceneNode::accept_visitor(class SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    Status SceneNode::copy_to(Object& other) const {
        auto* ptr = dynamic_cast<SceneNode*>(&other);

        ptr->m_name = m_name;
        ptr->m_uuid = m_uuid;

        ptr->m_children.reserve(m_children.size());

        for (auto& child : m_children) {
            Ref<Object> child_copy;

            if (!child->clone(child_copy)) {
                WG_LOG_ERROR("failed to clone node child " << child->get_name());
                return StatusCode::Error;
            }

            auto as_node = child_copy.cast<SceneNode>();
            assert(as_node);

            as_node->m_parent = ptr;
            ptr->m_children.push_back(as_node);
        }

        return StatusCode::Ok;
    }

    void SceneNode::register_class() {
        auto* cls = Class::register_class<SceneNode>();
        cls->add_field(ClassField(VarType::StringId, SID("name")), &SceneNode::m_name);
    }

}// namespace wmoge