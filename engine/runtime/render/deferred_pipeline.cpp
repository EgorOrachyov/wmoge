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

#include "platform/window_manager.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    DeferredPipeline::DeferredPipeline() {
        WG_AUTO_PROFILE_RENDER("DeferredPipeline::DeferredPipeline");

        for (GraphicsPipelineStage* stage : m_stages) {
            stage->set_pipeline(this);
        }
    }

    void DeferredPipeline::exectute() {
        WG_AUTO_PROFILE_RENDER("DeferredPipeline::exectute");

        if (m_views.empty()) {
            return;
        }

        const int view_count = int(m_views.size());
        // Engine*   engine     = Engine::instance();

        for (int i = view_count - 1; i >= 0; i--) {
            WG_AUTO_PROFILE_RENDER("Render view=" + StringUtils::from_int(i));

            const int         view_idx = i;
            const CameraData& camera   = m_cameras->data_at(view_idx);
        }
    }

}// namespace wmoge