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

#include "render_scene.hpp"

#include "core/engine.hpp"
#include "gfx/gfx_driver.hpp"

#include <cassert>

namespace wmoge {

    RenderScene::RenderScene() {
        m_driver = Engine::instance()->gfx_driver();
    }

    void RenderScene::flush_queue() {
        m_queue.flush();

        for (auto object : m_objects_to_update) {
            object->on_update(m_time_dt);
        }
    }

    void RenderScene::add_object(Ref<RenderObject> object) {
        assert(object);
        assert(m_objects.find(object) == m_objects.end());

        object->m_render_scene = this;
        object->m_driver       = m_driver;

        m_objects.emplace(object);

        if (object->need_update()) {
            m_objects_to_update.push_back(object.get());
        }
        if (object->need_render_dynamic()) {
            m_objects_dynamic.push_back(object.get());
        }

        object->on_scene_enter();
    }

    void RenderScene::remove_object(Ref<RenderObject> object) {
        assert(object);
        assert(m_objects.find(object) != m_objects.end());

        object->on_scene_exit();
        object->m_render_scene = nullptr;
        object->m_driver       = nullptr;

        m_objects.erase(object);
        m_objects_to_update.erase(std::find(m_objects_to_update.begin(), m_objects_to_update.end(), object.get()));
        m_objects_dynamic.erase(std::find(m_objects_dynamic.begin(), m_objects_dynamic.end(), object.get()));
    }

    void RenderScene::add_camera(Ref<RenderCamera2d> camera) {
        add_object(camera.as<RenderObject>());
        m_cameras_2d.emplace(camera.get());
    }

    void RenderScene::remove_camera(Ref<RenderCamera2d> camera) {
        remove_object(camera.as<RenderObject>());
        m_cameras_2d.erase(camera.get());
    }

    void RenderScene::set_pipeline(class RenderPipeline* pipeline) {
        m_pipeline = pipeline;
    }
    void RenderScene::set_time(float time, float time_dt) {
        m_time    = time;
        m_time_dt = time_dt;
    }

}// namespace wmoge