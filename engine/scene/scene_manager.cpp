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

namespace wmoge {

    SceneManager::SceneManager() {
        WG_LOG_INFO("init scene manager");
    }

    void SceneManager::next(Ref<Scene> scene) {
        std::lock_guard guard(m_mutex);

        m_next = std::move(scene);
    }
    void SceneManager::unload(const Ref<Scene>& scene) {
        std::lock_guard guard(m_mutex);

        m_to_unload.erase(scene);
        m_scenes.erase(scene);
    }
    void SceneManager::unload_deferred(const Ref<Scene>& scene) {
        std::lock_guard guard(m_mutex);

        m_to_unload.insert(scene);
    }
    void SceneManager::unload_all() {
        std::lock_guard guard(m_mutex);

        m_scenes.clear();
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

        auto scene = make_ref<Scene>();
        m_scenes.insert(scene);

        return scene;
    }

}// namespace wmoge