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

#include "hgfx_pass_base.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "render/shader_manager.hpp"
#include "shaders/generated/auto_base_reflection.hpp"

namespace wmoge {

    bool HgfxPassBase::compile(GfxCtx* gfx_ctx) {
        WG_AUTO_PROFILE_HGFX("HgfxPassBase::compile");

        Engine*        engine         = Engine::instance();
        ShaderManager* shader_manager = engine->shader_manager();
        GfxDriver*     gfx_driver     = engine->gfx_driver();

        fast_vector<std::string> defines;
        if (out_srgb) defines.push_back("OUT_SRGB");
        if (no_alpha) defines.push_back("NO_ALPHA");

        assert(attribs.get(GfxVertAttrib::Pos3f));
        GfxVertAttribsStreams streams = {attribs};

        GfxVertElements elements;
        elements.add_vert_attribs(attribs, 0, false);

        GfxPipelineState pipeline_state{};
        pipeline_state.shader       = shader_manager->get_shader(SID("base"), streams, defines);
        pipeline_state.vert_format  = gfx_driver->make_vert_format(elements, name);
        pipeline_state.prim_type    = prim_type;
        pipeline_state.poly_mode    = poly_mode;
        pipeline_state.cull_mode    = cull_mode;
        pipeline_state.front_face   = front_face;
        pipeline_state.depth_enable = depth_enable;
        pipeline_state.depth_write  = depth_write;
        pipeline_state.depth_func   = depth_func;
        pipeline_state.blending     = !no_alpha;

        m_pipeline = gfx_driver->make_pipeline(pipeline_state, name);

        ShaderBase::Params params;
        params.mat_clip_proj_view = (gfx_driver->clip_matrix() * mat_proj_view).transpose();
        params.inverse_gamma      = 1.0f / gamma;
        params.mix_weight_1       = mix_weights[0];
        params.mix_weight_2       = mix_weights[1];
        params.mix_weight_3       = mix_weights[2];

        m_buffer_setup = gfx_driver->uniform_pool()->allocate(params);

        return true;
    }

    bool HgfxPassBase::configure(GfxCtx* gfx_ctx) {
        WG_AUTO_PROFILE_HGFX("HgfxPassBase::configure");

        if (gfx_ctx->bind_pipeline(m_pipeline)) {
            gfx_ctx->bind_uniform_buffer(ShaderBase::PARAMS_LOC, m_buffer_setup.offset, m_buffer_setup.range, Ref<GfxUniformBuffer>(m_buffer_setup.buffer));
            return true;
        }

        return false;
    }

    StringId HgfxPassBase::get_pass_name() {
        return name;
    }
    HgfxPassType HgfxPassBase::get_pass_type() {
        return HgfxPassType::Default;
    }

    void HgfxPassBase::register_class() {
        auto* cls = Class::register_class<HgfxPassBase>();
    }

}// namespace wmoge