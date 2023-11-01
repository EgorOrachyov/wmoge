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

#include "pass_autoexposure.hpp"

#include "debug/profiler.hpp"
#include "shaders/generated/auto_luminance_avg_reflection.hpp"
#include "shaders/generated/auto_luminance_histogram_reflection.hpp"

namespace wmoge {

    PassAutoExposure::PassAutoExposure() {
        GfxSamplerDesc sampler_desc;
        sampler_desc.min_flt = GfxSampFlt::Linear;
        sampler_desc.mag_flt = GfxSampFlt::Linear;
        sampler_desc.u       = GfxSampAddress::ClampToEdge;
        sampler_desc.v       = GfxSampAddress::ClampToEdge;

        m_sampler = get_gfx_driver()->make_sampler(sampler_desc, SID(sampler_desc.to_str()));

        GfxCompPipelineState pso_state;

        pso_state.shader     = get_shader_manager()->get_shader(SID("luminance_histogram"), {});
        m_pipeline_histogram = get_gfx_driver()->make_comp_pipeline(pso_state, SID("luminance_histogram"));

        pso_state.shader = get_shader_manager()->get_shader(SID("luminance_avg"), {});
        m_pipeline_avg   = get_gfx_driver()->make_comp_pipeline(pso_state, SID("luminance_avg"));
    }

    void PassAutoExposure::execute(int view_idx) {
        WG_AUTO_PROFILE_RENDER("PassAutoExposure::execute");

        const AutoExposureSettings&     exposure_settings = get_pipeline()->get_settings().auto_exposure;
        const RenderView&               view              = get_pipeline()->get_views()[view_idx];
        const GraphicsPipelineTextures& textures          = get_pipeline()->get_textures();
        const GraphicsPipelineShared&   shared            = get_pipeline()->get_shared();
        const RenderSettings&           render_settings   = get_render_engine()->get_settings();

        if (!exposure_settings.enable) {
            return;
        }

        ShaderLuminanceHistogram::Params params;
        params.DeltaTime            = get_render_engine()->get_delta_time();
        params.TotalPixelsCount     = float(textures.size.x() * textures.size.y());
        params.Mode                 = float(exposure_settings.mode);
        params.ExposureCompensation = exposure_settings.exposure_compensation;
        params.SpeedUp              = exposure_settings.speed_up;
        params.SpeedDown            = exposure_settings.speed_down;
        params.HistogramLogMax      = exposure_settings.histogram_log_max;
        params.HistogramLogMin      = exposure_settings.histogram_log_min;

        assert(sizeof(ShaderLuminanceHistogram::Params) == sizeof(ShaderLuminanceAvg::Params));

        auto setup = get_gfx_driver()->uniform_pool()->allocate(params);

        GfxDescSetResources resources;
        {
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderLuminanceHistogram::PARAMS_SLOT;
                bind.type           = GfxBindingType::UniformBuffer;
                value.resource      = Ref<GfxResource>(setup.buffer);
                value.offset        = setup.offset;
                value.range         = setup.range;
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderLuminanceHistogram::HISTOGRAM_SLOT;
                bind.type           = GfxBindingType::StorageBuffer;
                value.resource      = shared.lum_histogram;
                value.offset        = 0;
                value.range         = shared.lum_histogram->size();
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderLuminanceHistogram::LUMINANCE_SLOT;
                bind.type           = GfxBindingType::StorageBuffer;
                value.resource      = shared.lum_luminance;
                value.offset        = 0;
                value.range         = shared.lum_luminance->size();
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderLuminanceHistogram::IMAGE_SLOT;
                bind.type           = GfxBindingType::SampledTexture;
                value.resource      = textures.color_hdr;
                value.sampler       = m_sampler;
            }
        }

        auto desc_set = get_gfx_driver()->make_desc_set(resources, SID("luminance_histogram"));

        get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
            WG_GFX_LABEL(thread_ctx, SID("PassAutoExposure::execute"));
            {
                WG_GFX_LABEL(thread_ctx, SID("luminance histogram"));

                if (thread_ctx->bind_comp_pipeline(m_pipeline_histogram)) {
                    thread_ctx->bind_desc_set(desc_set, 0);
                    thread_ctx->dispatch(GfxCtx::group_size(textures.size.x(), textures.size.y(), 16));
                }

                thread_ctx->barrier_buffer(shared.lum_histogram);
            }
            {
                WG_GFX_LABEL(thread_ctx, SID("luminance avg"));

                if (thread_ctx->bind_comp_pipeline(m_pipeline_avg)) {
                    thread_ctx->bind_desc_set(desc_set, 0);
                    thread_ctx->dispatch(Vec3i(1, 1, 1));
                }

                thread_ctx->barrier_buffer(shared.lum_luminance);
            }
        });
    }

    std::string PassAutoExposure::get_name() const {
        return "PassAutoExposure";
    }

    GraphicsPipelineStageType PassAutoExposure::get_type() const {
        return GraphicsPipelineStageType::ToneMapping;
    }

}// namespace wmoge