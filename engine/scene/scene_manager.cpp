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
#include "core/task_parallel_for.hpp"
#include "debug/profiler.hpp"
#include "ecs/ecs_registry.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"
#include "render/render_engine.hpp"
#include "scene/scene_components.hpp"
#include "systems/system_render.hpp"
#include "systems/system_transform.hpp"

#include <cassert>
#include <utility>

namespace wmoge {

    SceneManager::SceneManager() {
        WG_LOG_INFO("init scene manager");

        EcsRegistry* ecs_registry = Engine::instance()->ecs_registry();

        ecs_registry->register_component<EcsComponentChildren>();
        ecs_registry->register_component<EcsComponentParent>();
        ecs_registry->register_component<EcsComponentSceneTransform>();
        ecs_registry->register_component<EcsComponentLocalToWorld>();
        ecs_registry->register_component<EcsComponentLocalToParent>();
        ecs_registry->register_component<EcsComponentName>();
        ecs_registry->register_component<EcsComponentTag>();
        ecs_registry->register_component<EcsComponentCamera>();
        ecs_registry->register_component<EcsComponentLight>();
        ecs_registry->register_component<EcsComponentMeshStatic>();
    }

    void SceneManager::clear() {
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
            WG_LOG_ERROR("no active scene to run, please create one");
            return;
        }

        if (m_running->get_state() != SceneState::Playing) {
            WG_LOG_ERROR("active scene must be in a playing state");
            return;
        }

        // Play scene
        scene_play();

        // If changed after play, updated active scene
        scene_change();
    }
    void SceneManager::change(Ref<Scene> scene) {
        assert(scene);

        m_next = std::move(scene);
    }
    Ref<Scene> SceneManager::get_running_scene() {
        return m_running;
    }
    Ref<Scene> SceneManager::make_scene(const StringId& name) {
        WG_AUTO_PROFILE_SCENE("SceneManager::make_scene");

        auto scene = make_ref<Scene>(name);
        m_scenes.push_back(scene);

        return scene;
    }
    std::optional<Ref<Scene>> SceneManager::find_by_name(const StringId& name) {
        WG_AUTO_PROFILE_SCENE("SceneManager::find_by_name");

        for (Ref<Scene>& scene : m_scenes) {
            if (scene->get_name() == name) {
                return scene;
            }
        }

        return std::nullopt;
    }

    void SceneManager::scene_hier() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_hier");

        Scene*                 scene             = m_running.get();
        SceneTransformManager* transform_manager = scene->get_transforms();

        const int num_of_layers = transform_manager->get_num_layers();

        for (int layer_idx = 0; layer_idx < num_of_layers; layer_idx++) {
            ArrayView<SceneTransform*> layer      = transform_manager->get_layer(layer_idx);
            const int                  layer_size = int(layer.size());
            const int                  batch_size = 16;

            if (!layer.empty()) {
                const StringId  task_name = SID("transform_" + StringUtils::from_int(layer_idx));
                TaskParallelFor task(task_name, [&](TaskContext&, int id, int) {
                    layer[id]->update(false);
                    return 0;
                });

                task.schedule(layer_size, batch_size).wait_completed();
            }
        }
    }

    void SceneManager::scene_transforms() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_transforms");

        Scene*    scene     = m_running.get();
        EcsWorld* ecs_world = scene->get_ecs_world();

        EcsSysCacheMatrices sys_cache_matrices;
        ecs_world->execute_system(sys_cache_matrices);

        EcsSysUpdateStaticMeshes sys_update_static_meshes;
        sys_update_static_meshes.render_scene = scene->get_render_scene();
        sys_update_static_meshes.vis_system   = scene->get_visibility_system();
        ecs_world->execute_system(sys_update_static_meshes);
    }

    void SceneManager::scene_cameras() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_cameras");

        Engine*       engine        = Engine::instance();
        RenderEngine* render_engine = engine->render_engine();

        Scene*         scene          = m_running.get();
        CameraManager* camera_manager = scene->get_cameras();
        RenderCameras& render_cameras = render_engine->get_cameras();

        camera_manager->fill_render_cameras(render_cameras);
    }

    void SceneManager::scene_visibility() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_visibility");

        Engine*       engine        = Engine::instance();
        RenderEngine* render_engine = engine->render_engine();

        Scene*               scene          = m_running.get();
        EcsWorld*            ecs_world      = scene->get_ecs_world();
        VisibilitySystem*    vis_system     = scene->get_visibility_system();
        const RenderCameras& render_cameras = render_engine->get_cameras();

        vis_system->cull(render_cameras);

        EcsSysPocessVisStaticMeshes sys_process_vis_static_meshes;
        sys_process_vis_static_meshes.render_scene = scene->get_render_scene();
        sys_process_vis_static_meshes.vis_system   = scene->get_visibility_system();
        ecs_world->execute_system(sys_process_vis_static_meshes);
    }

    void SceneManager::scene_render() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_render");

        Engine*        engine         = Engine::instance();
        GfxCtx*        gfx_ctx        = engine->gfx_ctx();
        RenderEngine*  render_engine  = engine->render_engine();
        WindowManager* window_manager = engine->window_manager();
        Ref<Window>    window         = window_manager->primary_window();

        Scene*            scene             = m_running.get();
        GraphicsPipeline* graphics_pipeline = scene->get_graphics_pipeline();

        render_engine->begin_rendering();

        render_engine->set_time(scene->get_time());
        render_engine->set_delta_time(scene->get_delta_time());
        render_engine->set_target(window);
        render_engine->set_scene(scene->get_render_scene());
        render_engine->set_visiblity(scene->get_visibility_system());

        render_engine->prepare_frame_data();
        render_engine->allocate_veiws();
        render_engine->collect_batches();
        render_engine->compile_batches();
        render_engine->group_queues();
        render_engine->sort_queues();
        render_engine->merge_cmds();
        render_engine->flush_buffers();

        graphics_pipeline->set_scene(scene->get_render_scene());
        graphics_pipeline->set_cameras(&render_engine->get_cameras());
        graphics_pipeline->set_views(render_engine->get_views());
        graphics_pipeline->exectute();

        render_engine->end_rendering();
    }

    void SceneManager::scene_pfx() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_pfx");
    }

    void SceneManager::scene_scripting() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_scripting");
    }

    void SceneManager::scene_physics() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_physics");
    }

    void SceneManager::scene_audio() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_audio");
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
        m_running->get_graphics_pipeline()->init();
    }

    void SceneManager::scene_play() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_play");

        m_running->advance(Engine::instance()->get_delta_time_game());

        scene_hier();
        scene_transforms();
        scene_cameras();
        scene_visibility();
        scene_render();
        scene_pfx();
        scene_scripting();
        scene_physics();
        scene_audio();

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

}// namespace wmoge