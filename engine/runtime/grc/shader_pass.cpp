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

#include "shader_pass.hpp"

#include "core/log.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader.hpp"
#include "grc/shader_manager.hpp"

#include <cassert>

namespace wmoge {

    ShaderPass::ShaderPass(Shader& shader) {
        m_shader = &shader;
        m_params.resize(shader.get_num_spaces());

        set_technique(0);
    }

    void ShaderPass::set_param_block(std::int16_t idx, Ref<ShaderParamBlock> block) {
        assert(block);
        assert(idx < m_params.size());
        m_params[idx] = std::move(block);
    }

    void ShaderPass::set_technique(std::int16_t idx) {
        assert(idx < m_shader->get_num_techniques());
        m_permutation.technique_idx = idx;
        m_technique                 = &m_shader->get_reflection().techniques[idx];
        set_pass(0);
    }

    void ShaderPass::set_technique(const Strid& name) {
        auto q = m_shader->find_technique(name);
        if (q) {
            set_technique(q.value());
        }
    }

    void ShaderPass::set_pass(std::int16_t idx) {
        assert(idx < m_shader->get_num_passes(m_permutation.technique_idx));
        m_permutation.pass_idx = idx;
        m_pass                 = &m_technique->passes[m_permutation.pass_idx];
        m_pipeline_state       = m_pass->state;
    }

    void ShaderPass::set_pass(const Strid& name) {
        auto q = m_shader->find_pass(m_permutation.technique_idx, name);
        if (q) {
            set_pass(q.value());
        }
    }

    void ShaderPass::set_option(const Strid& name, const Strid& variant) {
        assert(m_technique);
        assert(m_pass);

        auto selector = [&](const ShaderOptions& options) {
            auto tq = options.options_map.find(name);
            if (tq != options.options_map.end()) {
                auto vq = options.options[tq->second].variants.find(variant);
                if (vq != options.options[tq->second].variants.end()) {
                    m_permutation.options.set(vq->second);
                    return true;
                }
            }
            return false;
        };

        if (!selector(m_technique->options) || !selector(m_pass->options)) {
            WG_LOG_ERROR("no such option " << name << "=" << variant);
        }
    }

    void ShaderPass::set_attribs(int buffer, GfxVertAttribs attribs, std::optional<GfxVertAttribs> layout) {
        m_permutation.vert_attribs |= attribs;
        m_vert_attribs[buffer] = attribs;
        m_vert_layout[buffer]  = layout.value_or(attribs);
        m_vert_instanced.set(buffer, false);
    }

    void ShaderPass::set_attribs_instanced(int buffer, GfxVertAttribs attribs, std::optional<GfxVertAttribs> layout) {
        m_permutation.vert_attribs |= attribs;
        m_vert_attribs[buffer] = attribs;
        m_vert_layout[buffer]  = layout.value_or(attribs);
        m_vert_instanced.set(buffer, true);
    }

    void ShaderPass::set_prim_type(GfxPrimType prim_type) {
        m_prim_type = prim_type;
    }

    void ShaderPass::set_rs(const RasterState& rs) {
        m_pipeline_state.rs = rs;
    }

    void ShaderPass::set_ds(const DepthStencilState& ds) {
        m_pipeline_state.ds = ds;
    }

    void ShaderPass::set_bs(const BlendState& bs) {
        m_pipeline_state.bs = bs;
    }

    Status ShaderPass::configure(GfxCmdList& cmd_list) {
        assert(m_technique);
        assert(m_pass);

        if (m_shader->is_graphics()) {
            WG_CHECKED(configure_graphics(cmd_list));
        } else if (m_shader->is_compute()) {
            WG_CHECKED(configure_compute(cmd_list));
        } else {
            WG_LOG_ERROR("unsupported domain of shader " << m_shader->get_name());
            return StatusCode::InvalidState;
        }

        ShaderManager* shader_manager = nullptr;
        GfxDriver*     driver         = nullptr;

        const std::int16_t num_spaces = m_shader->get_num_spaces();
        for (std::int16_t i = 0; i < num_spaces; i++) {
            if (!m_params[i]) {
                return StatusCode::InvalidState;
            }
            WG_CHECKED(m_params[i]->validate(shader_manager, driver, &cmd_list));
        }

        return WG_OK;
    }

    Status ShaderPass::configure_graphics(GfxCmdList& cmd_list) {
        ShaderManager* shader_manager = nullptr;

        auto platform = shader_manager->get_active_platform();
        auto program  = shader_manager->get_or_create_program(m_shader, platform, m_permutation);

        if (!program) {
            return StatusCode::NoValue;
        }

        PsoCache* pso_cache = nullptr;

        GfxVertElements ve;
        for (int i = 0; i < GfxLimits::MAX_VERT_STREAMS; i++) {
            if (m_vert_attribs[i].bits.any()) {
                ve.add_vert_attribs(m_vert_attribs[i], m_vert_layout[i], i, m_vert_instanced[i]);
            }
        }

        GfxRenderPassRef rp;
        cmd_list.peek_render_pass(rp);

        GfxPsoStateGraphics state;
        state.pass        = rp;
        state.program     = program;
        state.layout      = shader_manager->get_shader_pso_layout(m_shader);
        state.vert_format = pso_cache->get_or_create_vert_format(ve, ve.to_name());
        state.prim_type   = m_prim_type;
        m_pipeline_state.fill(state);

        GfxPsoRef pso_ref = pso_cache->get_or_create_pso(state, program->name());

        if (!pso_ref) {
            return StatusCode::NoValue;
        }

        cmd_list.bind_pso(pso_ref.cast<GfxPsoGraphics>());
        return WG_OK;
    }

    Status ShaderPass::configure_compute(GfxCmdList& cmd_list) {
        ShaderManager* shader_manager = nullptr;

        auto platform = shader_manager->get_active_platform();
        auto program  = shader_manager->get_or_create_program(m_shader, platform, m_permutation);

        if (!program) {
            return StatusCode::NoValue;
        }

        GfxPsoStateCompute state;
        state.program = program;
        state.layout  = shader_manager->get_shader_pso_layout(m_shader);

        PsoCache* pso_cache = nullptr;
        GfxPsoRef pso_ref   = pso_cache->get_or_create_pso(state, program->name());

        if (!pso_ref) {
            return StatusCode::NoValue;
        }

        cmd_list.bind_pso(pso_ref.cast<GfxPsoCompute>());
        return WG_OK;
    }

}// namespace wmoge
