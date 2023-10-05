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

    void MeshBuilder::add_index(std::uint32_t i) {
        indices.push_back(i);
        num_indices += 1;
    }

    bool MeshBuilder::build() {
        if (num_vertices == 0) {
            WG_LOG_ERROR("no vertices to build");
            return false;
        }
        if (num_indices == 0) {
            WG_LOG_ERROR("no indices to build");
            return false;
        }
        if (chunks.empty()) {
            WG_LOG_ERROR("no chunks to build");
            return false;
        }

        GfxVertAttribs attribs;

        if (!pos3.empty()) {
            attribs.set(GfxVertAttrib::Pos3f);
        }
        if (!norm.empty()) {
            attribs.set(GfxVertAttrib::Norm3f);
        }
        if (!tang.empty()) {
            attribs.set(GfxVertAttrib::Tang3f);
        }
        if (!bone_ids.empty()) {
            attribs.set(GfxVertAttrib::BoneIds4i);
        }
        if (!bone_weights.empty()) {
            attribs.set(GfxVertAttrib::BoneWeights4f);
        }
        if (!col[0].empty()) {
            attribs.set(GfxVertAttrib::Col04f);
        }
        if (!col[1].empty()) {
            attribs.set(GfxVertAttrib::Col14f);
        }
        if (!col[2].empty()) {
            attribs.set(GfxVertAttrib::Col24f);
        }
        if (!col[3].empty()) {
            attribs.set(GfxVertAttrib::Col34f);
        }
        if (!uv[0].empty()) {
            attribs.set(GfxVertAttrib::Uv02f);
        }
        if (!uv[1].empty()) {
            attribs.set(GfxVertAttrib::Uv12f);
        }
        if (!uv[2].empty()) {
            attribs.set(GfxVertAttrib::Uv22f);
        }
        if (!uv[3].empty()) {
            attribs.set(GfxVertAttrib::Uv32f);
        }

        int stride = 0;

        attribs.for_each([&](int i, GfxVertAttrib attrib) {
            stride += GfxVertAttribSizes[i];
        });

        Ref<Data> vert_data = make_ref<Data>(num_vertices * stride);
        Ref<Data> ind_data  = make_ref<Data>(num_indices * sizeof(std::uint32_t));

        std::memcpy(ind_data->buffer(), indices.data(), indices.size() * sizeof(std::uint32_t));

        int offset = 0;

        const void* attribs_data[] = {
                pos3.data(),
                pos2.data(),
                norm.data(),
                tang.data(),
                bone_ids.data(),
                bone_weights.data(),
                col[0].data(),
                col[1].data(),
                col[2].data(),
                col[3].data(),
                uv[0].data(),
                uv[1].data(),
                uv[2].data(),
                uv[3].data()};

        attribs.for_each([&](int i, GfxVertAttrib attrib) {
            const auto* src_ptr = reinterpret_cast<const std::uint8_t*>(attribs_data[i]);
            auto*       dst_ptr = reinterpret_cast<std::uint8_t*>(vert_data->buffer()) + offset;

            for (int vert_id = 0; vert_id < num_vertices; vert_id += 1) {
                std::memcpy(dst_ptr, src_ptr, GfxVertAttribSizes[i]);
                src_ptr += GfxVertAttribSizes[i];
                dst_ptr += stride;
            }

            offset += GfxVertAttribSizes[i];
        });

        assert(mesh);

        for (const MeshChunk& chunk : chunks) {
            mesh->add_chunk(chunk);
        }

        mesh->set_vertex_params(num_vertices, GfxPrimType::Triangles);
        mesh->set_vertex_buffer(0, vert_data, attribs);
        mesh->set_index_buffer(ind_data, num_indices, GfxIndexType::Uint32);
        mesh->update_aabb();
        mesh->update_gfx_buffers();

        return true;
    }

}// namespace wmoge
