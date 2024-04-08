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

#include "core/class.hpp"
#include "core/flat_map.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "resource/prefab.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_tree.hpp"

#include <cassert>
#include <filesystem>

namespace wmoge {

    WG_IO_BEGIN(SceneNodeData)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(name)
    WG_IO_FIELD(uuid)
    WG_IO_FIELD(type)
    WG_IO_FIELD_OPT(transform)
    WG_IO_FIELD_OPT(prefab)
    WG_IO_FIELD_OPT(properties)
    WG_IO_FIELD_OPT(parent)
    WG_IO_END(SceneNodeData)

    WG_IO_BEGIN(SceneNodesData)
    WG_IO_PROFILE()
    WG_IO_FIELD(nodes)
    WG_IO_END(SceneNodesData)

    void SceneNodeProp::register_class() {
        auto* cls = Class::register_class<SceneNodeProp>();
    }

    SceneNode::SceneNode(const Strid& name, wmoge::SceneNodeType type) {
        m_name = name;
        m_type = type;
    }

    Status SceneNode::build(const std::vector<SceneNodeData>& nodes_data) {
        WG_AUTO_PROFILE_SCENE("SceneNode::build");

        flat_map<UUID, Ref<SceneNode>> uuid_to_node;

        uuid_to_node.reserve(nodes_data.size());

        for (const SceneNodeData& node_data : nodes_data) {
            Ref<SceneNode> node = make_ref<SceneNode>(node_data.name, node_data.type);
            node->set_uuid(node_data.uuid);
            node->set_transform(node_data.transform);
            node->set_properties(copy_objects(node_data.properties));
            uuid_to_node[node_data.uuid] = node;
        }

        Ref<SceneNode> root(this);

        for (const SceneNodeData& node_data : nodes_data) {
            Ref<SceneNode>& node   = uuid_to_node[node_data.uuid];
            Ref<SceneNode>& parent = node_data.parent ? uuid_to_node[node_data.parent.value()] : root;
            parent->add_child(node);
        }

        return StatusCode::Ok;
    }
    Status SceneNode::dump(std::vector<SceneNodeData>& nodes_data) {
        WG_AUTO_PROFILE_SCENE("SceneNode::dump");

        std::vector<Ref<SceneNode>> nodes = get_nodes();
        flat_map<SceneNode*, UUID>  node_to_uuid;

        node_to_uuid.reserve(nodes.size());
        nodes_data.reserve(nodes.size());

        Ref<SceneNode> root(this);

        for (const Ref<SceneNode>& node : nodes) {
            const UUID node_uuid     = node->get_uuid() ? node->get_uuid() : UUID::generate();
            node_to_uuid[node.get()] = node_uuid;

            SceneNodeData& node_data = nodes_data.emplace_back();
            node_data.name           = node->get_name();
            node_data.type           = node->get_type();
            node_data.uuid           = node->get_uuid();
            node_data.transform      = node->get_transform();
            node_data.properties     = node->copy_properties();
        }

        for (std::size_t node_idx = 0; node_idx < nodes_data.size(); node_idx++) {
            const Ref<SceneNode>& node      = nodes[node_idx];
            SceneNodeData&        node_data = nodes_data[node_idx];

            if (node->has_parent() && node->get_parent() != root.get()) {
                node_data.parent = node_to_uuid[node->get_parent()];
            }
        }

        return StatusCode::Ok;
    }

    void SceneNode::enter_tree(class SceneTree* tree) {
        WG_AUTO_PROFILE_SCENE("SceneNode::enter_tree");

        assert(tree);
        assert(!m_tree);

        m_tree   = tree;
        m_entity = instantiate_entity(m_tree->get_scene().get(), has_parent() ? m_parent->get_entity() : Entity());

        for (auto& child : m_children) {
            child->enter_tree(tree);
        }
    }
    void SceneNode::exit_tree() {
        WG_AUTO_PROFILE_SCENE("SceneNode::exit_tree");

        assert(m_tree);

        for (auto& child : m_children) {
            child->exit_tree();
        }

        if (m_entity.is_valid()) {
            m_tree->get_scene()->destroy_entity(m_entity);
            m_entity.reset();
        }

        m_tree = nullptr;
    }

    void SceneNode::process_event(const EventSceneNode& event) {
        WG_AUTO_PROFILE_SCENE("SceneNode::process_event");

        if (event.notification == SceneNodeNotification::TransformUpdated) {
            m_l2w = m_transform.to_mat4x4();
            m_w2l = m_transform.to_inv_mat4x4();
            if (has_parent()) {
                m_l2w = get_parent()->get_l2w() * m_l2w;
                m_w2l = m_w2l * get_parent()->get_w2l();
            }
        }

        if (has_tree()) {
            dispatch_to_props(event);
        }

        dispatch_to_children(event);
    }

    void SceneNode::dispatch_to_props(const EventSceneNode& event) {
        for (Ref<SceneNodeProp>& prop : m_properties) {
            prop->process_event(event);
        }
    }

