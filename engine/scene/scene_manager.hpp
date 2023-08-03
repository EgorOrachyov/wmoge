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

#include "core/fast_map.hpp"
#include "core/fast_set.hpp"
#include "scene/scene.hpp"

#include <mutex>
#include <stack>
#include <unordered_map>
#include <vector>

namespace wmoge {

    /**
     * @class SceneManager
     * @brief Manager for game loaded and active scenes
     */
    class SceneManager final {
    public:
        SceneManager();

        void next(Ref<Scene> scene);
        void unload(const Ref<Scene>& scene);
        void unload_deferred(const Ref<Scene>& scene);
        void unload_all();

        Ref<Scene> get_running_scene();
        Ref<Scene> get_next_scene();
        Ref<Scene> make_scene(const StringId& name);

    private:
        fast_set<Ref<Scene>> m_scenes;   // allocated scenes in the engine
        fast_set<Ref<Scene>> m_to_unload;// scheduled to be unloaded

        Ref<Scene> m_running;// active scene
        Ref<Scene> m_next;   // scene to become active on next frame

        std::recursive_mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_MANAGER_HPP
