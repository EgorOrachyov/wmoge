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

#include "render_particles_2d.hpp"

#include "debug/profiler.hpp"
#include "resource/shader_2d.hpp"

namespace wmoge {

    void RenderParticles2d::initialize(const ref_ptr<PfxEmitter>& emitter) {
        assert(emitter);

        m_emitter = emitter;
    }

    void RenderParticles2d::on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) {
        WG_AUTO_PROFILE_RENDER();

        if (m_dirty_params) {
            auto* ptr           = reinterpret_cast<Shader2d::DrawParams*>(m_driver->map_uniform_buffer(m_draw_params_no_transform));
            ptr->model          = Math2d::identity3x3();
            ptr->model_prev     = Math2d::identity3x3();
            ptr->model_inv      = Math2d::identity3x3();
            ptr->model_inv_prev = Math2d::identity3x3();
            ptr->tint           = m_tint;
            ptr->layer_id       = m_layer_id;
            m_driver->unmap_uniform_buffer(m_draw_params_no_transform);
        }

        RenderCanvasItem::on_render_dynamic(views, mask);

        m_emitter->render(views, mask, this);
    }
    void RenderParticles2d::on_scene_enter() {
        WG_AUTO_PROFILE_RENDER();

        create_draw_params();
        create_vert_format();

        m_draw_params_no_transform = m_driver->make_uniform_buffer(sizeof(Shader2d::DrawParams), GfxMemUsage::GpuLocal, get_name());
    }
    bool RenderParticles2d::need_update() const {
        return false;
    }
    bool RenderParticles2d::need_render_dynamic() const {
        return true;
    }

}// namespace wmoge