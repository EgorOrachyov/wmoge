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

#include "pass_bloom.hpp"

#include "debug/profiler.hpp"
#include "shaders/generated/auto_bloom_reflection.hpp"

namespace wmoge {

    PassBloom::PassBloom() {
        GfxSamplerDesc sampler_desc;
        sampler_desc.min_flt = GfxSampFlt::Linear;
        sampler_desc.mag_flt = GfxSampFlt::Linear;
        sampler_desc.u       = GfxSampAddress::ClampToEdge;
        sampler_desc.v       = GfxSampAddress::ClampToEdge;

        m_sampler = get_gfx_driver()->make_sampler(sampler_desc, SID(sampler_desc.to_str()));

        GfxVertAttribs  attribgs = {GfxVertAttrib::Pos2f, GfxVertAttrib::Uv02f};
        GfxVertElements elements;
        elements.add_vert_attribs(attribgs, 0, false);

        GfxPipelineState pso_state;
        pso_state.depth_enable = false;
        pso_state.depth_write  = false;
        pso_state.blending     = false;
        pso_state.vert_format  = get_gfx_driver()->make_vert_format(elements, SID("[pos2, uv2,]"));

        pso_state.shader                = get_shader_manager()->get_shader(SID("bloom"), attribgs, {"BLOOM_DOWNSAMPLE_PREFILTER"});
        m_pipeline_downsample_prefilter = get_gfx_driver()->make_pipeline(pso_state, SID("bloom_downsample_prefilter"));

        pso_state.shader      = get_shader_manager()->get_shader(SID("bloom"), attribgs, {"BLOOM_DOWNSAMPLE"});
        m_pipeline_downsample = get_gfx_driver()->make_pipeline(pso_state, SID("bloom_downsample"));

        pso_state.shader    = get_shader_manager()->get_shader(SID("bloom"), attribgs, {"BLOOM_UPSAMPLE"});
        m_pipeline_upsample = get_gfx_driver()->make_pipeline(pso_state, SID("bloom_upsample"));
    }

    void PassBloom::execute(int view_idx) {
        WG_AUTO_PROFILE_RENDER("PassBloom::execute");

        const BloomSettings&            settings       = get_pipeline()->get_settings().bloom;
        const GraphicsPipelineTextures& textures       = get_pipeline()->get_textures();
        const int                       num_bloom_mips = int(textures.bloom_downsample.size());

        if (!settings.enable) {
            return;
        }

        ShaderBloom::Params params;
        params.Clip           = (get_gfx_driver()->clip_matrix()).transpose();
        params.ThresholdKnee  = Vec4f(settings.threshold, settings.threshold - settings.knee, settings.knee * 2.0f, 0.25f / settings.knee);
        params.UpsampleWeight = settings.uspample_weight;
        params.UpsampleRadius = settings.radius;

        auto setup = get_gfx_driver()->uniform_pool()->allocate(params);

        GfxDescSetResources resources;
        {
            auto& [bind, value] = resources.emplace_back();
            bind.type           = GfxBindingType::UniformBuffer;
            bind.binding        = ShaderBloom::PARAMS_SLOT;
            value.resource      = Ref<GfxResource>(setup.buffer);
            value.offset        = setup.offset;
            value.range         = setup.range;
        }
        {
            auto& [bind, value] = resources.emplace_back();
            bind.type           = GfxBindingType::SampledTexture;
            bind.binding        = ShaderBloom::SOURCE_SLOT;
            value.resource      = textures.color_hdr;
            value.sampler       = m_sampler;
        }
        {
            auto& [bind, value] = resources.emplace_back();
            bind.type           = GfxBindingType::SampledTexture;
            bind.binding        = ShaderBloom::SOURCEPREV_SLOT;
            value.resource      = textures.color_hdr;
            value.sampler       = m_sampler;
        }

        // Downsample prefilter pass
        {
            auto desc_set = get_gfx_driver()->make_desc_set(resources, SID("bloom_downsample_prefilter"));

            get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
                thread_ctx->begin_render_pass({}, SID("PassBloom::execute (downsample + prefilter)"));
                {
                    const int w = textures.bloom_downsample[0]->width();
                    const int h = textures.bloom_downsample[0]->height();

                    thread_ctx->bind_color_target(textures.bloom_downsample[0], 0, 0, 0);
                    thread_ctx->viewport(Rect2i(0, 0, w, h));

                    if (thread_ctx->bind_pipeline(m_pipeline_downsample_prefilter)) {
                        thread_ctx->bind_desc_set(desc_set, 0);
                        thread_ctx->bind_vert_buffer(get_render_engine()->get_fullscreen_tria(), 0, 0);
                        thread_ctx->draw(3, 0, 1);
                    }
                }
                thread_ctx->end_render_pass();
            });
        }

        // Downsample pass
        {
            for (int i = 1; i < num_bloom_mips; i++) {
                {
                    auto& [bind, value] = resources[ShaderBloom::SOURCE_SLOT];
                    value.resource      = textures.bloom_downsample[i - 1];
                }

                auto desc_set = get_gfx_driver()->make_desc_set(resources, SID("bloom_downsample"));

                get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
                    thread_ctx->begin_render_pass({}, SID("PassBloom::execute (downsample mip=" + StringUtils::from_int(i) + ")"));
                    {
                        const int w = textures.bloom_downsample[i]->width();
                        const int h = textures.bloom_downsample[i]->height();

                        thread_ctx->bind_color_target(textures.bloom_downsample[i], 0, 0, 0);
                        thread_ctx->viewport(Rect2i(0, 0, w, h));

                        if (thread_ctx->bind_pipeline(m_pipeline_downsample)) {
                            thread_ctx->bind_desc_set(desc_set, 0);
                            thread_ctx->bind_vert_buffer(get_render_engine()->get_fullscreen_tria(), 0, 0);
                            thread_ctx->draw(3, 0, 1);
                        }
                    }
                    thread_ctx->end_render_pass();
                });
            }
        }

        // Upsample pass
        {
            Ref<GfxTexture> source_prev = textures.bloom_downsample.back();

            for (int i = num_bloom_mips - 2; i >= 0; i--) {
                {
                    auto& [bind, value] = resources[ShaderBloom::SOURCE_SLOT];
                    value.resource      = textures.bloom_downsample[i];
                }
                {
                    auto& [bind, value] = resources[ShaderBloom::SOURCEPREV_SLOT];
                    value.resource      = source_prev;
                }

                auto desc_set = get_gfx_driver()->make_desc_set(resources, SID("bloom_upsample"));

                get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
                    thread_ctx->begin_render_pass({}, SID("PassBloom::execute (upsample mip=" + StringUtils::from_int(i) + ")"));
                    {
                        const int w = textures.bloom_upsample[i]->width();
                        const int h = textures.bloom_upsample[i]->height();

                        thread_ctx->bind_color_target(textures.bloom_upsample[i], 0, 0, 0);
                        thread_ctx->viewport(Rect2i(0, 0, w, h));

                        if (thread_ctx->bind_pipeline(m_pipeline_upsample)) {
                            thread_ctx->bind_desc_set(desc_set, 0);
                            thread_ctx->bind_vert_buffer(get_render_engine()->get_fullscreen_tria(), 0, 0);
                            thread_ctx->draw(3, 0, 1);
                        }
                    }
                    thread_ctx->end_render_pass();
                });

                source_prev = textures.bloom_upsample[i];
            }
        }
    }

    std::string PassBloom::get_name() const {
        return "PassBloom";
    }

    GraphicsPipelineStageType PassBloom::get_type() const {
        return GraphicsPipelineStageType::Bloom;
    }

}// namespace wmoge