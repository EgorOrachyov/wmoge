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

#ifndef WMOGE_SCENE_OBJECT_HPP
#define WMOGE_SCENE_OBJECT_HPP

#include "core/class.hpp"
#include "core/fast_vector.hpp"
#include "core/object.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "io/yaml.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"

#include <array>
#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class SceneObject
     * @brief Is a base object which can be placed inside a scene
     *
     * Scene objects form a scene tree hierarchy in a game. Scene objects has a
     * parent-child relation ship. Each object may have its own local transform
     * and global transform based on parent global transform.
     *
     * Scene object can be rotated, scaled and moved. All actions will affect
     * direct and indirect children in a sub-tree of this object.
     *
     * Object has a collection of unique components. Components define a reusable
     * state and logic, which can be used to compose game objects of any complexity.
     * Components can be used for audio, collision, 2d and 3d drawing, etc.
     *
     * Scene object extends base engine object and allows to a script to be
     * attached to any object on a scene. Script is instanced, so every object
     * has a unique state associated with that.
     *
     * @see Scene
     * @see SceneComponent
     * @see SceneManager
     */
    class SceneObject final : public Object {
    public:
        WG_OBJECT(SceneObject, Object)

        int signal(const StringId& signal) override;

        void               add_child(Ref<SceneObject> child);
        void               add_sibling(Ref<SceneObject> sibling);
        void               remove_child(Ref<SceneObject> child);
        void               remove_children();
        class Scene*       get_scene();
        class SceneObject* get_parent();
        class SceneObject* get_child(int index);
        class SceneObject* get_child(const StringId& name);
        class SceneObject* find_child(const StringId& name);
        const StringId&    get_name();
        const StringId&    get_name_absolute();
        bool               is_in_scene();

        class SceneComponent* get_component_base(int index);
        class SceneComponent* get_component_base(const StringId& class_name);
        class SceneComponent* get_or_create_component_base(const StringId& class_name);

        template<class ClassType>
        ClassType* get(int index);
        template<class ClassType>
        ClassType* get();
        template<class ClassType>
        ClassType* get_or_create();

    protected:
        friend class Scene;
        friend class ScenePacked;
        friend class SceneManager;
        friend class Spatial2d;

        bool on_load_from_yaml(const YamlConstNodeRef& node);
        void on_scene_enter();
        void on_scene_exit();
        void on_transform_updated();
        void shutdown();
        void destroy() override;

    private:
        using StorageComponents = fast_vector<Ref<class SceneComponent>, 4>;
        using StorageChildren   = fast_vector<Ref<class SceneObject>, 2>;

        StorageComponents  m_components;
        StorageChildren    m_children;
        class SceneObject* m_parent = nullptr;
        class Scene*       m_scene  = nullptr;
        StringId           m_name;
        StringId           m_name_absolute;
        bool               m_is_in_scene = false;
    };

    static_assert(sizeof(SceneObject) <= 128, "scene object too large");

    template<class ClassType>
    ClassType* SceneObject::get(int index) {
        return dynamic_cast<ClassType*>(get_component_base(index));
    }

    template<class ClassType>
    ClassType* SceneObject::get() {
        return dynamic_cast<ClassType*>(get_component_base(ClassType::class_name_static()));
    }

    template<class ClassType>
    ClassType* SceneObject::get_or_create() {
        return dynamic_cast<ClassType*>(get_or_create_component_base(ClassType::class_name_static()));
    }

}// namespace wmoge

#endif//WMOGE_SCENE_OBJECT_HPP
