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

#include "render_canvas_item.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "math/math_utils2d.hpp"
#include "render/draw_cmd_compiler.hpp"
#include "render/render_engine.hpp"
#include "render/render_scene.hpp"
#include "resource/shader_2d.hpp"

namespace wmoge {

    RenderCanvasItem::~RenderCanvasItem() {
        Engine*       engine        = Engine::instance();
        RenderEngine* render_engine = engine->render_engine();

        for (auto cmd : m_cached_cmds) {
            render_engine->free_draw_cmd(cmd);
        }
        m_cached_cmds.clear();
    }

    void RenderCanvasItem::set_material(const ref_ptr<Material>& material) {
        m_material = material;
    }
    void RenderCanvasItem::set_layer_id(int layer_id) {
        m_layer_id = layer_id;
        mark_dirty_params();
    }
    void RenderCanvasItem::set_transform(const Mat3x3f& new_model, const Mat3x3f& new_model_inv) {
        m_model_prev     = m_model;
        m_model_inv_prev = m_model_inv;
        m_model          = new_model;
        m_model_inv      = new_model_inv;
        mark_dirty_params();
    }
    void RenderCanvasItem::set_tint(const Color4f& tint) {
        m_tint = tint;
        mark_dirty_params();
    }

    void RenderCanvasItem::on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) {
        RenderObject::on_render_dynamic(views, mask);

        if (m_is_visible && m_dirty_params) {
            update_draw_params();
            m_dirty_params = false;
        }
    }

    void RenderCanvasItem::create_draw_params() {
        m_draw_params = m_driver->make_uniform_buffer(sizeof(Shader2d::DrawParams), GfxMemUsage::GpuLocal, get_name());
    }
    void RenderCanvasItem::create_vert_format() {
        GfxVertElements elements;
        elements.add_element(SID("pos"), GfxFormat::RG32F, 0, offsetof(GfxVF_Pos2Uv2Col4, pos), sizeof(GfxVF_Pos2Uv2Col4));
        elements.add_element(SID("uv"), GfxFormat::RG32F, 0, offsetof(GfxVF_Pos2Uv2Col4, uv), sizeof(GfxVF_Pos2Uv2Col4));
        elements.add_element(SID("col"), GfxFormat::RGBA32F, 0, offsetof(GfxVF_Pos2Uv2Col4, col), sizeof(GfxVF_Pos2Uv2Col4));
        m_vert_format = m_driver->make_vert_format(elements, SID("b0_Pos2Uv2Col4"));
    }
    void RenderCanvasItem::update_draw_params() {
        auto* ptr           = reinterpret_cast<Shader2d::DrawParams*>(m_driver->map_uniform_buffer(m_draw_params));
        ptr->model          = Math2d::from3x3to4x4(m_model).transpose();
        ptr->model_prev     = Math2d::from3x3to4x4(m_model_prev).transpose();
        ptr->model_inv      = Math2d::from3x3to4x4(m_model_inv).transpose();
        ptr->model_inv_prev = Math2d::from3x3to4x4(m_model_inv_prev).transpose();
        ptr->tint           = m_tint;
        ptr->layer_id       = m_layer_id;
        m_driver->unmap_uniform_buffer(m_draw_params);
    }
    void RenderCanvasItem::mark_dirty_params() {
        m_dirty_params = true;
    }
    void RenderCanvasItem::compile_draw_cmds(int index_count) {
        WG_AUTO_PROFILE_RENDER("RenderCanvasItem::compile_draw_cmds");

        assert(m_vert_format);
        assert(m_vert_buffer);
        assert(m_index_buffer);
        assert(m_material);

        Engine*         engine        = Engine::instance();
        RenderEngine*   render_engine = engine->render_engine();
        RenderMaterial* material      = m_material->get_render_material().get();

        for (auto cmd : m_cached_cmds) {
            render_engine->free_draw_cmd(cmd);
        }
        m_cached_cmds.clear();

        m_cached_cmds.resize(1, render_engine->allocate_draw_cmd());
        m_cached_keys.resize(1, DrawCmdSortingKey::make_overlay(material, m_layer_id));
        ArrayView<DrawCmd*> cmds(m_cached_cmds.data(), m_cached_cmds.size());

        DrawPrimitive primitive;
        primitive.draw_params.index_count    = index_count;
        primitive.draw_params.base_vertex    = 0;
        primitive.draw_params.instance_count = 1;
        primitive.vertices.buffers[0]        = m_vert_buffer.get();
        primitive.vertices.offsets[0]        = 0;
        primitive.indices                    = DrawIndexBuffer{m_index_buffer.get(), 0, GfxIndexType::Uint16};
        primitive.constants                  = DrawUniformBuffer{m_draw_params.get(), 0, m_draw_params->size(), 1};
        primitive.vert_format                = m_vert_format.get();
        primitive.material                   = material;
        primitive.draw_pass.set(DrawPass::Overlay2dPass);
        primitive.attribs   = MeshAttribs{};
        primitive.prim_type = GfxPrimType::Triangles;
        primitive.name      = get_name();

        DrawCmdCompiler compiler;
        compiler.set_render_scene(get_render_scene());

        if (!compiler.compile(primitive, cmds)) {
            WG_LOG_ERROR("failed to compile cmds for " << get_name());
            return;
        }
    }

}// namespace wmoge
