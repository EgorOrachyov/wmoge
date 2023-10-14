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

        m_default = make_scene(SID("<default>"));
        m_running = m_default;
    }

    void SceneManager::clear() {
        for (auto& scene : m_scenes) {
            scene->clear();
        }

        m_scenes.clear();
        m_to_clear.clear();
        m_running.reset();
    }
    void SceneManager::update() {
        WG_AUTO_PROFILE_SCENE("SceneManager::update");

        if (!m_running) {
            WG_LOG_ERROR("no active scene to run, please create one");
            return;
        }

        scene_hier();
        scene_transforms();
        scene_render();
        scene_physics();
        scene_pfx();
        scene_audio();
        scene_scripting();
    }
    void SceneManager::make_active(Ref<Scene> scene) {
        std::lock_guard guard(m_mutex);

        assert(scene);
        assert(m_running);

        if (scene != m_running) {
            WG_LOG_INFO("switch scene from " << m_running->get_name() << " to " << scene->get_name());
        }

        m_running = std::move(scene);
    }
    Ref<Scene> SceneManager::get_running_scene() {
        std::lock_guard guard(m_mutex);
        return m_running;
    }
    Ref<Scene> SceneManager::make_scene(const StringId& name) {
        WG_AUTO_PROFILE_SCENE("SceneManager::make_scene");

        std::lock_guard guard(m_mutex);

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

        EcsQuery query_update_l2w_l2p;
        query_update_l2w_l2p.set_read<EcsComponentSceneTransform>();
        query_update_l2w_l2p.set_write<EcsComponentLocalToWorld>();
        query_update_l2w_l2p.set_write<EcsComponentLocalToWorld>();
        ecs_world->each(query_update_l2w_l2p, [&](EcsEntity entity) {
            const Ref<SceneTransform> transform = ecs_world->get_component<EcsComponentSceneTransform>(entity).transform;
            Mat4x4f&                  mat_l2w   = ecs_world->get_component_rw<EcsComponentLocalToWorld>(entity).matrix;
            Mat4x4f&                  mat_l2p   = ecs_world->get_component_rw<EcsComponentLocalToParent>(entity).matrix;

            mat_l2w = transform->get_l2w_cached();
            mat_l2p = transform->get_lt();
        });
    }
    void SceneManager::scene_render() {
        WG_AUTO_PROFILE_SCENE("SceneManager::scene_render");

        Engine*        engine         = Engine::instance();
        GfxCtx*        gfx_ctx        = engine->gfx_ctx();
        GfxDriver*     gfx_driver     = engine->gfx_driver();
        RenderEngine*  render_engine  = engine->render_engine();
        WindowManager* window_manager = engine->window_manager();
        Ref<Window>    window         = window_manager->primary_window();

        Scene*         scene          = m_running.get();
        EcsWorld*      ecs_world      = scene->get_ecs_world();
        CameraManager* camera_manager = scene->get_cameras();

        render_engine->begin_rendering();

        Ref<Camera>  camera      = camera_manager->find_active().value_or(camera_manager->get_default_camera());
        RenderCamera camera_prev = camera->get_render_camera();
        RenderCamera camera_curr = camera->update_render_camera(window->fbo_size());

        RenderCameras& render_cameras = render_engine->get_cameras();
        render_cameras.clear();
        render_cameras.add_camera(CameraType::Color, camera_curr, camera_prev);

        render_engine->set_time(scene->get_time());
        render_engine->set_delta_time(scene->get_delta_time());
        render_engine->set_target(window);
        render_engine->allocate_veiws();
        render_engine->prepare_frame_data();

        MeshBatchCollector& batch_collector = render_engine->get_collector();
        batch_collector.clear();

        EcsQuery query_static_mesh;
        query_static_mesh.set_read<EcsComponentLocalToWorld>();
        query_static_mesh.set_read<EcsComponentMeshStatic>();
        ecs_world->each(query_static_mesh, [&](EcsEntity entity) {
            const auto& transform = ecs_world->get_component<EcsComponentLocalToWorld>(entity).matrix;
            const auto& mesh      = ecs_world->get_component<EcsComponentMeshStatic>(entity).mesh;

            RenderCameraMask mask;
            mask.flip();

            mesh->update_transform(transform, transform);
            mesh->collect(render_cameras, mask, batch_collector);
        });

        MeshBatchCompiler& batch_compiler = render_engine->get_compiler();
        batch_compiler.clear();
        batch_compiler.set_cameras(render_cameras);
        batch_compiler.set_views(render_engine->get_views());

        for (const MeshBatch& batch : batch_collector.get_batches()) {
            batch_compiler.compile_batch(batch);
        }

        GfxDescSetResources set_resources;
        {
            {
                auto& r               = set_resources.emplace_back();
                r.first.type          = GfxBindingType::UniformBuffer;
                r.first.binding       = 0;
                r.first.array_element = 0;
                r.second.resource     = render_engine->get_frame_data().as<GfxResource>();
                r.second.offset       = 0;
                r.second.range        = render_engine->get_frame_data()->size();
            }
            {
                auto& r               = set_resources.emplace_back();
                r.first.type          = GfxBindingType::UniformBuffer;
                r.first.binding       = 1;
                r.first.array_element = 0;
                r.second.resource     = render_engine->get_views()[0].view_data.as<GfxResource>();
                r.second.offset       = 0;
                r.second.range        = render_engine->get_views()[0].view_data->size();
            }
        }
        Ref<GfxDescSet> set_common = gfx_driver->make_desc_set(set_resources, SID("common"));

        gfx_ctx->begin_render_pass({}, SID("GBuffer"));
        {
            gfx_ctx->bind_target(window);
            gfx_ctx->viewport(render_cameras.data_at(0).viewport);
            gfx_ctx->clear(0, Color4f(0, 0, 0, 0));
            gfx_ctx->clear(1.0f, 0);

            render_engine->get_views()[0].queues[int(MeshPassType::GBuffer)].execute(gfx_ctx, set_common);
        }
        gfx_ctx->end_render_pass();

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

}// namespace wmoge