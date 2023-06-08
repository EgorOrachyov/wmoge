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

#include "stage_overlay_2d.hpp"

#include "core/engine.hpp"
#include "debug/console.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_ctx.hpp"
#include "math/math_utils2d.hpp"
#include "math/math_utils3d.hpp"
#include "render/draw_cmd.hpp"
#include "render/render_view.hpp"
#include "resource/shader.hpp"

namespace wmoge {

    const StringId& PipelineStageOverlay2d::get_name() const {
        static StringId name("stage-overlay-2d");
        return name;
    }

    void PipelineStageOverlay2d::on_register() {
        WG_AUTO_PROFILE_RENDER("PipelineStageOverlay2d::on_register");

        GfxDriver* driver = get_gfx_driver();

        //        m_pass_params = driver->make_uniform_buffer(int(sizeof(Shader2d::PassParams)), GfxMemUsage::GpuLocal, SID("overlay-2d-params"));
    }

    void PipelineStageOverlay2d::on_execute(int view_index) {
        WG_AUTO_PROFILE_RENDER("PipelineStageOverlay2d::on_execute");

        GfxCtx*       ctx   = Engine::instance()->gfx_ctx();
        RenderView*   view  = get_view(view_index);
        RenderScene*  scene = get_render_scene();
        DrawCmdQueue& queue = view->get_draw_cmds(DrawPass::Overlay2dPass);

        const Vec2f half_screen = view->get_screen_size() * 0.5f;

        const Mat4x4f mat_clip        = ctx->clip_matrix();
        const Mat4x4f mat_view        = Math3d::orthographic(-half_screen.x(), half_screen.x(), -half_screen.y(), half_screen.y(), -1000.0f, 1000.0f);
        const Mat4x4f mat_camera      = Math2d::from3x3to4x4(view->get_screen_camera_mat_inv());
        const Mat4x4f mat_camera_prev = Math2d::from3x3to4x4(view->get_screen_camera_mat_inv_prev());

        //        auto* pass_params                = reinterpret_cast<Shader2d::PassParams*>(driver->map_uniform_buffer(m_pass_params));
        //        pass_params->clip_proj_view      = (mat_clip * mat_view * mat_camera).transpose();
        //        pass_params->clip_proj_view_prev = (mat_clip * mat_view * mat_camera_prev).transpose();
        //        pass_params->view_size           = view->get_screen_size();
        //        pass_params->time                = scene->get_time();
        //        pass_params->time_dt             = scene->get_time_dt();
        //        driver->unmap_uniform_buffer(m_pass_params);

        auto&  window        = view->get_display();
        auto&  viewport_rect = view->get_viewport_rect();
        Rect2i viewport      = Rect2i{int(viewport_rect.x() * window->fbo_width()),
                                 int(viewport_rect.y() * window->fbo_height()),
                                 int(viewport_rect.z() * window->fbo_width()),
                                 int(viewport_rect.w() * window->fbo_height())};

        ctx->begin_render_pass(GfxRenderPassDesc{}, SID("overlay-pass"));
        ctx->bind_target(window);
        ctx->viewport(viewport);

        DrawUniformBuffer pass_buffer;
        pass_buffer.location = 0;
        pass_buffer.offset   = 0;
        pass_buffer.range    = m_pass_params->size();
        pass_buffer.buffer   = m_pass_params.get();

        queue.execute(ctx, &pass_buffer, 1);

        ctx->end_render_pass();
    }

}// namespace wmoge