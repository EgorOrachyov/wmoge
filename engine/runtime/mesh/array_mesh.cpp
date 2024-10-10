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

#include "array_mesh.hpp"

#include "io/enum.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cstring>

namespace wmoge {

    void ArrayMesh::add_vertex(const MeshVertex& v) {
        assert((m_data.attribs & v.attribs).bits == m_data.attribs.bits);

        const GfxVertAttribs& attribs = m_data.attribs;

        if (attribs.get(GfxVertAttrib::Pos3f)) {
            m_data.pos3.push_back(v.pos3);
        }
        if (attribs.get(GfxVertAttrib::Pos2f)) {
            m_data.pos2.push_back(v.pos2);
        }
        if (attribs.get(GfxVertAttrib::Norm3f)) {
            m_data.norm.push_back(v.norm);
        }
        if (attribs.get(GfxVertAttrib::Tang3f)) {
            m_data.tang.push_back(v.tang);
        }
        if (attribs.get(GfxVertAttrib::BoneIds4i)) {
            m_data.bone_ids.push_back(v.bone_ids);
        }
        if (attribs.get(GfxVertAttrib::BoneWeights4f)) {
            m_data.bone_weights.push_back(v.bone_weights);
        }
        if (attribs.get(GfxVertAttrib::Col04f)) {
            m_data.col0.push_back(v.col[0]);
        }
        if (attribs.get(GfxVertAttrib::Col14f)) {
            m_data.col1.push_back(v.col[1]);
        }
        if (attribs.get(GfxVertAttrib::Col24f)) {
            m_data.col2.push_back(v.col[2]);
        }
        if (attribs.get(GfxVertAttrib::Col34f)) {
            m_data.col3.push_back(v.col[3]);
        }
        if (attribs.get(GfxVertAttrib::Uv02f)) {
            m_data.uv0.push_back(v.uv[0]);
        }
        if (attribs.get(GfxVertAttrib::Uv12f)) {
            m_data.uv1.push_back(v.uv[1]);
        }
        if (attribs.get(GfxVertAttrib::Uv22f)) {
            m_data.uv2.push_back(v.uv[2]);
        }
        if (attribs.get(GfxVertAttrib::Uv32f)) {
            m_data.uv3.push_back(v.uv[3]);
        }
    }

    void ArrayMesh::add_face(const MeshFace& face) {
        m_data.faces.push_back(face);
    }

    void ArrayMesh::pack_attribs(const GfxVertAttribsStreams& layout, Ref<Data>& buffer, buffered_vector<MeshVertStream>& streams) const {
        const void* attribs_data[] = {
                m_data.pos3.data(),
                m_data.pos2.data(),
                m_data.norm.data(),
                m_data.tang.data(),
                m_data.bone_ids.data(),
                m_data.bone_weights.data(),
                m_data.col0.data(),
                m_data.col1.data(),
                m_data.col2.data(),
                m_data.col3.data(),
                m_data.uv0.data(),
                m_data.uv1.data(),
                m_data.uv2.data(),
                m_data.uv3.data()};

        const std::size_t attribs_size[] = {
                m_data.pos3.size(),
                m_data.pos2.size(),
                m_data.norm.size(),
                m_data.tang.size(),
                m_data.bone_ids.size(),
                m_data.bone_weights.size(),
                m_data.col0.size(),
                m_data.col1.size(),
                m_data.col2.size(),
                m_data.col3.size(),
                m_data.uv0.size(),
                m_data.uv1.size(),
                m_data.uv2.size(),
                m_data.uv3.size()};

        int vert_streams       = 0;
        int vert_buffer_offset = 0;
        int vert_buffer_size   = 0;

        m_data.attribs.for_each([&](int i, GfxVertAttrib attrib) {
            vert_buffer_size += int(attribs_size[i]) * GfxVertAttribSizes[i];
        });

        buffer = make_ref<Data>(vert_buffer_size);

        for (int i = 0; i < layout.size(); i++) {
            if ((m_data.attribs & layout[i]).bits.any()) {
                MeshVertStream stream;
                stream.attribs = m_data.attribs & layout[i];
                stream.offset  = vert_buffer_offset;
                stream.stride  = 0;
                stream.size    = 0;

                stream.attribs.for_each([&](int i, GfxVertAttrib attrib) {
                    stream.stride += GfxVertAttribSizes[i];
                    stream.size += int(attribs_size[i]) * GfxVertAttribSizes[i];
                });

                streams.push_back(stream);

                int offset = vert_buffer_offset;
                stream.attribs.for_each([&](int i, GfxVertAttrib attrib) {
                    const auto* src_ptr = reinterpret_cast<const std::uint8_t*>(attribs_data[i]);
                    auto*       dst_ptr = reinterpret_cast<std::uint8_t*>(buffer->buffer()) + offset;

                    for (int vert_id = 0; vert_id < get_num_vertices(); vert_id += 1) {
                        std::memcpy(dst_ptr, src_ptr, GfxVertAttribSizes[i]);
                        src_ptr += GfxVertAttribSizes[i];
                        dst_ptr += stream.stride;
                    }

                    offset += GfxVertAttribSizes[i];
                });

                vert_buffer_offset += stream.size;
                vert_streams += 1;
            }
        }
    }

    void ArrayMesh::pack_faces(Ref<Data>& buffer, MeshIndexStream& stream) const {
        buffer = make_ref<Data>(get_num_faces() * sizeof(MeshFace));
        std::memcpy(buffer->buffer(), m_data.faces.data(), get_num_faces() * sizeof(MeshFace));

        stream.offset     = 0;
        stream.size       = int(buffer->size());
        stream.index_type = GfxIndexType::Uint32;
    }

}// namespace wmoge