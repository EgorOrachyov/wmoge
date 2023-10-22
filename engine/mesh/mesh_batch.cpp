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
#include "mesh/mesh_bucket.hpp"
#include "mesh/mesh_processors.hpp"
#include "render/render_engine.hpp"
#include "render/render_scene.hpp"
#include "render/shader_manager.hpp"
#include "render/vertex_factory.hpp"
#include "resource/material.hpp"
#include "resource/shader.hpp"

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
        Engine* engine = Engine::instance();

        m_shader_manager = engine->shader_manager();
        m_driver         = engine->gfx_driver();
        m_ctx            = engine->gfx_ctx();

        // Register pass processors here
        m_processors[int(MeshPassType::GBuffer)] = std::make_unique<MeshPassProcessorGBuffer>();
    }

    Status MeshBatchCompiler::compile_batch(const MeshBatch& batch, int batch_index) {
        if (!batch.cam_mask.any()) {
            return StatusCode::Ok;
        }

        batch.material->validate();

        const Ref<Shader>&        shader           = batch.material->get_shader();
        const ShaderPipelineState pipeline_state   = shader->get_pipeline_state();
        const int                 primitive_id     = batch.object->get_primitive_id();
        const bool                supports_merging = batch.vertex_factory->get_type_info().supports_merging;

        for (int cam_idx = 0; cam_idx < int(m_cameras->get_size()); cam_idx++) {
            if (!batch.cam_mask[cam_idx]) {
                continue;
            }

            const RenderCameraData&  camera    = m_cameras->data_at(cam_idx);
            const MeshPassRelevance& relevance = camera.pass_relevance;
            RenderView&              view      = m_views[cam_idx];

            for (int pass_id = 0; pass_id < MESH_PASSES_TOTAL; pass_id++) {
                const MeshPassType pass_type = static_cast<MeshPassType>(pass_id);

                if (!relevance.get(pass_type)) {
                    continue;
                }
                if (!m_processors[pass_id] || !m_processors[pass_id]->filter(batch)) {
                    continue;
                }

                Ref<GfxPipeline> gfx_pso;

                if (batch.pass_list && batch.pass_list->has_pass(pass_type)) {
                    gfx_pso = batch.pass_list->get_pass(pass_type).value();
                }
                if (!gfx_pso) {
                    Status result = m_processors[pass_id]->compile(batch, gfx_pso);

                    if (result.is_error()) {
                        WG_LOG_ERROR("failed to compile pass " << m_processors[pass_id]->get_name() << " for batch=" << batch_index);
                        continue;
                    }

                    if (batch.pass_list) {
                        batch.pass_list->add_pass(gfx_pso, pass_type);
                    }
                }

                RenderCmd cmd;
                cmd.index_setup        = batch.index_buffer;
                cmd.desc_sets[0]       = view.view_set.get();
                cmd.desc_sets_slots[0] = 0;
                cmd.desc_sets[1]       = batch.material->get_desc_set().get();
                cmd.desc_sets_slots[1] = 1;
                cmd.pipeline           = gfx_pso.get();
                cmd.call_params        = batch.elements[0].draw_call;
                cmd.primitive_buffer   = 0;

                batch.vertex_factory->fill_setup(VertexInputType::Default, cmd.vert_buffers, cmd.primitive_buffer);

                RenderCmdKey cmd_key;
                RenderCmd*   final_cmd;
                int          bucket_slot = -1;

                if (cmd.call_params.instances == 1 && supports_merging) {
                    MeshBucketMap& bucket_map = m_scene->get_bucket_map(pass_type);
                    bucket_map.add_for_instancing(cmd, final_cmd, bucket_slot);
                    cmd_key.value = (std::uint64_t(bucket_slot) << 32u) | std::uint64_t(batch.dist);
                } else {
                    final_cmd     = m_cmd_allocator->allocate();
                    *final_cmd    = cmd;
                    cmd_key.value = (std::uint64_t(0) << 32u) | std::uint64_t(batch.dist);
                }

                SortableRenderCmd sortable_cmd;
                sortable_cmd.cmd          = final_cmd;
                sortable_cmd.cmd_key      = cmd_key;
                sortable_cmd.bucket_slot  = bucket_slot;
                sortable_cmd.primitive_id = primitive_id;

                view.queues[pass_id].push(sortable_cmd);
            }
        }

        return StatusCode::Ok;
    }
    void MeshBatchCompiler::set_scene(RenderScene* scene) {
        m_scene = scene;
    }
    void MeshBatchCompiler::set_views(ArrayView<struct RenderView> views) {
        m_views = views;
    }
    void MeshBatchCompiler::set_cameras(RenderCameras& cameras) {
        m_cameras = &cameras;
    }
    void MeshBatchCompiler::set_cmd_allocator(RenderCmdAllocator& allocator) {
        m_cmd_allocator = &allocator;
    }
    void MeshBatchCompiler::clear() {
    }

}// namespace wmoge