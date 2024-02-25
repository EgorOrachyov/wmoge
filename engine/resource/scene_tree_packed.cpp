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

#include "scene_tree_packed.hpp"

#include "core/class.hpp"
#include "core/log.hpp"
#include "core/task.hpp"
#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"
#include "scene/scene_manager.hpp"
#include "system/engine.hpp"

namespace wmoge {

    Status SceneTreePacked::read_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("SceneTreePacked::read_from_yaml");

        WG_YAML_READ(node, m_data);

        return StatusCode::Ok;
    }
    Status SceneTreePacked::copy_to(Object& copy) const {
        Resource::copy_to(copy);
        auto* scene_tree_packed   = dynamic_cast<SceneTreePacked*>(&copy);
        scene_tree_packed->m_data = m_data;
        return StatusCode::Ok;
    }

    AsyncResult<Ref<SceneTree>> SceneTreePacked::instantiate_async() {
        WG_AUTO_PROFILE_RESOURCE("SceneTreePacked::instantiate_async");

        AsyncOp<Ref<SceneTree>> scene_async = make_async_op<Ref<SceneTree>>();

        Task scene_task(get_name(), [self = Ref<SceneTreePacked>(this), scene_async](TaskContext&) {
            Ref<SceneTree> scene_tree = make_ref<SceneTree>(self->get_name());

            Timer timer;
            timer.start();

            scene_tree->build(self->m_data);

            timer.stop();
            WG_LOG_INFO("instantiate scene tree " << self->get_name() << ", time: " << timer.get_elapsed_sec() << " sec");

            scene_async->set_result(std::move(scene_tree));

            return 0;
        });

        scene_task.schedule();

        return AsyncResult<Ref<SceneTree>>(scene_async);
    }
    Ref<SceneTree> SceneTreePacked::instantiate() {
        WG_AUTO_PROFILE_RESOURCE("SceneTreePacked::instantiate");

        auto async = instantiate_async();
        async.wait_completed();
        return async.is_failed() ? Ref<SceneTree>{} : async.result();
    }

    void SceneTreePacked::register_class() {
        auto* cls = Class::register_class<SceneTreePacked>();
    }

}// namespace wmoge