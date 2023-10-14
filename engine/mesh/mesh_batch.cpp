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

#include "mesh_batch.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"
#include "render/render_engine.hpp"
#include "render/shader_manager.hpp"
#include "resource/material.hpp"
#include "resource/shader.hpp"
#include "shaders/generated/auto_common_reflection.hpp"

namespace wmoge {

    MeshBatchCollector::MeshBatchCollector() {
        m_gfx_driver = Engine::instance()->gfx_driver();
        m_dyn_vbuff  = m_gfx_driver->dyn_vert_buffer();
        m_dyn_ibuff  = m_gfx_driver->dyn_index_buffer();
        m_dyn_ubuff  = m_gfx_driver->dyn_uniform_buffer();
    }

    void MeshBatchCollector::add_batch(const MeshBatch& b) {
        std::lock_guard guard(m_mutex);

        m_batches.push_back(b);
    }
    void MeshBatchCollector::clear() {
        m_batches.clear();
    }

    MeshBatchCompiler::MeshBatchCompiler() {
        Engine* engine   = Engine::instance();
        m_shader_manager = engine->shader_manager();
        m_driver         = engine->gfx_driver();
        m_ctx            = engine->gfx_ctx();
    }

    Status MeshBatchCompiler::compile_batch(const MeshBatch& batch) {
        batch.material->validate();

        const Ref<Shader>&        shader         = batch.material->get_shader();
        const ShaderPipelineState pipeline_state = shader->get_pipeline_state();

        for (int cam_idx = 0; cam_idx < int(m_cameras->get_size()); cam_idx++) {
            if (!batch.cam_mask[cam_idx]) {
                continue;
            }

            const RenderCameraData& camera = m_cameras->data_at(cam_idx);
            RenderView&             view   = m_views[cam_idx];

            if (camera.type != CameraType::Color) {
                continue;
            }

            Ref<GfxUniformBuffer> gfx_draw_call_data = m_driver->make_uniform_buffer(int(sizeof(ShaderCommon::DrawCallData)), GfxMemUsage::GpuLocal, SID("draw_call_data"));
            {
                ShaderCommon::DrawCallData& draw_call_data = *((ShaderCommon::DrawCallData*) m_ctx->map_uniform_buffer(gfx_draw_call_data));
                draw_call_data.Model                       = batch.elements[0].transform.transpose();
                draw_call_data.ModelPrev                   = batch.elements[0].transform_prev.transpose();
                m_ctx->unmap_uniform_buffer(gfx_draw_call_data);
            }

            GfxDescSetResources gfx_resources;
            {
                {
                    auto& r               = gfx_resources.emplace_back();
                    r.first.type          = GfxBindingType::UniformBuffer;
                    r.first.binding       = 0;
                    r.first.array_element = 0;
                    r.second.resource     = gfx_draw_call_data.as<GfxResource>();
                    r.second.offset       = 0;
                    r.second.range        = int(sizeof(ShaderCommon::DrawCallData));
                }
            }
            Ref<GfxDescSet> gfx_set = m_driver->make_desc_set(gfx_resources, SID("draw_call"));

            GfxVertElements gfx_elements;
            gfx_elements.add_vert_attribs(batch.vertex_streams[0], batch.vertex_streams[0], 0, false);

            GfxPipelineState gfx_pso_state;
            gfx_pso_state.shader       = shader->create_variant(batch.vertex_streams, {});
            gfx_pso_state.vert_format  = m_driver->make_vert_format(gfx_elements, SID(""));
            gfx_pso_state.prim_type    = batch.prim_type;
            gfx_pso_state.poly_mode    = pipeline_state.poly_mode;
            gfx_pso_state.cull_mode    = pipeline_state.cull_mode;
            gfx_pso_state.front_face   = pipeline_state.front_face;
            gfx_pso_state.depth_enable = pipeline_state.depth_enable;
            gfx_pso_state.depth_write  = pipeline_state.depth_write;
            gfx_pso_state.depth_func   = pipeline_state.depth_func;
            gfx_pso_state.blending     = false;
            Ref<GfxPipeline> gfx_pso   = m_driver->make_pipeline(gfx_pso_state, SID(""));

            RenderCmd cmd;
            cmd.vert_buffers       = batch.vertex_buffers;
            cmd.index_setup        = batch.index_buffer;
            cmd.desc_sets[0]       = view.view_set.get();
            cmd.desc_sets_slots[0] = 0;
            cmd.desc_sets[1]       = batch.material->get_desc_set().get();
            cmd.desc_sets_slots[1] = 1;
            cmd.desc_sets[2]       = gfx_set.get();
            cmd.desc_sets_slots[2] = 2;
            cmd.pipeline           = gfx_pso.get();
            cmd.call_params        = batch.elements[0].draw_call;

            view.queues[int(MeshPassType::GBuffer)].push(RenderCmdKey(), cmd);

            m_transient_pipeliens.push_back(gfx_pso);
            m_transient_sets.push_back(gfx_set);
        }

        return StatusCode::Ok;
    }
    void MeshBatchCompiler::set_views(ArrayView<struct RenderView> views) {
        m_views = views;
    }
    void MeshBatchCompiler::set_cameras(RenderCameras& cameras) {
        m_cameras = &cameras;
    }
    void MeshBatchCompiler::clear() {
        m_transient_pipeliens.clear();
        m_transient_sets.clear();
    }

}// namespace wmoge