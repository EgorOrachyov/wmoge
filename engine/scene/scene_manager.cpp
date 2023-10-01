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

#include "scene_manager.hpp"

#include <utility>

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "ecs/ecs_registry.hpp"
#include "scene/scene_components.hpp"

namespace wmoge {

    SceneManager::SceneManager() {
        WG_LOG_INFO("init scene manager");

        auto* ecs_registry = Engine::instance()->ecs_registry();

        ecs_registry->register_component<EcsComponentChildren>();
        ecs_registry->register_component<EcsComponentParent>();
        ecs_registry->register_component<EcsComponentSceneTransform>();
        ecs_registry->register_component<EcsComponentLocalToWorld>();
        ecs_registry->register_component<EcsComponentLocalToParent>();
        ecs_registry->register_component<EcsComponentName>();
        ecs_registry->register_component<EcsComponentTag>();
        ecs_registry->register_component<EcsComponentCamera>();
    }

    void SceneManager::clear() {
        for (auto& scene : m_scenes) {
            scene->clear();
        }

        m_scenes.clear();
        m_to_clear.clear();
        m_next.reset();
        m_running.reset();
    }

    void SceneManager::next(Ref<Scene> scene) {
        std::lock_guard guard(m_mutex);

        m_next = std::move(scene);
    }

    Ref<Scene> SceneManager::get_running_scene() {
        std::lock_guard guard(m_mutex);
        return m_running;
    }
    Ref<Scene> SceneManager::get_next_scene() {
        std::lock_guard guard(m_mutex);
        return m_next;
    }
    Ref<Scene> SceneManager::make_scene(const StringId& name) {
        std::lock_guard guard(m_mutex);

        auto scene = make_ref<Scene>(name);
        m_scenes.push_back(scene);

        return scene;
    }

    std::optional<Ref<Scene>> SceneManager::find_by_name(const StringId& name) {
        for (Ref<Scene>& scene : m_scenes) {
            if (scene->get_name() == name) {
                return scene;
            }
        }

        return std::nullopt;
    }

}// namespace wmoge