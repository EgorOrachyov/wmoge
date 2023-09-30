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

#include <cassert>
#include <filesystem>

namespace wmoge {

    SceneNode::SceneNode(const StringId& name, wmoge::SceneNodeType type) {
        m_name = name;
        m_type = type;
    }

    void SceneNode::set_name(const StringId& name) {
        m_name = name;
    }
    void SceneNode::set_uuid(const UUID& uuid) {
        m_uuid = uuid;
    }
    void SceneNode::set_transform(const TransformEdt& transform) {
        m_transform = transform;
    }
    void SceneNode::set_properties(std::vector<Ref<SceneProperty>> props) {
        m_properties = std::move(props);
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
    void SceneNode::visit(const std::function<void(const Ref<SceneNode>& node)>& visitor) {
        for (const Ref<SceneNode>& child : m_children) {
            visitor(child);
            child->visit(visitor);
        }
    }

    std::optional<Ref<SceneNode>> SceneNode::find_child(const std::string& name) {
        for (const auto& child : m_children) {
            if (child->get_name().str() == name) {
                return child;
            }
        }

        return std::nullopt;
    }
    std::optional<Ref<SceneNode>> SceneNode::find_child_recursive(const std::string& path) {
        std::filesystem::path parsed_path(path);

        SceneNode*                    current = this;
        std::optional<Ref<SceneNode>> found;

        for (const auto& sub_element : parsed_path) {
            found = current->find_child(sub_element.string());

            if (!found) {
                return std::nullopt;
            }
        }

        return found;
    }

    std::vector<Ref<SceneProperty>> SceneNode::copy_properties() const {
        std::vector<Ref<SceneProperty>> properties;

        if (!copy_objects(m_properties, properties)) {
            WG_LOG_ERROR("failed to copy node properties " << get_name());
            return {};
        }

        return properties;
    }

    std::string SceneNode::get_path() const {
        return m_parent ? m_parent->get_path() + '/' + m_name.str() : m_name.str();
    }
    bool SceneNode::has_parent() const {
        return m_parent;
    }

    Status SceneNode::copy_to(Object& other) const {
        auto* ptr = dynamic_cast<SceneNode*>(&other);

        ptr->m_name      = m_name;
        ptr->m_uuid      = m_uuid;
        ptr->m_type      = m_type;
        ptr->m_transform = m_transform;

        if (!copy_objects(m_properties, ptr->m_properties)) {
            WG_LOG_ERROR("failed to clone node properties " << get_name());
            return StatusCode::Error;
        }

        if (!copy_objects(m_children, ptr->m_children)) {
            WG_LOG_ERROR("failed to clone node children " << get_name());
            return StatusCode::Error;
        }

        for (auto& child : ptr->m_children) {
            child->m_parent = ptr;
        }

        return StatusCode::Ok;
    }

    void SceneNode::register_class() {
        auto* cls = Class::register_class<SceneNode>();
        cls->add_field(ClassField(VarType::StringId, SID("name")), &SceneNode::m_name);
    }

}// namespace wmoge