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

#include "debug/profiler.hpp"
#include "gfx/gfx_ctx.hpp"
#include "render/render_object.hpp"

#include <cassert>

namespace wmoge {

    RenderScene::RenderScene() {
        m_objects_gpu_data.set_name(SID("objects_data_gpu"));
        m_objects_ids.set_name(SID("objects_ids"));
    }

    void RenderScene::add_object(RenderObject* object, VisibilityItem vis_item) {
        assert(object);

        if (m_free_objects_ids.empty()) {
            const int new_id   = int(m_objects.size());
            const int new_size = new_id + 1;

            m_objects.resize(new_size);
            m_objects_vis.resize(new_size);
            m_objects_gpu_data.resize(new_size);

            m_free_objects_ids.push_back(new_id);
        }

        assert(!m_free_objects_ids.empty());

        const int new_id = m_free_objects_ids.back();
        m_free_objects_ids.pop_back();

        m_objects[new_id]          = object;
        m_objects_vis[new_id]      = vis_item;
        m_objects_gpu_data[new_id] = GPURenderObjectData();

        object->set_primitive_id(new_id);
    }

    void RenderScene::remove_object(RenderObject* object) {
        assert(object);

        const int id = object->get_primitive_id();
        m_free_objects_ids.push_back(id);

        m_objects[id]          = nullptr;
        m_objects_vis[id]      = VisibilityItem();
        m_objects_gpu_data[id] = GPURenderObjectData();
    }

    void RenderScene::flush_buffers(GfxCtx* gfx_ctx) {
        WG_AUTO_PROFILE_RENDER("RenderScene::flush_buffers");

        m_objects_gpu_data.flush(gfx_ctx);
        m_objects_ids.flush(gfx_ctx);
    }

}// namespace wmoge