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

#include "scene_object.hpp"

#include "components/script_component.hpp"
#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "scene/scene.hpp"
#include "scene/scene_component.hpp"
#include "scene/scene_manager.hpp"

#include <cassert>

namespace wmoge {

    int SceneObject::signal(const StringId& signal) {
        // todo: better solution?
        if (auto* script = get<ScriptComponent>()) script->on_signal(signal);
        return 0;
    }
    void SceneObject::shutdown() {
        WG_AUTO_PROFILE_SCENE("SceneObject::shutdown");

        for (auto& c : m_children) {
            c->shutdown();
        }
        m_children.clear();

        if (is_in_scene()) {
            on_scene_exit();
        }
        m_components.clear();
    }
    void SceneObject::add_child(Ref<SceneObject> child) {
        assert(child);
        assert(!child->m_parent);

        if (!child) {
            WG_LOG_ERROR("an attempt to attach null child to " << get_name());
            return;
        }
        if (child->m_parent) {
            WG_LOG_ERROR("an attempt to attach child " << child->get_name() << " with parent to " << get_name());
            return;
        }

        m_children.push_back(child);
        child->m_parent = this;

        if (is_in_scene()) {
            child->m_scene         = m_scene;
            child->m_name_absolute = SID(m_name_absolute.str() + "/" + child->m_name.str());

            if (is_in_scene()) {
                child->on_scene_enter();
            }
        }
    }
    void SceneObject::add_sibling(Ref<SceneObject> sibling) {
        assert(sibling);
        assert(!sibling->m_parent);
        assert(m_parent);

        if (!sibling) {
            WG_LOG_ERROR("an attempt to attach null sibling to " << get_name());
            return;
        }
        if (sibling->m_parent) {
            WG_LOG_ERROR("an attempt to attach sibling " << sibling->get_name() << " with parent to " << get_name());
            return;
        }
        if (!m_parent) {
            WG_LOG_ERROR("an attempt to attach sibling " << sibling->get_name() << " to " << get_name() << " without parent");
            return;
        }

        m_parent->add_child(sibling);
    }
    void SceneObject::remove_child(Ref<SceneObject> child) {
        assert(child);
        assert(child->m_parent == this);

        if (!child) {
            WG_LOG_ERROR("an attempt to remove null child from " << get_name());
            return;
        }
        if (child->m_parent != this) {
            WG_LOG_ERROR("is not a child " << child->get_name() << " of parent " << get_name());
            return;
        }

        m_children.erase(std::find(m_children.begin(), m_children.end(), child));

        if (is_in_scene()) {
            child->on_scene_exit();
        }
    }
    void SceneObject::remove_children() {
        StorageChildren children;
        std::swap(children, m_children);

        if (is_in_scene()) {
            for (auto& child : children) {
                child->on_scene_exit();
            }
        }
    }

    void SceneObject::destroy() {
        Engine::instance()->scene_manager()->get_container(class_ptr())->destroy(this);
    }

    bool SceneObject::on_load_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_SCENE("SceneObject::on_load_from_yaml");

        assert(node.valid());

        if (!node.valid()) {
            WG_LOG_ERROR("passed invalid node - cannot load object");
            return false;
        }

        auto* scene_manager = Engine::instance()->scene_manager();

        if (node.has_child("components")) {
            auto components = node["components"];

            for (auto it = components.first_child(); it.valid(); it = it.next_sibling()) {
                StringId component_type = Yaml::read_sid(it["component"]);

                auto* component = get_or_create_component_base(component_type);
                if (!component) {
                    WG_LOG_ERROR("failed to create component " << component);
                    return false;
                }

                if (!component->on_load_from_yaml(it)) {
                    WG_LOG_ERROR("failed to load component from node");
                    return false;
                }
            }
        }

        if (node.has_child("children")) {
            auto children = node["children"];

            for (auto it = children.first_child(); it.valid(); it = it.next_sibling()) {
                StringId child_name = Yaml::read_sid(it["object"]);
                auto     child      = scene_manager->make_object(child_name);

                if (!child->on_load_from_yaml(it)) {
                    WG_LOG_ERROR("failed to load child " << child_name << " of " << get_name());
                    return false;
                }

                add_child(std::move(child));
            }
        }

