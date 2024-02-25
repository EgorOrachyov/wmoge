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

#include "render/deferred_pipeline.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_entity.hpp"

#include <cassert>

namespace wmoge {

    Scene::Scene(StringId name) {
        m_name              = name;
        m_ecs_world         = std::make_unique<EcsWorld>();
        m_visibility_system = std::make_unique<VisibilitySystem>();
        m_render_scene      = std::make_unique<RenderScene>();

        m_ecs_world->set_attribute(0, *this);
    }

    Entity Scene::create_entity() {
        return Entity(get_ecs_world()->allocate_entity(), this);
    }
    void Scene::destroy_entity(Entity entity) {
        assert(this == entity.get_scene());
        get_ecs_world()->destroy_entity(entity.get_ecs_id());
    }

    Status Scene::build(const SceneData& data) {
        return StatusCode::Ok;
    }

    void Scene::advance(float delta_time) {
        m_delta_time = delta_time;
        m_time += m_delta_time;
    }
    void Scene::clear() {
        m_ecs_world->clear();
    }
    void Scene::set_state(SceneState state) {
        m_state = state;
    }
    void Scene::finalize() {
        m_ecs_world.reset();
        m_visibility_system.reset();
        m_render_scene.reset();
    }
    const StringId& Scene::get_name() {
        return m_name;
    }
    EcsWorld* Scene::get_ecs_world() {
        return m_ecs_world.get();
    }
    VisibilitySystem* Scene::get_visibility_system() {
        return m_visibility_system.get();
    }
    RenderScene* Scene::get_render_scene() {
        return m_render_scene.get();
    }

}// namespace wmoge