    void SceneNode::dispatch_to_children(const EventSceneNode& event) {
        for (Ref<SceneNode>& child : m_children) {
            child->process_event(event);
        }
    }

    void SceneNode::set_name(const Strid& name) {
        m_name = name;
    }
    void SceneNode::set_uuid(const UUID& uuid) {
        m_uuid = uuid;
    }
    void SceneNode::set_transform(const TransformEdt& transform) {
        m_transform = transform;
    }
    void SceneNode::set_properties(std::vector<Ref<SceneNodeProp>> props) {
        m_properties = std::move(props);

        for (auto& prop : m_properties) {
            prop->set_node(this);
        }
    }

    void SceneNode::add_child(const Ref<SceneNode>& child) {
        WG_AUTO_PROFILE_SCENE("SceneNode::add_child");

        assert(child);
        assert(child->m_parent == nullptr);
        assert(child->m_tree == nullptr);

        m_children.push_back(child);
        child->m_parent = this;
        child->m_path   = m_path + '/' + child->m_name.str();

        if (has_tree()) {
            child->enter_tree(m_tree);
        }

        EventSceneNode event;

        event.notification = SceneNodeNotification::TransformUpdated;
        child->process_event(event);
    }
    void SceneNode::remove_child(const Ref<SceneNode>& child) {
        WG_AUTO_PROFILE_SCENE("SceneNode::remove_child");

        assert(child);
        assert(child->m_parent == this);
        assert(child->m_tree == m_tree);

        if (has_tree()) {
            child->exit_tree();
        }

        child->m_parent = nullptr;
        child->m_path.clear();
        m_children.erase(std::find(m_children.begin(), m_children.end(), child));
    }
    bool SceneNode::is_child(const Ref<SceneNode>& node) const {
        return std::find(m_children.begin(), m_children.end(), node) != m_children.end();
    }
    bool SceneNode::contains(const Ref<SceneNode>& node) const {
        if (node.get() == this) {
            return true;
        }
        for (const Ref<SceneNode>& child : m_children) {
            if (child->contains(node)) {
                return true;
            }
        }
        return false;
    }
    void SceneNode::each(const std::function<void(const Ref<SceneNode>&)>& visitor) {
        for (const Ref<SceneNode>& child : m_children) {
            visitor(child);
            child->each(visitor);
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

    std::vector<Ref<SceneNodeProp>> SceneNode::copy_properties() const {
        std::vector<Ref<SceneNodeProp>> properties;

        if (!copy_objects(m_properties, properties)) {
            WG_LOG_ERROR("failed to copy node properties " << get_name());
            return {};
        }

        return std::move(properties);
    }

    std::vector<Ref<SceneNode>> SceneNode::get_nodes() {
        std::vector<Ref<SceneNode>> nodes;

        each([&](const Ref<SceneNode>& node) {
            nodes.push_back(node);
        });

        return std::move(nodes);
    }

    bool SceneNode::has_parent() const {
        return m_parent;
    }
    bool SceneNode::has_prefab() const {
        return m_prefab;
    }
    bool SceneNode::has_entity() const {
        return m_entity.is_valid();
    }
    bool SceneNode::has_tree() const {
        return m_tree;
    }
    bool SceneNode::has_scene() const {
        return m_tree && m_tree->get_scene();
    }

    Entity SceneNode::instantiate_entity(class Scene* scene, Entity parent) {
        WG_AUTO_PROFILE_SCENE("SceneNode::instantiate_entity");

        EcsArch arch;
        for (auto& prop : m_properties) {
            prop->fill_arch(arch);
        }

        EcsWorld* esc_world  = scene->get_ecs_world();
        EcsEntity ecs_entity = esc_world->allocate_entity();
        esc_world->make_entity(ecs_entity, arch);

        Entity entity(ecs_entity, scene);
        for (auto& prop : m_properties) {
            prop->add_components(entity, parent);
        }

        return entity;
    }

    Scene* SceneNode::get_scene() const {
        return m_tree ? m_tree->get_scene().get() : nullptr;
    }

    Status SceneNode::copy_to(Object& other) const {
        auto* ptr = dynamic_cast<SceneNode*>(&other);

        ptr->m_name      = m_name;
        ptr->m_uuid      = UUID::generate();
        ptr->m_type      = m_type;
        ptr->m_transform = m_transform;
        ptr->set_properties(copy_properties());

        std::vector<Ref<SceneNode>> children = copy_objects(m_children);
        for (auto& child : children) {
            ptr->add_child(child);
        }

        return StatusCode::Ok;
    }

    void SceneNode::register_class() {
        auto* cls = Class::register_class<SceneNode>();
        cls->add_field(ClassField(VarType::Strid, SID("name")), &SceneNode::m_name);
        cls->add_field(ClassField(VarType::String, SID("path")), &SceneNode::m_path);
    }

}// namespace wmoge