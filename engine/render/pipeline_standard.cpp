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

#include "pipeline_standard.hpp"

#include "debug/profiler.hpp"
#include "math/math_utils2d.hpp"
#include "math/math_utils3d.hpp"
#include "render/pipeline/stage_overlay_2d.hpp"

namespace wmoge {

    PipelineStandard::PipelineStandard() {
        // allocate specific stages
        m_stage_overlay_2d = std::make_unique<PipelineStageOverlay2d>();
        m_stage_overlay_2d->set_pipeline(this);

        // setup list of all stages
        m_stages.push_back(m_stage_overlay_2d.get());

        // setup mapping to draw passes
        m_passes[int(DrawPass::Overlay2dPass)] = m_stage_overlay_2d.get();

        // trigger register to prepare stages before rendering
        for (auto stage : m_stages) {
            stage->on_register();
        }
    }

    PipelineStandard::~PipelineStandard() = default;

    void PipelineStandard::execute() {
        WG_AUTO_PROFILE_RENDER("PipelineStandard::execute");

        allocate_views();
        collect_objects();
        sort_queues();
        render();
    }

    void PipelineStandard::allocate_views() {
        WG_AUTO_PROFILE_RENDER("PipelineStandard::allocate_views");

        m_views.clear();

        auto& cameras = get_render_scene()->get_cameras_2d();

        for (RenderCamera2d* camera : cameras) {
            if (camera->need_view()) {
                m_views.push_back(make_ref<RenderView>());
                camera->on_view_setup(m_views.back().get());
            }
        }
    }

    void PipelineStandard::collect_objects() {
        WG_AUTO_PROFILE_RENDER("PipelineStandard::collect_objects");

        RenderViewList views;
        views.reserve(m_views.size());

        for (auto& view : m_views) {
            views.push_back(view.get());
        }

        RenderViewMask mask;
        mask.set();

        auto& objects_dynamic = get_render_scene()->get_objects_dynamic();

        for (auto object : objects_dynamic) {
            if (object->is_visible()) {
                object->on_render_dynamic(views, mask);
            }
        }
    }

    void PipelineStandard::sort_queues() {
        WG_AUTO_PROFILE_RENDER("PipelineStandard::sort_queues");

        for (int i = 0; i < int(m_views.size()); i++) {
            for (auto& queue : m_views[i]->m_draw_cmds) {
                queue.sort();
            }
        }
    }

    void PipelineStandard::render() {
        WG_AUTO_PROFILE_RENDER("PipelineStandard::render");

        for (int i = 0; i < int(m_views.size()); i++) {
            for (auto stage : m_stages) {
                stage->on_execute(i);
            }
        }
    }

}// namespace wmoge