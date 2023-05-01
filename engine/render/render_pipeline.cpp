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

#include "render_pipeline.hpp"

#include "core/engine.hpp"
#include "platform/window_manager.hpp"
#include "resource/config_file.hpp"

namespace wmoge {

    class RenderPipeline* RenderPipelineStage::get_pipeline() {
        assert(m_pipeline);
        return m_pipeline;
    }
    class RenderScene* RenderPipelineStage::get_render_scene() {
        assert(m_pipeline);
        return m_pipeline->get_render_scene();
    }
    class GfxDriver* RenderPipelineStage::get_gfx_driver() {
        assert(m_pipeline);
        return m_pipeline->get_gfx_driver();
    }
    RenderView* RenderPipelineStage::get_view(int view_index) {
        assert(m_pipeline);
        return m_pipeline->get_view(view_index);
    }

    RenderPipeline::RenderPipeline() {
        m_gfx_driver = Engine::instance()->gfx_driver();
    }

    void RenderPipeline::set_scene(RenderScene* render_scene) {
        assert(render_scene);
        m_render_scene = render_scene;
    }

    const fast_vector<ref_ptr<RenderView>>& RenderPipeline::get_views() {
        return m_views;
    }
    const fast_vector<RenderPipelineStage*>& RenderPipeline::get_stages() {
        return m_stages;
    }
    const std::array<RenderPipelineStage*, int(DrawPass::Max)> RenderPipeline::get_passes() {
        return m_passes;
    }
    RenderView* RenderPipeline::get_view(int view_index) {
        assert(view_index < m_views.size());
        return m_views[view_index].get();
    }
    RenderPipelineStage* RenderPipeline::get_pass(wmoge::DrawPass pass) {
        assert(int(pass) < m_passes.size());
        return m_passes[int(pass)];
    }
    RenderScene* RenderPipeline::get_render_scene() {
        return m_render_scene;
    }
    GfxDriver* RenderPipeline::get_gfx_driver() {
        return m_gfx_driver;
    }

}// namespace wmoge