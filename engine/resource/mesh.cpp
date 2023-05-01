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

#include "mesh.hpp"

#include "core/engine.hpp"
#include "gfx/gfx_driver.hpp"

namespace wmoge {

    void Mesh::add_chunk(const MeshChunk& mesh_chunk) {
        m_chunks.push_back(mesh_chunk);
    }
    void Mesh::set_vertex_params(int num_vertices, GfxPrimType prim_type) {
        m_num_vertices = num_vertices;
        m_prim_type    = prim_type;
    }
    void Mesh::set_vertex_buffer(int index, ref_ptr<Data> buffer) {
        assert(index < GfxLimits::MAX_VERT_BUFFERS);
        m_vertex_buffers[index] = std::move(buffer);
    }
    void Mesh::set_index_params(int num_indices, GfxIndexType index_type) {
        m_num_indices = num_indices;
        m_index_type  = index_type;
    }
    void Mesh::set_index_buffer(ref_ptr<Data> buffer) {
        m_index_buffer = std::move(buffer);
    }
    void Mesh::set_attribs(MeshAttribs attribs) {
        m_attribs = attribs;

        for (int i = 0; i < MAX_BUFFER; i++) {
            m_buffers_strides[i] = 0;
            int start, end;
            get_buffer_attribs(i, start, end);
            for (int j = start; j < end; j++) {
                MeshAttrib attrib = static_cast<MeshAttrib>(j);
                if (attribs.get(attrib)) {
                    m_buffers_strides[i] += get_attrib_size(attrib);
                }
            }
        }

        for (int i = 0; i < MAX_ATTRIB; i++) {
            MeshAttrib attrib = static_cast<MeshAttrib>(i);
            if (!attribs.get(attrib)) {
                m_attributes_offsets[i] = -1;
                continue;
            }
            int start, end;
            get_buffer_attribs(get_attrib_buffer(attrib), start, end);
            for (int j = start; j < end; j++) {
                MeshAttrib iter = static_cast<MeshAttrib>(j);
                if (attribs.get(iter)) {
                    if (iter == attrib) {
                        break;
                    }
                    m_attributes_offsets[i] += get_attrib_size(iter);
                }
            }
        }
    }

    void Mesh::update_aabb() {
        Aabbf aabb;

        if (!m_chunks.empty()) {
            aabb = m_chunks.front().aabb;
            for (int i = 1; i < m_chunks.size(); ++i) {
                aabb = aabb.join(m_chunks[i].aabb);
            }
        }

        m_aabb = aabb;
    }

    void Mesh::update_gfx_buffers() {
        auto* engine = Engine::instance();
        auto* gfx    = engine->gfx_driver();

        GfxMemUsage mem_usage = GfxMemUsage::GpuLocal;

        for (int i = 0; i < MAX_BUFFER; ++i) {
            if (m_vertex_buffers[i]) {
                int      size           = static_cast<int>(m_vertex_buffers[i]->size());
                StringId name           = SID(get_name().str() + "_vert" + std::to_string(i));
                m_gfx_vertex_buffers[i] = gfx->make_vert_buffer(size, mem_usage, name);
                gfx->update_vert_buffer(m_gfx_vertex_buffers[i], 0, size, m_vertex_buffers[i]);
            }
        }

        if (m_index_buffer) {
            int      size      = static_cast<int>(m_index_buffer->size());
            StringId name      = SID(get_name().str() + "_index");
            m_gfx_index_buffer = gfx->make_index_buffer(size, mem_usage, name);
            gfx->update_index_buffer(m_gfx_index_buffer, 0, size, m_index_buffer);
        }
    }

    const std::vector<MeshChunk>& Mesh::get_chunks() {
        return m_chunks;
    }
    const MeshChunk& Mesh::get_chunk(int i) {
        assert(i < m_chunks.size());
        return m_chunks[i];
    }
    const ref_ptr<Data>& Mesh::get_vertex_buffer(int i) {
        assert(i < MAX_BUFFER);
        return m_vertex_buffers[i];
    }
    const ref_ptr<Data>& Mesh::get_index_buffer() {
        return m_index_buffer;
    }
    const ref_ptr<GfxVertBuffer>& Mesh::get_gfx_vertex_buffer(int i) {
        assert(i < MAX_BUFFER);
        return m_gfx_vertex_buffers[i];
    }
    const ref_ptr<GfxIndexBuffer>& Mesh::get_gfx_index_buffer() {
        return m_gfx_index_buffer;
    }
    int Mesh::get_buffer_stride(int buffer) {
        return m_buffers_strides[buffer];
    }
    int Mesh::get_attrib_offset(MeshAttrib attrib) {
        return m_attributes_offsets[static_cast<int>(attrib)];
    }
    GfxIndexType Mesh::get_index_type() {
        return m_index_type;
    }
    GfxPrimType Mesh::get_prim_type() {
        return m_prim_type;
    }
    MeshAttribs Mesh::get_attribs() {
        return m_attribs;
    }
    int Mesh::get_num_vertices() {
        return m_num_vertices;
    }
    int Mesh::get_num_indices() {
        return m_num_indices;
    }
    Aabbf Mesh::get_aabb() {
        return m_aabb;
    }

