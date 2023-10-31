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

#include "pass_composition.hpp"

#include "debug/profiler.hpp"
#include "shaders/generated/auto_composition_reflection.hpp"

namespace wmoge {

    PassComposition::PassComposition() {
        GfxSamplerDesc sampler_desc;
        sampler_desc.min_flt = GfxSampFlt::Linear;
        sampler_desc.mag_flt = GfxSampFlt::Linear;
        sampler_desc.u       = GfxSampAddress::ClampToEdge;
        sampler_desc.v       = GfxSampAddress::ClampToEdge;

        m_sampler = get_gfx_driver()->make_sampler(sampler_desc, SID(sampler_desc.to_str()));

        GfxVertAttribs  attribs = {GfxVertAttrib::Pos2f, GfxVertAttrib::Uv02f};
        GfxVertElements elements;
        elements.add_vert_attribs(attribs, 0, false);

        GfxPipelineState pso_state;
        pso_state.shader       = get_shader_manager()->get_shader(SID("composition"), attribs, {});
        pso_state.vert_format  = get_gfx_driver()->make_vert_format(elements, SID("[pos2, uv2,]"));
        pso_state.depth_enable = false;
        pso_state.depth_write  = false;
        pso_state.blending     = false;

        m_pipeline = get_gfx_driver()->make_pipeline(pso_state, SID("composition"));
    }

    void PassComposition::execute(int view_idx, const Ref<Window>& window) {
        WG_AUTO_PROFILE_RENDER("PassComposition::execute");

        const GraphicsPipelineTextures& textures        = get_pipeline()->get_textures();
        const RenderSettings&           render_settings = get_render_engine()->get_settings();

        ShaderComposition::Params params;
        params.Clip = get_gfx_driver()->clip_matrix().transpose();

        auto setup = get_gfx_driver()->uniform_pool()->allocate(params);

        GfxDescSetResources resources;
        {
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderComposition::PARAMS_SLOT;
                bind.type           = GfxBindingType::UniformBuffer;
                value.resource      = Ref<GfxResource>(setup.buffer);
                value.offset        = setup.offset;
                value.range         = setup.range;
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderComposition::COLOR_SLOT;
                bind.type           = GfxBindingType::SampledTexture;
                value.resource      = textures.color_ldr.as<GfxResource>();
                value.sampler       = m_sampler;
            }
        }

        auto desc_set = get_gfx_driver()->make_desc_set(resources, SID("composition"));

        get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
            thread_ctx->begin_render_pass({}, SID("PassComposition::execute"));
            {
                thread_ctx->bind_target(window);
                thread_ctx->viewport(Rect2i(0, 0, window->fbo_width(), window->fbo_height()));
                thread_ctx->clear(1.0f, 0);

                if (thread_ctx->bind_pipeline(m_pipeline)) {
                    thread_ctx->bind_desc_set(desc_set, 0);
                    thread_ctx->bind_vert_buffer(get_render_engine()->get_fullscreen_tria(), 0, 0);
                    thread_ctx->draw(3, 0, 1);
                }
            }
            thread_ctx->end_render_pass();
        });
    }

    std::string PassComposition::get_name() const {
        return "PassComposition";
    }

    GraphicsPipelineStageType PassComposition::get_type() const {
        return GraphicsPipelineStageType::Composition;
    }

}// namespace wmoge