        return true;
    }
    void SceneObject::on_scene_enter() {
        WG_AUTO_PROFILE_SCENE("SceneObject::on_scene_enter");

        get_scene()->get_registry()->add<SceneObject>(this);

        for (const auto& c : m_components) {
            c->on_scene_enter();
        }
        for (const auto& c : m_children) {
            c->m_scene         = m_scene;
            c->m_name_absolute = SID(m_name_absolute.str() + "/" + c->m_name.str());
            c->on_scene_enter();
        }

        m_is_in_scene = true;
    }
    void SceneObject::on_scene_exit() {
        WG_AUTO_PROFILE_SCENE("SceneObject::on_scene_exit");

        get_scene()->get_registry()->remove<SceneObject>(this);

        for (const auto& c : m_children) {
            c->on_scene_exit();
        }
        for (const auto& c : m_components) {
            c->on_scene_exit();
        }

        m_is_in_scene = false;
        m_scene       = nullptr;
    }
    void SceneObject::on_transform_updated() {
        WG_AUTO_PROFILE_SCENE("SceneObject::on_transform_updated");

        for (const auto& c : m_components) {
            c->on_transform_updated();
        }
        for (const auto& c : m_children) {
            c->on_transform_updated();
        }
    }

    class Scene* SceneObject::get_scene() {
        return m_scene;
    }
    class SceneObject* SceneObject::get_parent() {
        return m_parent;
    }
    class SceneObject* SceneObject::get_child(int index) {
        assert(index < m_children.size());

        if (index >= m_children.size()) {
            WG_LOG_ERROR("no child with index " << index << " in " << get_name());
            return nullptr;
        }

        return m_children[index].get();
    }
    class SceneObject* SceneObject::get_child(const StringId& name) {
        for (const auto& c : m_children) {
            if (c->get_name() == name) {
                return c.get();
            }
        }
        return nullptr;
    }
    class SceneObject* SceneObject::find_child(const StringId& name) {
        WG_AUTO_PROFILE_SCENE("SceneObject::find_child");

        for (const auto& c : m_children) {
            if (c->get_name() == name) {
                return c.get();
            }
        }
        for (const auto& c : m_children) {
            if (auto* object = c->find_child(name)) {
                return object;
            }
        }

        return nullptr;
    }

    class SceneComponent* SceneObject::get_component_base(int index) {
        assert(index < m_components.size());

        if (index >= m_components.size()) {
            WG_LOG_ERROR("no component with index " << index << " in " << get_name());
            return nullptr;
        }

        return m_components[index].get();
    }
    class SceneComponent* SceneObject::get_component_base(const StringId& class_name) {
        for (const auto& c : m_components) {
            if (c->class_name() == class_name) {
                return c.get();
            }
        }
        return nullptr;
    }
    class SceneComponent* SceneObject::get_or_create_component_base(const StringId& class_name) {
        class SceneComponent* component = get_component_base(class_name);

        if (!component) {
            Class* cls = Class::class_ptr(class_name);
            if (!cls) {
                WG_LOG_ERROR("no such class to create component " << class_name);
                return nullptr;
            }

            m_components.push_back(cls->instantiate().cast<SceneComponent>());

            component                 = m_components.back().get();
            component->m_scene_object = this;
            component->on_create();

            if (is_in_scene()) {
                component->on_scene_enter();
            }
        }

        return component;
    }

    const StringId& SceneObject::get_name() {
        return m_name;
    }
    const StringId& SceneObject::get_name_absolute() {
        return m_name_absolute;
    }
    bool SceneObject::is_in_scene() {
        return m_is_in_scene;
    }

    void SceneObject::register_class() {
        auto* cls = Class::register_class<SceneObject>();
        cls->add_property(Property(VarType::StringId, SID("name"), SID("get_name")));
        cls->add_property(Property(VarType::StringId, SID("name_absolute"), SID("get_name_absolute")));
        cls->add_method(Method(VarType::StringId, SID("get_name"), {}), &SceneObject::get_name, {});
        cls->add_method(Method(VarType::StringId, SID("get_name_absolute"), {}), &SceneObject::get_name_absolute, {});
        cls->set_instantiate([cls, engine = Engine::instance()]() { return engine->scene_manager()->get_container(cls)->create(); });
    }

}// namespace wmoge