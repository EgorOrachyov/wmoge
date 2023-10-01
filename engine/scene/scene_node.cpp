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
#include "core/log.hpp"
#include "resource/prefab.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_tree.hpp"

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
    void SceneNode::set_tree(SceneTree* tree) {
        m_tree = tree;
    }

    void SceneNode::add_child(const Ref<SceneNode>& child) {
        assert(child);
        assert(child->m_parent == nullptr);

        m_children.push_back(child);
        child->m_parent = this;
        child->m_tree   = m_tree;
        child->m_path   = m_path + '/' + child->m_name.str();
    }
    void SceneNode::remove_child(const Ref<SceneNode>& child) {
        assert(child);
        assert(child->m_parent == this);

        child->m_parent = nullptr;
        child->m_tree   = nullptr;
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

    std::vector<Ref<SceneProperty>> SceneNode::copy_properties() const {
        std::vector<Ref<SceneProperty>> properties;

        if (!copy_objects(m_properties, properties)) {
            WG_LOG_ERROR("failed to copy node properties " << get_name());
            return {};
        }

        return properties;
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

    std::optional<Ref<SceneTransform>> SceneNode::get_hier_transform() const {
        if (has_entity()) {
            assert(has_scene());
            Scene*    scene = get_scene();
            EcsWorld* world = scene->get_ecs_world();
            return world->get_component<EcsComponentSceneTransform>(m_entity).transform;
        }
        return std::nullopt;
    }
    std::optional<Ref<SceneTransform>> SceneNode::get_hier_transform_parent() const {
        if (has_parent()) {
            return m_parent->get_hier_transform();
        }
        return std::nullopt;
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

    void SceneNode::make_entity() {
        assert(m_entity.is_invalid());
        assert(has_tree());
        assert(has_scene());

        Scene*    scene = get_scene();
        EcsWorld* world = scene->get_ecs_world();
        EcsArch   arch;

        arch.set_component<EcsComponentName>();
        arch.set_component<EcsComponentSceneTransform>();
        arch.set_component<EcsComponentLocalToWorld>();
        arch.set_component<EcsComponentLocalToParent>();

        for (auto& prop : m_properties) {
            prop->collect_arch(arch, *this);
        }

        m_entity = world->allocate_entity();
        world->make_entity(m_entity, arch);

        sync_name();
        sync_transform();

        for (auto& prop : m_properties) {
            prop->on_make_entity(m_entity, *this);
        }
    }
    void SceneNode::delete_entity() {
        assert(m_entity.is_valid());
        assert(has_tree());
        assert(has_scene());

        Scene*    scene = get_scene();
        EcsWorld* world = scene->get_ecs_world();

        for (auto& prop : m_properties) {
            prop->on_delete_entity(m_entity, *this);
        }

        world->destroy_entity(m_entity);
        m_entity = EcsEntity{};
    }
    void SceneNode::remake_entity() {
        if (has_entity()) {
            delete_entity();
        }
        make_entity();
    }
    void SceneNode::sync_name() {
        assert(has_entity());
        assert(has_scene());

        Scene*    scene = get_scene();
        EcsWorld* world = scene->get_ecs_world();

        world->get_component_rw<EcsComponentName>(m_entity).name = get_path();
    }
    void SceneNode::sync_transform() {
        assert(has_entity());
        assert(has_scene());

        Scene*    scene = get_scene();
        EcsWorld* world = scene->get_ecs_world();

        // Hierarchy plus matrices for other systems
        EcsComponentSceneTransform& ecs_transform = world->get_component_rw<EcsComponentSceneTransform>(m_entity);
        EcsComponentLocalToParent&  ecs_l2p       = world->get_component_rw<EcsComponentLocalToParent>(m_entity);
        EcsComponentLocalToWorld&   ecs_l2w       = world->get_component_rw<EcsComponentLocalToWorld>(m_entity);

        // If no hier transfrom, create new and link
        if (!ecs_transform.transform) {
            ecs_transform.transform    = make_ref<SceneTransform>(scene->get_transforms());
            auto parent_hier_transfrom = get_hier_transform_parent();
            // join hierarchy
            if (parent_hier_transfrom.has_value()) {
                parent_hier_transfrom.value()->add_child(ecs_transform.transform);
            }
            // if not linked we are first here, link as a root
            if (!ecs_transform.transform->is_linked()) {
                ecs_transform.transform->set_layer(0);
            }
        }

        // Update transfrom from node local edt params
        const TransformEdt& t = get_transform();
        ecs_transform.transform->set_lt(t.get_transform(), t.get_inverse_transform());
        ecs_transform.transform->update(true);
        ecs_l2w.matrix = ecs_transform.transform->get_l2w_cached();
        ecs_l2p.matrix = ecs_transform.transform->get_lt();
    }

    void SceneNode::register_class() {
        auto* cls = Class::register_class<SceneNode>();
        cls->add_field(ClassField(VarType::StringId, SID("name")), &SceneNode::m_name);
        cls->add_field(ClassField(VarType::String, SID("path")), &SceneNode::m_path);
    }

}// namespace wmoge