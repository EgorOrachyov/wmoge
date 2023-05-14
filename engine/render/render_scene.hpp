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

#ifndef WMOGE_RENDER_SCENE_HPP
#define WMOGE_RENDER_SCENE_HPP

#include "core/callback_queue.hpp"
#include "core/fast_set.hpp"
#include "render/objects/render_camera_2d.hpp"
#include "render/render_object.hpp"

namespace wmoge {

    /**
     * @class RenderScene
     * @brief Private render system representation of the scene
     */
    class RenderScene {
    public:
        RenderScene();

        void flush_queue();

        void add_object(Ref<RenderObject> object);
        void remove_object(Ref<RenderObject> object);
        void add_camera(Ref<RenderCamera2d> camera);
        void remove_camera(Ref<RenderCamera2d> camera);

        void set_pipeline(class RenderPipeline* pipeline);
        void set_time(float time, float time_dt);

        const fast_set<Ref<RenderObject>>&     get_objects() { return m_objects; }
        const fast_vector<RenderObject*>&      get_objects_to_update() { return m_objects_to_update; }
        const fast_vector<RenderObject*>&      get_objects_dynamic() { return m_objects_dynamic; }
        const fast_set<class RenderCamera2d*>& get_cameras_2d() { return m_cameras_2d; }
        float                                  get_time() { return m_time; }
        float                                  get_time_dt() { return m_time_dt; }
        CallbackQueue*                         get_queue() { return &m_queue; }
        class GfxDriver*                       get_driver() { return m_driver; }
        class RenderPipeline*                  get_pipeline() { return m_pipeline; }

    private:
        fast_set<Ref<RenderObject>>     m_objects;           /** Set of all object stored in the scene to be rendered */
        fast_vector<RenderObject*>      m_objects_to_update; /** Subset of objects, which require update */
        fast_vector<RenderObject*>      m_objects_dynamic;   /** Objects requiring dynamic rendering path (collect cmds each frame) */
        fast_set<class RenderCamera2d*> m_cameras_2d;        /** 2d cameras for overlay view */

        /** Scene params, chanced from frame to frame */
        float m_time    = 0.0f;
        float m_time_dt = 0.0f;

        CallbackQueue         m_queue;
        class GfxDriver*      m_driver   = nullptr;
        class RenderPipeline* m_pipeline = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_SCENE_HPP
