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

#include "pass_tonemap.hpp"

#include "debug/profiler.hpp"
#include "shaders/generated/auto_tonemap_reflection.hpp"

namespace wmoge {

    PassToneMap::PassToneMap() {
        GfxSamplerDesc sampler_desc;
        sampler_desc.min_flt = GfxSampFlt::Linear;
        sampler_desc.mag_flt = GfxSampFlt::Linear;
        sampler_desc.u       = GfxSampAddress::ClampToEdge;
        sampler_desc.v       = GfxSampAddress::ClampToEdge;

        m_sampler = get_gfx_driver()->make_sampler(sampler_desc, SID(sampler_desc.to_str()));

        GfxCompPipelineState pso_state;
        pso_state.shader = get_shader_manager()->get_shader(SID("tonemap"), {});

        m_pipeline = get_gfx_driver()->make_comp_pipeline(pso_state, SID("tonemap"));
    }

    void PassToneMap::execute(int view_idx) {
        WG_AUTO_PROFILE_RENDER("PassToneMap::execute");

        const BloomSettings&            bloom_settings   = get_pipeline()->get_settings().bloom;
        const TonemapSettings&          tonemap_settings = get_pipeline()->get_settings().tonemap;
        const RenderView&               view             = get_pipeline()->get_views()[view_idx];
        const GraphicsPipelineTextures& textures         = get_pipeline()->get_textures();
        const RenderSettings&           render_settings  = get_render_engine()->get_settings();

        ShaderTonemap::Params params;
        params.TargetSize             = textures.size;
        params.InverseGamma           = 1.0f / render_settings.gamma;
        params.Exposure               = tonemap_settings.exposure;
        params.WhitePoint             = tonemap_settings.white_point;
        params.Mode                   = float(tonemap_settings.mode);
        params.BloomIntensity         = bloom_settings.intensity;
        params.BloomDirtMaskIntensity = bloom_settings.dirt_mask_intensity;

        auto setup = get_gfx_driver()->uniform_pool()->allocate(params);

        const Ref<GfxTexture> black              = get_tex_manager()->get_gfx_default_texture_black();
        const Ref<GfxTexture> bloom              = bloom_settings.enable ? textures.bloom_upsample[0] : black;
        const bool            has_bloom_dirt     = bloom_settings.enable && bloom_settings.dirt_mask.has_value();
        const Ref<GfxTexture> bloom_dirt         = has_bloom_dirt ? bloom_settings.dirt_mask->get_safe()->get_texture() : black;
        const Ref<GfxSampler> bloom_dirt_sampler = has_bloom_dirt ? bloom_settings.dirt_mask->get_safe()->get_sampler() : m_sampler;

        GfxDescSetResources resources;
        {
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderTonemap::PARAMS_SLOT;
                bind.type           = GfxBindingType::UniformBuffer;
                value.resource      = Ref<GfxResource>(setup.buffer);
                value.offset        = setup.offset;
                value.range         = setup.range;
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderTonemap::IMAGE_SLOT;
                bind.type           = GfxBindingType::SampledTexture;
                value.resource      = textures.color_hdr.as<GfxResource>();
                value.sampler       = m_sampler;
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderTonemap::BLOOM_SLOT;
                bind.type           = GfxBindingType::SampledTexture;
                value.resource      = bloom.as<GfxResource>();
                value.sampler       = m_sampler;
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderTonemap::BLOOMDIRTMASK_SLOT;
                bind.type           = GfxBindingType::SampledTexture;
                value.resource      = bloom_dirt.as<GfxResource>();
                value.sampler       = bloom_dirt_sampler;
            }
            {
                auto& [bind, value] = resources.emplace_back();
                bind.binding        = ShaderTonemap::RESULT_SLOT;
                bind.type           = GfxBindingType::StorageImage;
                value.resource      = textures.color_ldr.as<GfxResource>();
            }
        }

        auto desc_set = get_gfx_driver()->make_desc_set(resources, SID("tonemap"));

        get_gfx_ctx()->execute([&](GfxCtx* thread_ctx) {
            WG_GFX_LABEL(thread_ctx, SID("PassToneMap::execute"));

            thread_ctx->barrier_image(textures.color_ldr, GfxTexBarrierType::Storage);

            if (thread_ctx->bind_comp_pipeline(m_pipeline)) {
                thread_ctx->bind_desc_set(desc_set, 0);
                thread_ctx->dispatch(GfxCtx::group_size(params.TargetSize.x(), params.TargetSize.y(), 16));
            }

            thread_ctx->barrier_image(textures.color_ldr, GfxTexBarrierType::Sampling);
        });
    }

    std::string PassToneMap::get_name() const {
        return "PassToneMap";
    }

    GraphicsPipelineStageType PassToneMap::get_type() const {
        return GraphicsPipelineStageType::ToneMapping;
    }

}// namespace wmoge