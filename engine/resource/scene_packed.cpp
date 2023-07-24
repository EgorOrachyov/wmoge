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
#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "platform/file_system.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene_manager.hpp"

namespace wmoge {

    bool ScenePacked::load_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("ScenePacked::load_from_import_options");

        if (!Resource::load_from_yaml(node)) {
            return false;
        }

        auto params = node;

        std::string format;
        params["format"] >> format;

        std::string file;
        params["file"] >> file;

        assert(format == "text");

        if (format == "text") {
            std::vector<std::uint8_t> data;
            if (!Engine::instance()->file_system()->read_file(file, data)) {
                WG_LOG_ERROR("failed to read file " << file);
                return false;
            }

            m_scene_data_yaml.emplace(yaml_parse(data));
            if (m_scene_data_yaml.value().empty()) {
                WG_LOG_ERROR("failed to parse file " << file);
                return false;
            }

            return true;
        }

        WG_LOG_ERROR("unsupported scene format " << format);
        return false;
    }
    void ScenePacked::copy_to(Resource& copy) {
        Resource::copy_to(copy);
        auto* scene_packed              = dynamic_cast<ScenePacked*>(&copy);
        scene_packed->m_scene_data_yaml = m_scene_data_yaml;
    }

    AsyncResult<Ref<Scene>> ScenePacked::instantiate_async() {
        WG_AUTO_PROFILE_RESOURCE("ScenePacked::instantiate_async");

        assert(m_scene_data_yaml.has_value());

        if (!m_scene_data_yaml.has_value()) {
            WG_LOG_ERROR("cannot instantiate scene from no data");
            return AsyncResult<Ref<Scene>>{};
        }

        auto* resource_manager = Engine::instance()->resource_manager();
        auto& file             = m_scene_data_yaml.value();
        auto  scene_async      = make_async_op<Ref<Scene>>();
        auto  deps             = file["deps"];
        auto  scene_packed     = Ref<ScenePacked>(this);

        std::vector<Async> deps_loading;

        for (auto it = deps.first_child(); it.valid(); it = it.next_sibling()) {
            auto res       = SID("");//Yaml::read_sid(it);
            auto res_async = resource_manager->load_async(res);

            if (res_async.is_null()) {
                WG_LOG_ERROR("failed to obtain async load for dep " << res);
                return AsyncResult<Ref<Scene>>{};
            }

            deps_loading.push_back(res_async.as_async());
        }

        Task scene_task(get_name(), [scene_async, scene_packed](auto&) {
            WG_AUTO_PROFILE_RESOURCE("ScenePacked::construct_scene");

            Timer timer;

            timer.start();

            auto* scene_manager = Engine::instance()->scene_manager();
            auto  scene         = scene_manager->make_scene(scene_packed->get_name());

            if (!scene->m_root->on_load_from_yaml(scene_packed->m_scene_data_yaml.value()["tree"])) {
                WG_LOG_ERROR("failed to instantiate scene " << scene_packed->get_name() << " from text (yaml) file");
                scene_async->set_failed();
                return 0;
            }

            timer.stop();

            WG_LOG_INFO("instantiate scene " << scene_packed->get_name() << ", time: " << timer.get_elapsed_sec() << " sec");
            scene_async->set_result(std::move(scene));
            return 0;
        });

        WG_LOG_INFO("total deps to pre-load " << deps_loading.size() << " for " << get_name());

        auto scene_task_hnd = scene_task.schedule(Async::join(ArrayView(deps_loading)));

        scene_task_hnd.add_on_completion([scene_async](AsyncStatus status, std::optional<int>&) {
            if (status == AsyncStatus::Failed) {
                scene_async->set_failed();
            }
        });

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