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

#include "mesh_builder.hpp"

#include "core/class.hpp"
#include "core/data.hpp"
#include "core/log.hpp"

#include <cstring>

namespace wmoge {

    void MeshBuilder::set_mesh(Ref<Mesh> mesh) {
        m_mesh = std::move(mesh);
    }

    void MeshBuilder::add_index(std::uint32_t i) {
        m_indices.push_back(i);
        m_num_indices += 1;
    }

    void MeshBuilder::add_triangle(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2) {
        add_index(v0);
        add_index(v1);
        add_index(v2);
    }

    void MeshBuilder::add_vertex(const MeshVertex& v) {
        const GfxVertAttribs& attribs = v.attribs;

        if (!attribs.bits.any()) {
            WG_LOG_ERROR("passed vertex with no attributes to add");
            return;
        }

        if (attribs.get(GfxVertAttrib::Pos3f)) {
            m_pos3.push_back(v.pos3);
        }
        if (attribs.get(GfxVertAttrib::Pos2f)) {
            m_pos2.push_back(v.pos2);
        }
        if (attribs.get(GfxVertAttrib::Norm3f)) {
            m_norm.push_back(v.norm);
        }
        if (attribs.get(GfxVertAttrib::Tang3f)) {
            m_tang.push_back(v.tang);
        }
        if (attribs.get(GfxVertAttrib::BoneIds4i)) {
            m_bone_ids.push_back(v.bone_ids);
        }
        if (attribs.get(GfxVertAttrib::BoneWeights4f)) {
            m_bone_weights.push_back(v.bone_weights);
        }
        if (attribs.get(GfxVertAttrib::Col04f)) {
            m_col[0].push_back(v.col[0]);
        }
        if (attribs.get(GfxVertAttrib::Col14f)) {
            m_col[1].push_back(v.col[1]);
        }
        if (attribs.get(GfxVertAttrib::Col24f)) {
            m_col[2].push_back(v.col[2]);
        }
        if (attribs.get(GfxVertAttrib::Col34f)) {
            m_col[3].push_back(v.col[3]);
        }
        if (attribs.get(GfxVertAttrib::Uv02f)) {
            m_uv[0].push_back(v.uv[0]);
        }
        if (attribs.get(GfxVertAttrib::Uv12f)) {
            m_uv[1].push_back(v.uv[1]);
        }
        if (attribs.get(GfxVertAttrib::Uv22f)) {
            m_uv[2].push_back(v.uv[2]);
        }
        if (attribs.get(GfxVertAttrib::Uv32f)) {
            m_uv[3].push_back(v.uv[3]);
        }

        m_num_vertices += 1;
    }

    void MeshBuilder::add_chunk(const MeshChunk& chunk) {
        m_chunks.push_back(chunk);
    }

    Status MeshBuilder::build() {
        if (m_num_vertices == 0) {
            WG_LOG_ERROR("no vertices to build");
            return StatusCode::InvalidData;
        }
        if (m_num_indices == 0) {
            WG_LOG_ERROR("no indices to build");
            return StatusCode::InvalidData;
        }
        if (m_chunks.empty()) {
            WG_LOG_ERROR("no chunks to build");
            return StatusCode::InvalidData;
        }

        GfxVertAttribs attribs;

        if (!m_pos3.empty()) {
            attribs.set(GfxVertAttrib::Pos3f);
        }
        if (!m_norm.empty()) {
            attribs.set(GfxVertAttrib::Norm3f);
        }
        if (!m_tang.empty()) {
            attribs.set(GfxVertAttrib::Tang3f);
        }
        if (!m_bone_ids.empty()) {
            attribs.set(GfxVertAttrib::BoneIds4i);
        }
        if (!m_bone_weights.empty()) {
            attribs.set(GfxVertAttrib::BoneWeights4f);
        }
        if (!m_col[0].empty()) {
            attribs.set(GfxVertAttrib::Col04f);
        }
        if (!m_col[1].empty()) {
            attribs.set(GfxVertAttrib::Col14f);
        }
        if (!m_col[2].empty()) {
            attribs.set(GfxVertAttrib::Col24f);
        }
        if (!m_col[3].empty()) {
            attribs.set(GfxVertAttrib::Col34f);
        }
        if (!m_uv[0].empty()) {
            attribs.set(GfxVertAttrib::Uv02f);
        }
        if (!m_uv[1].empty()) {
            attribs.set(GfxVertAttrib::Uv12f);
        }
        if (!m_uv[2].empty()) {
            attribs.set(GfxVertAttrib::Uv22f);
        }
        if (!m_uv[3].empty()) {
            attribs.set(GfxVertAttrib::Uv32f);
        }

        int stride = 0;

        attribs.for_each([&](int i, GfxVertAttrib attrib) {
            stride += GfxVertAttribSizes[i];
        });

        Ref<Data> vert_data = make_ref<Data>(m_num_vertices * stride);
        Ref<Data> ind_data  = make_ref<Data>(m_num_indices * sizeof(std::uint32_t));

        std::memcpy(ind_data->buffer(), m_indices.data(), m_indices.size() * sizeof(std::uint32_t));

        int offset = 0;

        const void* attribs_data[] = {
                m_pos3.data(),
                m_pos2.data(),
                m_norm.data(),
                m_tang.data(),
                m_bone_ids.data(),
                m_bone_weights.data(),
                m_col[0].data(),
                m_col[1].data(),
                m_col[2].data(),
                m_col[3].data(),
                m_uv[0].data(),
                m_uv[1].data(),
                m_uv[2].data(),
                m_uv[3].data()};

        attribs.for_each([&](int i, GfxVertAttrib attrib) {
            const auto* src_ptr = reinterpret_cast<const std::uint8_t*>(attribs_data[i]);
            auto*       dst_ptr = reinterpret_cast<std::uint8_t*>(vert_data->buffer()) + offset;

            for (int vert_id = 0; vert_id < m_num_vertices; vert_id += 1) {
                std::memcpy(dst_ptr, src_ptr, GfxVertAttribSizes[i]);
                src_ptr += GfxVertAttribSizes[i];
                dst_ptr += stride;
            }

            offset += GfxVertAttribSizes[i];
        });

        assert(m_mesh);

        for (const MeshChunk& chunk : m_chunks) {
            m_mesh->add_chunk(chunk);
        }

        m_mesh->set_vertex_params(m_num_vertices, GfxPrimType::Triangles);
        m_mesh->set_vertex_buffer(0, vert_data, attribs);
        m_mesh->set_index_buffer(ind_data, m_num_indices, GfxIndexType::Uint32);
        m_mesh->update_aabb();
        m_mesh->update_gfx_buffers();

        return StatusCode::Ok;
    }

}// namespace wmoge
