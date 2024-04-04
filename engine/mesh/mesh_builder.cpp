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

    void MeshBuilder::add_chunk(const Strid& name, const Ref<ArrayMesh>& data) {
        assert(data);

        m_chunks.push_back(data);
        m_chunks_names.push_back(name);
        m_chunks_parents.push_back(-1);
        m_chunks_children.emplace_back();
    }

    void MeshBuilder::add_child(int parent_idx, int child_idx) {
        m_chunks_children[parent_idx].push_back(child_idx);
        m_chunks_parents[child_idx] = parent_idx;
    }

    Status MeshBuilder::build() {
        const GfxVertAttribs        attribs_stream1 = {GfxVertAttrib::Pos3f, GfxVertAttrib::Pos2f, GfxVertAttrib::Norm3f, GfxVertAttrib::Tang3f};
        const GfxVertAttribs        attribs_stream2 = {GfxVertAttrib::BoneIds4i, GfxVertAttrib::BoneWeights4f};
        const GfxVertAttribs        attribs_stream3 = {GfxVertAttrib::Col04f, GfxVertAttrib::Col14f, GfxVertAttrib::Col24f, GfxVertAttrib::Col34f, GfxVertAttrib::Uv02f, GfxVertAttrib::Uv12f, GfxVertAttrib::Uv22f, GfxVertAttrib::Uv32f};
        const GfxVertAttribsStreams stream_masks    = {attribs_stream1, attribs_stream2, attribs_stream3};

        int curr_vert_buffer  = 0;
        int curr_index_buffer = 0;
        int curr_vert_stream  = 0;
        int curr_index_stream = 0;

        const int n_chunks = int(m_chunks.size());

        for (int i = 0; i < n_chunks; i++) {
            Ref<Data>                  vert_data;
            fast_vector<GfxVertStream> vert_streams;
            m_chunks[i]->pack_attribs(stream_masks, vert_data, vert_streams);

            for (auto& vert_stream : vert_streams) {
                vert_stream.buffer = curr_vert_buffer;
            }

            Ref<Data>      index_data;
            GfxIndexStream index_stream;
            m_chunks[i]->pack_faces(index_data, index_stream);

            index_stream.buffer = curr_index_buffer;
            m_mesh->add_intex_stream(index_stream);

            MeshChunk chunk;
            chunk.name               = m_chunks_names[i];
            chunk.aabb               = m_chunks[i]->get_data().aabb;
            chunk.attribs            = m_chunks[i]->get_data().attribs;
            chunk.index_stream       = curr_index_stream;
            chunk.vert_stream_offset = curr_vert_stream;
            chunk.vert_stream_count  = int(vert_streams.size());
            chunk.prim_type          = GfxPrimType::Triangles;
            chunk.elem_count         = m_chunks[i]->get_num_faces() * 3;
            chunk.parent             = m_chunks_parents[i];
            chunk.children           = m_chunks_children[i];
            m_mesh->add_chunk(chunk);

            m_mesh->add_vertex_buffer(vert_data);
            m_mesh->add_index_buffer(index_data);

            for (auto& vert_stream : vert_streams) {
                m_mesh->add_vert_stream(vert_stream);
            }

            curr_index_stream += 1;
            curr_vert_stream += int(vert_streams.size());

            curr_vert_buffer += 1;
            curr_index_buffer += 1;
        }

        m_mesh->update_aabb();
        m_mesh->update_gfx_buffers();

        return StatusCode::Ok;
    }

}// namespace wmoge
