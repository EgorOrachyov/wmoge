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

#include "render_mesh_static.hpp"

#include <cassert>

namespace wmoge {

    RenderMeshStatic::RenderMeshStatic(Ref<Model> model) {
        assert(model);
        m_model = std::move(model);

        auto& mesh = m_model->get_lods()[0].mesh.get_safe();
        m_factories.emplace_back(mesh->get_gfx_vertex_buffers(), mesh->get_attribs(), get_friendly_name());
        m_factories.back().init();
    }

    void RenderMeshStatic::collect(const RenderCameras& cameras, RenderCameraMask mask, MeshBatchCollector& collector) {
        const int lod_idx = 0;

        const ArrayView<const ResourceRefHard<Material>> materials = m_model->get_materials();
        const ModelLod&                                  lod       = m_model->get_lods()[lod_idx];
        const Ref<Mesh>&                                 mesh      = lod.mesh.get_safe();
        const ArrayView<const MeshChunk>                 chunks    = mesh->get_chunks();

        for (int i = 0; i < int(chunks.size()); i++) {
            const MeshChunk& chunk = chunks[i];

            MeshBatchElement element;
            element.name                = chunk.name;// must be debug only
            element.draw_call.base      = chunk.vertex_offset;
            element.draw_call.count     = chunk.index_count;
            element.draw_call.instances = 1;
            element.transform           = m_transform_l2w;
            element.transform_prev      = m_transform_l2w;// static mesh has no movement and motion

            MeshBatch batch;
            batch.elements[0]             = element;
            batch.vertex_factory          = &m_factories.back();
            batch.index_buffer.buffer     = mesh->get_gfx_index_buffer().get();
            batch.index_buffer.index_type = mesh->get_index_type();
            batch.index_buffer.offset     = chunk.index_offset;
            batch.cam_mask                = mask;
            batch.material                = materials[lod.materials[i]].get_safe().get();
            batch.mesh_params             = nullptr;// todo: custom mesh data
            batch.pass_list               = nullptr;// todo: cache pso list
            batch.object                  = this;
            batch.prim_type               = mesh->get_prim_type();

            collector.add_batch(batch);
        }
    }

    void RenderMeshStatic::update_transform(const Mat4x4f& l2w) {
        m_transform_l2w = l2w;
    }

    bool RenderMeshStatic::has_materials() const {
        return !m_model->get_materials().empty();
    }

    std::optional<Ref<Material>> RenderMeshStatic::get_material() const {
        if (has_materials()) {
            return m_model->get_materials()[0].get_safe();
        }
        return std::nullopt;
    }

    std::vector<Ref<Material>> RenderMeshStatic::get_materials() const {
        std::vector<Ref<Material>>                 result;
        ArrayView<const ResourceRefHard<Material>> materials = m_model->get_materials();

        result.reserve(materials.size());
        for (auto& ref : materials) {
            result.push_back(ref.get_safe());
        }

        return result;
    }

}// namespace wmoge