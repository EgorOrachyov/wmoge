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
#include "scene/scene.hpp"

#include <deque>
#include <memory>
#include <optional>
#include <stack>
#include <vector>

namespace wmoge {

    /**
     * @class SceneManager
     * @brief Manager for game loaded and active scenes
     */
    class SceneManager final {
    public:
        SceneManager();

        void                      clear();
        void                      update();
        void                      change(Ref<Scene> scene);
        Ref<Scene>                get_running_scene();
        Ref<Scene>                make_scene(const Strid& name);
        std::optional<Ref<Scene>> find_by_name(const Strid& name);

    private:
        void update_scene_hier();
        void update_scene_cameras();
        void update_scene_visibility();
        void render_scene();

    private:
        void scene_change();
        void scene_start();
        void scene_play();
        void scene_pause();
        void scene_resume();
        void scene_finish();

    private:
        std::vector<Ref<Scene>> m_scenes;  // allocated scenes in the engine
        std::deque<Ref<Scene>>  m_to_clear;// scheduled to be cleared

        Ref<Scene> m_running;// active scene
        Ref<Scene> m_next;   // next scene to set
        Ref<Scene> m_default;// default scene to always show something

        class EcsRegistry*  m_ecs_registry  = nullptr;
        class RenderEngine* m_render_engine = nullptr;

        struct SyncContext {
            Async complete_heir;
            Async complete_cameras;
            Async complete_visibility;
            Async complete_render;

            void await_all();
        };

        SyncContext m_sync;// Allows to sync different parts of scene update
    };

}// namespace wmoge