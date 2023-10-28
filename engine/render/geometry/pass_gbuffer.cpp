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

#include "pass_gbuffer.hpp"

#include "debug/profiler.hpp"
#include "render/graphics_pipeline.hpp"

namespace wmoge {

    void PassGBuffer::execute(int view_idx) {
        WG_AUTO_PROFILE_RENDER("PassGBuffer::execute");

        const RenderView&               view     = get_pipeline()->get_views()[view_idx];
        const GraphicsPipelineTextures& textures = get_pipeline()->get_textures();

        get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
            thread_ctx->begin_render_pass({}, SID("PassGBuffer::execute"));
            {
                thread_ctx->bind_color_target(textures.gbuffer[0], 0, 0, 0);
                thread_ctx->bind_color_target(textures.gbuffer[1], 1, 0, 0);
                thread_ctx->bind_color_target(textures.gbuffer[2], 2, 0, 0);
                thread_ctx->bind_color_target(textures.primitive_id, 3, 0, 0);
                thread_ctx->bind_depth_target(textures.depth, 0, 0);
                thread_ctx->viewport(textures.viewport);
                thread_ctx->clear(0, Color::BLACK4f);
                thread_ctx->clear(1, Color::BLACK4f);
                thread_ctx->clear(2, Color::BLACK4f);
                thread_ctx->clear(3, Color::BLACK4f);
                thread_ctx->clear(1.0f, 0);

                view.queues[int(MeshPassType::GBuffer)].execute(thread_ctx);
            }
            thread_ctx->end_render_pass();
        });
    }

    std::string PassGBuffer::get_name() const {
        return "PassGBuffer";
    }

    GraphicsPipelineStageType PassGBuffer::get_type() const {
        return GraphicsPipelineStageType::SceneGBuffer;
    }

}// namespace wmoge