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
#include "post_process/pass_tonemap.hpp"

namespace wmoge {

    DeferredPipeline::DeferredPipeline()  = default;
    DeferredPipeline::~DeferredPipeline() = default;

    void DeferredPipeline::init() {
        WG_AUTO_PROFILE_RENDER("DeferredPipeline::init");

        m_pass_gbuffer = std::make_unique<PassGBuffer>();
        m_pass_tonemap = std::make_unique<PassToneMap>();

        m_stages.push_back(m_pass_gbuffer.get());
        m_stages.push_back(m_pass_tonemap.get());

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

        for (int i = view_count - 1; i >= 0; i--) {
            const int               view_idx = i;
            const RenderCameraData& camera   = m_cameras->data_at(view_idx);

            if (camera.type == CameraType::Color) {
                assert(view_idx == 0);

                m_pass_gbuffer->execute(view_idx);
                m_pass_tonemap->execute(view_idx, Engine::instance()->window_manager()->primary_window());//tmp
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