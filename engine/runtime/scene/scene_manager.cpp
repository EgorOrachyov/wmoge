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

#include "core/async.hpp"
#include "core/task_parallel_for.hpp"
#include "ecs/ecs_registry.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "platform/time.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler.hpp"
#include "render/render_engine.hpp"
#include "scene/scene_components.hpp"
#include "system/ioc_container.hpp"

#include <cassert>
#include <utility>

namespace wmoge {

    SceneManager::SceneManager() {
        WG_LOG_INFO("init scene manager");

        m_render_engine = IocContainer::iresolve_v<RenderEngine>();
        m_ecs_registry  = IocContainer::iresolve_v<EcsRegistry>();

        m_ecs_registry->register_component<EcsComponentChildren>();
        m_ecs_registry->register_component<EcsComponentParent>();
        m_ecs_registry->register_component<EcsComponentTransform>();
        m_ecs_registry->register_component<EcsComponentTransformUpd>();
        m_ecs_registry->register_component<EcsComponentLocalToWorld>();
        m_ecs_registry->register_component<EcsComponentWorldToLocal>();
        m_ecs_registry->register_component<EcsComponentLocalToParent>();
        m_ecs_registry->register_component<EcsComponentAabbLocal>();
        m_ecs_registry->register_component<EcsComponentAabbWorld>();
        m_ecs_registry->register_component<EcsComponentName>();
        m_ecs_registry->register_component<EcsComponentTag>();
        m_ecs_registry->register_component<EcsComponentCamera>();
        m_ecs_registry->register_component<EcsComponentLight>();
        m_ecs_registry->register_component<EcsComponentCullingItem>();
    }

    void SceneManager::clear() {
        WG_AUTO_PROFILE_SCENE("SceneManager::clear");

        for (auto& scene : m_scenes) {
            scene->finalize();
        }

        m_scenes.clear();
        m_to_clear.clear();
        m_running.reset();
    }
    void SceneManager::update() {
        WG_AUTO_PROFILE_SCENE("SceneManager::update");

        // Process change before start of update
        scene_change();

        if (!m_running) {
            return;
        }

        if (m_running->get_state() != SceneState::Playing) {
            WG_LOG_ERROR("active scene must be in a playing state");
            return;
        }

        // Play scene
        scene_play();
    }
    void SceneManager::change(Ref<Scene> scene) {
        assert(scene);

        m_next = std::move(scene);
    }
    Ref<Scene> SceneManager::get_running_scene() {
        return m_running;
    }
    Ref<Scene> SceneManager::make_scene(const Strid& name) {
        WG_AUTO_PROFILE_SCENE("SceneManager::make_scene");

        return m_scenes.emplace_back(make_ref<Scene>(name));
    }
    std::optional<Ref<Scene>> SceneManager::find_by_name(const Strid& name) {
        WG_AUTO_PROFILE_SCENE("SceneManager::find_by_name");

        for (Ref<Scene>& scene : m_scenes) {
            if (scene->get_name() == name) {
                return scene;
            }
        }

        return std::nullopt;
    }

    void SceneManager::update_scene_hier() {
        WG_AUTO_PROFILE_SCENE("SceneManager::update_scene_hier");
    }

    void SceneManager::update_scene_cameras() {
        WG_AUTO_PROFILE_SCENE("SceneManager::update_scene_cameras");
    }

    void SceneManager::update_scene_visibility() {
        WG_AUTO_PROFILE_SCENE("SceneManager::update_scene_visibility");
    }

    void SceneManager::render_scene() {
        WG_AUTO_PROFILE_SCENE("SceneManager::render_scene");
    }

    void SceneManager::scene_change() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_change");

        if (m_next) {
            if (m_running) {
                assert(m_running->get_state() == SceneState::Playing);
                scene_pause();
            }

            m_running = std::move(m_next);

            if (m_running->get_state() == SceneState::Default) {
                scene_start();
            }
            if (m_running->get_state() == SceneState::Paused) {
                scene_resume();
            }

            m_next.reset();
        }
    }

    void SceneManager::scene_start() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_start");

        m_running->set_state(SceneState::Playing);
    }

    void SceneManager::scene_play() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_play");

        // m_running->advance(... instance()->time()->get_delta_time_game());

        m_sync = SyncContext();

        update_scene_hier();
        update_scene_cameras();
        update_scene_visibility();
        render_scene();

        m_sync.await_all();

        assert(m_running->get_state() == SceneState::Playing);
    }

    void SceneManager::scene_pause() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_pause");

        m_running->set_state(SceneState::Paused);
    }

    void SceneManager::scene_resume() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_resume");

        m_running->set_state(SceneState::Playing);
    }

    void SceneManager::scene_finish() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_finish");

        m_running->set_state(SceneState::Finished);
    }

    void SceneManager::SyncContext::await_all() {
        WG_AUTO_PROFILE_SCENE("SyncContext::await_all");

        if (complete_heir.is_not_null()) complete_heir.wait_completed();
        if (complete_cameras.is_not_null()) complete_cameras.wait_completed();
        if (complete_visibility.is_not_null()) complete_visibility.wait_completed();
        if (complete_render.is_not_null()) complete_render.wait_completed();
    }

}// namespace wmoge