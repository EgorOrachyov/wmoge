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

#include "core/engine.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    SceneManager::SceneManager() {
        WG_LOG_INFO("init scene manager");
    }
    SceneManager::~SceneManager() {
        shutdown();
    }

    void SceneManager::shutdown() {
        WG_AUTO_PROFILE_SCENE("SceneManager::shutdown");

        m_running.reset();
        m_next_running.reset();

        m_scenes.clear();
        m_to_shutdown.clear();

        WG_LOG_INFO("shutdown scene manager");
    }
    void SceneManager::next_running(Ref<Scene> scene) {
        std::lock_guard guard(m_mutex);

        assert(scene);
        assert(m_scenes.find(scene) != m_scenes.end());

        m_next_running = scene;
    }
    void SceneManager::shutdown_scene(Ref<Scene> scene) {
        std::lock_guard guard(m_mutex);

        assert(scene);
        assert(m_scenes.find(scene) != m_scenes.end());

        m_to_shutdown.insert(scene);
    }
    Ref<Scene> SceneManager::get_running_scene() {
        return m_running;
    }
    Ref<Scene> SceneManager::make_scene(const StringId& name) {
        auto scene = make_ref<Scene>();

        std::lock_guard guard(m_mutex);
        m_scenes.insert(scene);

        return scene;
    }
    void SceneManager::on_start_frame() {
        WG_AUTO_PROFILE_SCENE("SceneManager::on_start_frame");

        std::lock_guard guard(m_mutex);

        if (m_next_running) {
            m_running = std::move(m_next_running);
        }
    }
    void SceneManager::on_update() {
        WG_AUTO_PROFILE_SCENE("SceneManager::on_update");

        std::lock_guard guard(m_mutex);

        if (!m_running && !m_next_running) {
            // No active scene to process
            return;
        }
        if (!m_running) {
            // Ok, since have next to run in the next frame
            return;
        }
    }
    void SceneManager::on_debug_draw() {
        WG_AUTO_PROFILE_SCENE("SceneManager::on_debug_draw");
    }
    void SceneManager::on_end_frame() {
        WG_AUTO_PROFILE_SCENE("SceneManager::on_end_frame");

        std::lock_guard guard(m_mutex);

        if (m_to_shutdown.find(m_running) != m_to_shutdown.end()) {
            m_running.reset();
        }

        for (auto& scene : m_to_shutdown) {
            m_scenes.erase(scene);
        }

        m_to_shutdown.clear();
    }

}// namespace wmoge