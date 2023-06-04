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

#include "pfx_sprite_renderer.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "render/draw_cmd_compiler.hpp"
#include "render/draw_primitive.hpp"
#include "render/objects/render_particles_2d.hpp"

#include <cstring>

namespace wmoge {

    PfxSpriteRenderer::PfxSpriteRenderer(Ref<Material> material, int vert_buffer_size, int index_buffer_size, const StringId& name) {
        WG_AUTO_PROFILE_PFX("PfxSpriteRenderer::PfxSpriteRenderer");

        assert(material);

        auto* gfx_driver = Engine::instance()->gfx_driver();

        m_name         = name;
        m_material     = std::move(material);
        m_vert_buffer  = gfx_driver->make_vert_buffer(vert_buffer_size, GfxMemUsage::GpuLocal, name);
        m_index_buffer = gfx_driver->make_index_buffer(index_buffer_size, GfxMemUsage::GpuLocal, name);
    }

    void PfxSpriteRenderer::on_update_data(const PfxComponentRenderData& data) {
        m_data = data;
    }
    void PfxSpriteRenderer::on_render_dynamic(RenderViewList& views, const RenderViewMask& mask, RenderObject* object) {
        WG_AUTO_PROFILE_PFX("PfxSpriteRenderer::on_render_dynamic");

        if (m_data.n_indices <= 0) {
            return;
        }

        auto* gfx_driver = Engine::instance()->gfx_driver();
        auto* proxy      = dynamic_cast<RenderParticles2d*>(object);

        assert(proxy);
        assert(m_data.vertices);
        assert(m_data.indices);
        assert(m_data.n_indices);
        assert(m_data.n_vertices);

        if (!m_cmd_compiled) {
            RenderMaterial*   material    = m_material->get_render_material().get();
            GfxUniformBuffer* draw_params = proxy->get_draw_params_no_transform().get();
            GfxVertFormat*    vert_format = proxy->get_vert_format().get();

            m_cmd_key = DrawCmdSortingKey::make_overlay(material, proxy->get_layer_id());

            DrawPrimitive primitive;
            primitive.draw_params.index_count    = m_data.n_indices;
            primitive.draw_params.base_vertex    = 0;
            primitive.draw_params.instance_count = 1;
            primitive.vertices.buffers[0]        = m_vert_buffer.get();
            primitive.vertices.offsets[0]        = 0;
            primitive.indices                    = DrawIndexBuffer{m_index_buffer.get(), 0, GfxIndexType::Uint16};
            primitive.constants                  = DrawUniformBuffer{draw_params, 0, draw_params->size(), 1};
            primitive.vert_format                = vert_format;
            primitive.material                   = material;
            primitive.draw_pass.set(DrawPass::Overlay2dPass);
            primitive.attribs   = {};
            primitive.prim_type = GfxPrimType::Triangles;
            primitive.name      = m_name;

            DrawCmdCompiler compiler;
            compiler.set_render_scene(proxy->get_render_scene());

            DrawCmd*            ptr = &m_draw_cmd;
            ArrayView<DrawCmd*> cmds(&ptr, 1);

            if (!compiler.compile(primitive, cmds)) {
                WG_LOG_ERROR("failed to compile cmds for " << m_name);
                return;
            }

            m_cmd_compiled = true;
        }

        std::memcpy(gfx_driver->map_vert_buffer(m_vert_buffer), m_data.vertices->buffer(), m_data.vertices->size());
        gfx_driver->unmap_vert_buffer(m_vert_buffer);

        std::memcpy(gfx_driver->map_index_buffer(m_index_buffer), m_data.indices->buffer(), m_data.indices->size());
        gfx_driver->unmap_index_buffer(m_index_buffer);

        m_draw_cmd.draw_params.instance_count = 1;
        m_draw_cmd.draw_params.base_vertex    = 0;
        m_draw_cmd.draw_params.index_count    = m_data.n_indices;

        for (int view_idx = 0; view_idx < int(views.size()); view_idx++) {
            RenderView* view = views[view_idx];

            if (!mask[view_idx]) {
                continue;
            }
            if (!view->is_overlay_view()) {
                continue;
            }

            DrawCmdSortingKey key = m_cmd_key;
            DrawCmd*          cmd = &m_draw_cmd;
            view->add_cmd(key, cmd, DrawPass::Overlay2dPass);
        }
    }
    bool PfxSpriteRenderer::need_render_dynamic() const {
        return true;
    }

}// namespace wmoge