    void Mesh::get_buffer_attribs(int buffer, int& start, int& stop) {
        start = -1;
        stop  = -1;
        switch (buffer) {
            case 0:
                start = 0;
                stop  = BUFF0_MAX_ATTRIB;
                break;
            case 1:
                start = BUFF0_MAX_ATTRIB;
                stop  = BUFF1_MAX_ATTRIB;
                break;
            case 2:
                start = BUFF1_MAX_ATTRIB;
                stop  = BUFF2_MAX_ATTRIB;
                break;
            default:
                return;
        }
    }
    int Mesh::get_attrib_size(MeshAttrib attrib) {
        switch (attrib) {
            case MeshAttrib::Position:
            case MeshAttrib::Normal:
            case MeshAttrib::Tangent:
                return 12;
            case MeshAttrib::BoneIds:
            case MeshAttrib::BoneWeights:
                return 16;
            case MeshAttrib::Color0:
            case MeshAttrib::Color1:
            case MeshAttrib::Color2:
            case MeshAttrib::Color3:
                return 16;
            case MeshAttrib::Uv0:
            case MeshAttrib::Uv1:
            case MeshAttrib::Uv2:
            case MeshAttrib::Uv3:
                return 8;
            default:
                return 0;
        }
    }
    int Mesh::get_attrib_buffer(MeshAttrib attrib) {
        switch (attrib) {
            case MeshAttrib::Position:
            case MeshAttrib::Normal:
            case MeshAttrib::Tangent:
                return 0;
            case MeshAttrib::BoneIds:
            case MeshAttrib::BoneWeights:
                return 1;
            case MeshAttrib::Color0:
            case MeshAttrib::Color1:
            case MeshAttrib::Color2:
            case MeshAttrib::Color3:
            case MeshAttrib::Uv0:
            case MeshAttrib::Uv1:
            case MeshAttrib::Uv2:
            case MeshAttrib::Uv3:
                return 2;
            default:
                return -1;
        }
    }

    ref_ptr<Mesh> Mesh::create_cube(const Vec3f& size) {
        Vec3f size_fact = size * 0.25f;

        Vec3f pos[] = {
                Vec3f(-1, 1, 1),
                Vec3f(-1, -1, 1),
                Vec3f(1, -1, 1),
                Vec3f(1, 1, 1),
                Vec3f(-1, 1, -1),
                Vec3f(-1, -1, -1),
                Vec3f(1, -1, -1),
                Vec3f(1, 1, -1)};

        Vec3i triangles[] = {
                Vec3i(0, 1, 2),
                Vec3i(2, 3, 0),
                Vec3i(3, 2, 6),
                Vec3i(6, 7, 3),
                Vec3i(7, 6, 5),
                Vec3i(5, 4, 7),
                Vec3i(4, 5, 1),
                Vec3i(1, 0, 4),
                Vec3i(4, 0, 3),
                Vec3i(3, 7, 4),
                Vec3i(1, 5, 6),
                Vec3i(6, 2, 1)};

        std::vector<Vec3f>         vertex_data;
        std::vector<std::uint16_t> index_data;
        std::uint16_t              index = 0;

        for (int i = 0; i < 12; i++) {
            int v0 = triangles[i].x();
            int v1 = triangles[i].y();
            int v2 = triangles[i].z();

            Vec3f n = Vec3f::cross(pos[v1] - pos[v0], pos[v2] - pos[v0]).normalized();

            vertex_data.push_back(pos[v0] * size_fact);
            vertex_data.push_back(n);
            vertex_data.push_back(pos[v1] * size_fact);
            vertex_data.push_back(n);
            vertex_data.push_back(pos[v2] * size_fact);
            vertex_data.push_back(n);

            index_data.push_back(index++);
            index_data.push_back(index++);
            index_data.push_back(index++);
        }

        MeshChunk chunk;
        chunk.name         = SID("main");
        chunk.index_count  = index;
        chunk.index_offset = 0;

        ref_ptr<Mesh> mesh = make_ref<Mesh>();
        mesh->add_chunk(chunk);
        mesh->set_name(SID("cube"));
        //mesh->set_vertex_buffer(0, Data::make(vertex_data.data(), vertex_data.size() * sizeof(Vec3f)));
        //mesh->set_index_buffer(Data::make(index_data.data(), index_data.size() * sizeof(std::uint16_t)));
        mesh->update_aabb();
        mesh->update_gfx_buffers();

        return mesh;
    }
    ref_ptr<Mesh> Mesh::create_sphere(const Vec3f& size) {

        std::vector<Vec3f>         vertex_data;
        std::vector<std::uint16_t> index_data;
        std::uint16_t              index = 0;

        MeshChunk chunk;
        chunk.name         = SID("main");
        chunk.index_count  = index;
        chunk.index_offset = 0;

        ref_ptr<Mesh> mesh = make_ref<Mesh>();
        mesh->add_chunk(chunk);
        mesh->set_name(SID("sphere"));
        //mesh->set_vertex_buffer(0, Data::make(vertex_data.data(), vertex_data.size() * sizeof(Vec3f)));
        //mesh->set_index_buffer(Data::make(index_data.data(), index_data.size() * sizeof(std::uint16_t)));
        mesh->update_aabb();
        mesh->update_gfx_buffers();

        return mesh;
    }

    void Mesh::register_class() {
        auto* cls = Class::register_class<Mesh>();
    }

}// namespace wmoge