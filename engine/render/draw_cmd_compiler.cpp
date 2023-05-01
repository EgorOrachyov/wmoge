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

#include "draw_cmd_compiler.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "render/render_engine.hpp"
#include "render/render_pipeline.hpp"
#include "render/render_scene.hpp"

namespace wmoge {

    DrawCmdCompiler::DrawCmdCompiler() {
        m_engine        = Engine::instance();
        m_gfx_driver    = m_engine->gfx_driver();
        m_render_engine = m_engine->render_engine();
    }

    bool DrawCmdCompiler::compile(const DrawPrimitive& primitive, ArrayView<DrawCmd*> cmds) {
        WG_AUTO_PROFILE_RENDER();

        assert(primitive.draw_pass.get(DrawPass::Overlay2dPass));
        assert(!cmds.empty());
        assert(m_gfx_driver);

        DrawCmd* cmd = cmds[0];

        const auto&    shader  = primitive.material->get_material()->get_shader();
        ShaderVariant* variant = shader->create_variant(primitive.attribs, m_defines);

        if (!variant) {
            WG_LOG_ERROR("failed to compile primitive " << primitive.name);
            return false;
        }

        DrawMaterialBindings bindings;
        bindings.first_texture = variant->get_material_first_texture();
        bindings.first_buffer  = variant->get_material_first_buffer();

        GfxPipelineState pipeline_state{};
        pipeline_state.shader      = variant->get_gfx_shader();
        pipeline_state.pass        = m_render_scene->get_pipeline()->get_pass(DrawPass::Overlay2dPass)->get_gfx_pass();
        pipeline_state.vert_format = ref_ptr<GfxVertFormat>(primitive.vert_format);
        pipeline_state.prim_type   = primitive.prim_type;
        pipeline_state.poly_mode   = shader->get_poly_mode();
        pipeline_state.cull_mode   = shader->get_cull_mode();
        pipeline_state.front_face  = shader->get_front_face();
        pipeline_state.blending    = true;

        const auto pipeline = m_gfx_driver->make_pipeline(pipeline_state, shader->get_name());

        if (!pipeline) {
            WG_LOG_ERROR("failed to create pipeline for primitive " << primitive.name);
            return false;
        }

        cmd->draw_params = primitive.draw_params;
        cmd->vertices    = primitive.vertices;
        cmd->indices     = primitive.indices;
        cmd->constants   = primitive.constants;
        cmd->material    = primitive.material;
        cmd->pipeline    = pipeline.get();
        cmd->bindings    = bindings;

        return true;
    }

    void DrawCmdCompiler::set_engine(class Engine* engine) {
        m_engine = engine;
    }
    void DrawCmdCompiler::set_gfx_driver(class GfxDriver* gfx_driver) {
        m_gfx_driver = gfx_driver;
    }
    void DrawCmdCompiler::set_render_engine(class RenderEngine* render_engine) {
        m_render_engine = render_engine;
    }
    void DrawCmdCompiler::set_render_scene(class RenderScene* render_scene) {
        m_render_scene = render_scene;
    }

}// namespace wmoge