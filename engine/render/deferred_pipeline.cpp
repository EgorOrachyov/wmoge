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

#include "deferred_pipeline.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "geometry/pass_gbuffer.hpp"
#include "platform/window_manager.hpp"
#include "post_process/pass_bloom.hpp"
#include "post_process/pass_composition.hpp"
#include "post_process/pass_tonemap.hpp"

namespace wmoge {

    DeferredPipeline::DeferredPipeline()  = default;
    DeferredPipeline::~DeferredPipeline() = default;

    void DeferredPipeline::init() {
        WG_AUTO_PROFILE_RENDER("DeferredPipeline::init");

        m_pass_gbuffer     = std::make_unique<PassGBuffer>();
        m_pass_bloom       = std::make_unique<PassBloom>();
        m_pass_tonemap     = std::make_unique<PassToneMap>();
        m_pass_composition = std::make_unique<PassComposition>();

        m_stages.push_back(m_pass_gbuffer.get());
        m_stages.push_back(m_pass_bloom.get());
        m_stages.push_back(m_pass_tonemap.get());
        m_stages.push_back(m_pass_composition.get());

        for (GraphicsPipelineStage* stage : m_stages) {
            stage->set_pipeline(this);
        }

        m_textures.resize(m_target_resolution);
        m_textures.update_viewport(m_resolution);
    }

    void DeferredPipeline::exectute() {
        WG_AUTO_PROFILE_RENDER("DeferredPipeline::exectute");

        if (m_views.empty()) {
            return;
        }

        const int view_count = int(m_views.size());
        Engine*   engine     = Engine::instance();
        GfxCtx*   gfx_ctx    = engine->gfx_ctx();

        WG_GFX_LABEL(gfx_ctx, SID("DeferredPipeline::exectute"));

        for (int i = view_count - 1; i >= 0; i--) {
            const int               view_idx = i;
            const RenderCameraData& camera   = m_cameras->data_at(view_idx);

            WG_GFX_LABEL(gfx_ctx, SID("Render view=" + StringUtils::from_int(view_idx)));

            if (camera.type == CameraType::Color) {
                assert(view_idx == 0);

                m_pass_gbuffer->execute(view_idx);
                m_pass_bloom->execute(view_idx);
                m_pass_tonemap->execute(view_idx);
                m_pass_composition->execute(0, engine->window_manager()->primary_window());
            }
        }
    }

    std::vector<GraphicsPipelineStage*> DeferredPipeline::get_stages() {
        return m_stages;
    }

    std::string DeferredPipeline::get_name() {
        return "DeferredPipeline";
    }

}// namespace wmoge