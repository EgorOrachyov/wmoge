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

#include "scene.hpp"

#include "profiler/profiler_cpu.hpp"
#include "render/deferred_pipeline.hpp"
#include "scene/scene_components.hpp"

#include <cassert>

namespace wmoge {

    Scene::Scene(const SceneCreateInfo& info) {
        WG_PROFILE_CPU_SCENE("Scene::Scene");

        m_name            = info.name;
        m_ecs_world       = std::make_unique<EcsWorld>(info.ecs_registry, info.task_manager);
        m_culling_manager = std::make_unique<CullingManager>();
        m_render_scene    = std::make_unique<RenderScene>();
    }

    Status Scene::build(const SceneData& data) {
        WG_PROFILE_CPU_SCENE("Scene::build");

        return WG_OK;
    }

    void Scene::advance(float delta_time) {
        m_delta_time = delta_time;
        m_time += m_delta_time;
        m_frame_id += 1;
    }
    void Scene::clear() {
        m_ecs_world->clear();
    }
    void Scene::set_state(SceneState state) {
        m_state = state;
    }
    void Scene::finalize() {
        WG_PROFILE_CPU_SCENE("Scene::finalize");

        m_ecs_world.reset();
        m_culling_manager.reset();
        m_render_scene.reset();
    }
    const Strid& Scene::get_name() {
        return m_name;
    }
    EcsWorld* Scene::get_ecs_world() {
        return m_ecs_world.get();
    }
    CullingManager* Scene::get_culling_manager() {
        return m_culling_manager.get();
    }
    RenderScene* Scene::get_render_scene() {
        return m_render_scene.get();
    }

}// namespace wmoge