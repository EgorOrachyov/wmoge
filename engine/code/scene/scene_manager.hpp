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

#pragma once

#include "core/async.hpp"
#include "core/flat_map.hpp"
#include "core/task_manager.hpp"
#include "scene/scene.hpp"
#include "scene/scene_data.hpp"
#include "scene/scene_feature.hpp"

#include <deque>
#include <memory>
#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class SceneManager
     * @brief Manager for game loaded and active scenes
     */
    class SceneManager final {
    public:
        SceneManager() = default;

        void                               clear();
        std::optional<SceneRef>            find_scene_by_name(const Strid& name);
        SceneRef                           make_scene(const Strid& name);
        Status                             build_scene(const SceneRef& scene, const SceneData& data);
        Async                              build_scene_async(TaskManager* task_manager, const SceneRef& scene, const Ref<SceneDataAsset>& data);
        void                               add_trait(const Ref<EntityFeatureTrait>& trait);
        std::optional<EntityFeatureTrait*> find_trait(const Strid& rtti);

    private:
        std::vector<SceneRef>                    m_scenes;  // allocated scenes in the engine
        std::deque<SceneRef>                     m_to_clear;// scheduled to be cleared
        flat_map<Strid, Ref<EntityFeatureTrait>> m_traits;
    };

    void bind_by_ioc_scene_manager(class IocContainer* ioc);

}// namespace wmoge