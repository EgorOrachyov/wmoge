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

#ifndef WMOGE_SCENE_MANAGER_HPP
#define WMOGE_SCENE_MANAGER_HPP

#include "scene/scene.hpp"

#include <deque>
#include <mutex>
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
        void                      make_active(Ref<Scene> scene);
        Ref<Scene>                get_running_scene();
        Ref<Scene>                make_scene(const StringId& name);
        std::optional<Ref<Scene>> find_by_name(const StringId& name);

    private:
        void scene_hier();
        void scene_transforms();
        void scene_render();
        void scene_pfx();
        void scene_scripting();
        void scene_physics();
        void scene_audio();

    private:
        std::vector<Ref<Scene>> m_scenes;  // allocated scenes in the engine
        std::deque<Ref<Scene>>  m_to_clear;// scheduled to be cleared
        Ref<Scene>              m_running; // active scene
        Ref<Scene>              m_default; // default scene to always show something

        std::mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_MANAGER_HPP
