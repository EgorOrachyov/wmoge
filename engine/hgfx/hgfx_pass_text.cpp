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

#include "hgfx_pass_text.hpp"

#include "core/class.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "math/math_utils3d.hpp"
#include "render/shader_manager.hpp"
#include "shaders/generated/auto_text_reflection.hpp"

namespace wmoge {

    bool HgfxPassText::compile(GfxCtx* gfx_ctx) {
        WG_AUTO_PROFILE_HGFX("HgfxPassText::compile");

        Engine*        engine         = Engine::instance();
        ShaderManager* shader_manager = engine->shader_manager();
        GfxDriver*     gfx_driver     = engine->gfx_driver();

        if (!m_pipeline) {
            fast_vector<std::string> defines;
            if (out_srgb) defines.push_back("OUT_SRGB");

            GfxVertAttribs attribs = {GfxVertAttrib::Pos3f, GfxVertAttrib::Col04f, GfxVertAttrib::Uv02f};

            GfxVertElements elements;
            elements.add_vert_attribs(attribs, attribs, 0, false);

            GfxPipelineState pipeline_state{};
            pipeline_state.shader      = shader_manager->get_shader(SID("text"), attribs, defines);
            pipeline_state.vert_format = gfx_driver->make_vert_format(elements, name);
            pipeline_state.blending    = true;

            m_pipeline = gfx_driver->make_pipeline(pipeline_state, name);
        }

        ShaderText::Params params;
        params.mat_clip_proj_screen = (gfx_driver->clip_matrix() * Math3d::orthographic(0.0f, screen_size.x(), 0, screen_size.y(), -1000.0f, 1000.0f)).transpose();
        params.inverse_gamma        = 1.0f / gamma;

        m_buffer_setup = gfx_driver->uniform_pool()->allocate(params);

        GfxDescSetResources resources;
        {
            // params
            {
                auto& [point, value] = resources.emplace_back();
                point.type           = GfxBindingType::UniformBuffer;
                point.binding        = ShaderText::PARAMS_SLOT;
                value.resource       = Ref<GfxResource>(m_buffer_setup.buffer);
                value.offset         = m_buffer_setup.offset;
                value.range          = m_buffer_setup.range;
            }
            // font
            {
                auto& [point, value] = resources.emplace_back();
                point.type           = GfxBindingType::SampledTexture;
                point.binding        = ShaderText::FONTTEXTURE_SLOT;
                value.resource       = font_texture;
                value.sampler        = font_sampler;
            }
        }

        m_desc_set = gfx_driver->make_desc_set(resources, name);

        return true;
    }

    bool HgfxPassText::configure(GfxCtx* gfx_ctx) {
        WG_AUTO_PROFILE_HGFX("HgfxPassText::configure");

        if (gfx_ctx->bind_pipeline(m_pipeline)) {
            gfx_ctx->bind_desc_set(m_desc_set, 0);
            return true;
        }

        return false;
    }

    StringId HgfxPassText::get_pass_name() {
        return name;
    }
    HgfxPassType HgfxPassText::get_pass_type() {
        return HgfxPassType::Default;
    }

    void HgfxPassText::register_class() {
        auto* cls = Class::register_class<HgfxPassText>();
    }

}// namespace wmoge