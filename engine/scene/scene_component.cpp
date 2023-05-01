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

#include "scene_component.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "scene/scene.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene_object.hpp"

namespace wmoge {

    void SceneComponent::on_scene_enter() {
        const Class*   object   = Object::class_ptr_static();
        Scene*         scene    = get_scene();
        SceneRegistry* registry = scene->get_registry();

        for (const Class* it = class_ptr(); it != object; it = it->super()) {
            registry->get_container(it)->add(this);
        }
    }
    void SceneComponent::on_scene_exit() {
        const Class*   object   = Object::class_ptr_static();
        Scene*         scene    = get_scene();
        SceneRegistry* registry = scene->get_registry();

        for (const Class* it = class_ptr(); it != object; it = it->super()) {
            registry->get_container(it)->remove(this);
        }
    }

    class Scene* SceneComponent::get_scene() {
        return get_scene_object()->get_scene();
    }
    class SceneObject* SceneComponent::get_scene_object() {
        return m_scene_object;
    }
    bool SceneComponent::is_in_scene() const {
        return m_scene_object->is_in_scene();
    }

    void SceneComponent::register_class() {
        auto* cls = Class::register_class<SceneComponent>();
    }
    void SceneComponent::destroy() {
        Engine::instance()->scene_manager()->get_container(class_ptr())->destroy(this);
    }

}// namespace wmoge