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

#include "scene_packed.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "core/log.hpp"
#include "core/task.hpp"
#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"
#include "scene/scene.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene_tree_visitors.hpp"

namespace wmoge {

    Status ScenePacked::read_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("ScenePacked::read_from_yaml");

        SceneTree& tree = m_scene_tree.emplace();
        WG_YAML_READ(node, tree);

        return StatusCode::Ok;
    }
    Status ScenePacked::copy_to(Object& copy) const {
        Resource::copy_to(copy);
        auto* scene_packed         = dynamic_cast<ScenePacked*>(&copy);
        scene_packed->m_scene_tree = m_scene_tree;
        return StatusCode::Ok;
    }

    AsyncResult<Ref<Scene>> ScenePacked::instantiate_async() {
        WG_AUTO_PROFILE_RESOURCE("ScenePacked::instantiate_async");

        assert(m_scene_tree.has_value());

        if (!m_scene_tree.has_value()) {
            WG_LOG_ERROR("cannot instantiate scene from no data");
            return AsyncResult<Ref<Scene>>{};
        }

        AsyncOp<Ref<Scene>> scene_async = make_async_op<Ref<Scene>>();

        Task scene_task(get_name(), [self = Ref<ScenePacked>(this), scene_async](TaskContext&) {
            Ref<Scene> scene = Engine::instance()->scene_manager()->make_scene(self->get_name());

            SceneTreeVisitorEmitScene visitor(scene);

            Timer timer;
            timer.start();

            if (!self->m_scene_tree.value().visit(visitor)) {
                WG_LOG_ERROR("failed to emit ecs scene from scene tree " << self->get_name());
                return 1;
            }

            self->m_scene_tree.value().copy_to(*scene->get_tree());

            timer.stop();
            WG_LOG_INFO("instantiate ecs scene " << self->get_name() << ", time: " << timer.get_elapsed_sec() << " sec");

            scene_async->set_result(std::move(scene));

            return 0;
        });

        scene_task.schedule();

        return AsyncResult<Ref<Scene>>(scene_async);
    }
    Ref<Scene> ScenePacked::instantiate() {
        WG_AUTO_PROFILE_RESOURCE("ScenePacked::instantiate");

        auto async = instantiate_async();
        async.wait_completed();
        return async.is_failed() ? Ref<Scene>{} : async.result();
    }

    void ScenePacked::register_class() {
        auto* cls = Class::register_class<ScenePacked>();
    }

}// namespace wmoge