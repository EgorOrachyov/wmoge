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
#include "debug/profiler.hpp"
#include "gfx/gfx_driver.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, MeshImportOptions::Process& process) {
        WG_YAML_READ_AS_OPT(node, "triangulate", process.triangulate);
        WG_YAML_READ_AS_OPT(node, "tangent_space", process.tangent_space);
        WG_YAML_READ_AS_OPT(node, "flip_uv", process.flip_uv);
        WG_YAML_READ_AS_OPT(node, "gen_normals", process.gen_normals);
        WG_YAML_READ_AS_OPT(node, "gen_smooth_normals", process.gen_smooth_normals);
        WG_YAML_READ_AS_OPT(node, "join_identical_vertices", process.join_identical_vertices);
        WG_YAML_READ_AS_OPT(node, "limit_bone_weights", process.limit_bone_weights);
        WG_YAML_READ_AS_OPT(node, "improve_cache_locality", process.improve_cache_locality);
        WG_YAML_READ_AS_OPT(node, "sort_by_ptype", process.sort_by_ptype);
        WG_YAML_READ_AS_OPT(node, "gen_uv", process.gen_uv);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const MeshImportOptions::Process& process) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "triangulate", process.triangulate);
        WG_YAML_WRITE_AS(node, "tangent_space", process.tangent_space);
        WG_YAML_WRITE_AS(node, "flip_uv", process.flip_uv);
        WG_YAML_WRITE_AS(node, "gen_normals", process.gen_normals);
        WG_YAML_WRITE_AS(node, "gen_smooth_normals", process.gen_smooth_normals);
        WG_YAML_WRITE_AS(node, "join_identical_vertices", process.join_identical_vertices);
        WG_YAML_WRITE_AS(node, "limit_bone_weights", process.limit_bone_weights);
        WG_YAML_WRITE_AS(node, "improve_cache_locality", process.improve_cache_locality);
        WG_YAML_WRITE_AS(node, "sort_by_ptype", process.sort_by_ptype);
        WG_YAML_WRITE_AS(node, "gen_uv", process.gen_uv);

        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, MeshImportOptions& options) {
        WG_YAML_READ_AS(node, "source_file", options.source_file);
        WG_YAML_READ_AS(node, "attributes", options.attributes);
        WG_YAML_READ_AS_OPT(node, "process", options.process);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const MeshImportOptions& options) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "source_file", options.source_file);
        WG_YAML_WRITE_AS(node, "attributes", options.attributes);
        WG_YAML_WRITE_AS(node, "process", options.process);

        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, MeshChunk& chunk) {
        WG_YAML_READ_AS(node, "aabb", chunk.aabb);
        WG_YAML_READ_AS(node, "name", chunk.name);
        WG_YAML_READ_AS(node, "vertex_offset", chunk.vertex_offset);
        WG_YAML_READ_AS(node, "index_offset", chunk.index_offset);
        WG_YAML_READ_AS(node, "index_count", chunk.index_count);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const MeshChunk& chunk) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "aabb", chunk.aabb);
        WG_YAML_WRITE_AS(node, "name", chunk.name);
        WG_YAML_WRITE_AS(node, "vertex_offset", chunk.vertex_offset);
        WG_YAML_WRITE_AS(node, "index_offset", chunk.index_offset);
        WG_YAML_WRITE_AS(node, "index_count", chunk.index_count);

        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, MeshFile& file) {
        WG_YAML_READ_AS(node, "chunks", file.chunks);
        WG_YAML_READ_AS(node, "index_type", file.index_type);
        WG_YAML_READ_AS(node, "prim_type", file.prim_type);
        WG_YAML_READ_AS(node, "attribs", file.attribs);
        WG_YAML_READ_AS(node, "num_vertices", file.num_vertices);
        WG_YAML_READ_AS(node, "num_indices", file.num_indices);
        WG_YAML_READ_AS(node, "aabb", file.aabb);
        WG_YAML_READ_AS(node, "vertex_buffers", file.vertex_buffers);
        WG_YAML_READ_AS(node, "index_buffer", file.index_buffer);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const MeshFile& file) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "chunks", file.chunks);
        WG_YAML_WRITE_AS(node, "index_type", file.index_type);
        WG_YAML_WRITE_AS(node, "prim_type", file.prim_type);
        WG_YAML_WRITE_AS(node, "attribs", file.attribs);
        WG_YAML_WRITE_AS(node, "num_vertices", file.num_vertices);
        WG_YAML_WRITE_AS(node, "num_indices", file.num_indices);
        WG_YAML_WRITE_AS(node, "aabb", file.aabb);
        WG_YAML_WRITE_AS(node, "vertex_buffers", file.vertex_buffers);
        WG_YAML_WRITE_AS(node, "index_buffer", file.index_buffer);

        return StatusCode::Ok;
    }

    void Mesh::add_chunk(const MeshChunk& mesh_chunk) {
        m_chunks.push_back(mesh_chunk);
    }
    void Mesh::set_vertex_params(int num_vertices, GfxPrimType prim_type) {
        m_num_vertices = num_vertices;
        m_prim_type    = prim_type;
    }
    void Mesh::set_vertex_buffer(int index, Ref<Data> buffer, GfxVertAttribs attribs) {
        assert(index < GfxLimits::MAX_VERT_BUFFERS);
        assert(buffer);
        m_vertex_buffers[index] = std::move(buffer);
        m_attribs[index]        = attribs;
    }
    void Mesh::set_index_buffer(Ref<Data> buffer, int num_indices, GfxIndexType index_type) {
        assert(buffer);
        m_num_indices  = num_indices;
        m_index_type   = index_type;
        m_index_buffer = std::move(buffer);
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
        auto* engine     = Engine::instance();
        auto* gfx_driver = engine->gfx_driver();
        auto* gfx_ctx    = engine->gfx_ctx();

        GfxMemUsage mem_usage = GfxMemUsage::GpuLocal;

        for (int i = 0; i < MAX_BUFFER; ++i) {
            if (m_vertex_buffers[i]) {
                int      size           = static_cast<int>(m_vertex_buffers[i]->size());
                StringId name           = SID(get_name().str() + "_vert" + std::to_string(i));
                m_gfx_vertex_buffers[i] = gfx_driver->make_vert_buffer(size, mem_usage, name);
                gfx_ctx->update_vert_buffer(m_gfx_vertex_buffers[i], 0, size, m_vertex_buffers[i]);
            }
        }

        if (m_index_buffer) {
            int      size      = static_cast<int>(m_index_buffer->size());
            StringId name      = SID(get_name().str() + "_index");
            m_gfx_index_buffer = gfx_driver->make_index_buffer(size, mem_usage, name);
            gfx_ctx->update_index_buffer(m_gfx_index_buffer, 0, size, m_index_buffer);
        }
    }

    ArrayView<const MeshChunk> Mesh::get_chunks() const {
        return m_chunks;
    }
    const MeshChunk& Mesh::get_chunk(int i) const {
        assert(i < m_chunks.size());
        return m_chunks[i];
    }
    const Ref<Data>& Mesh::get_vertex_buffer(int i) const {
        assert(i < MAX_BUFFER);
        return m_vertex_buffers[i];
    }
    const Ref<Data>& Mesh::get_index_buffer() const {
        return m_index_buffer;
    }
    const Ref<GfxVertBuffer>& Mesh::get_gfx_vertex_buffer(int i) const {
        assert(i < MAX_BUFFER);
        return m_gfx_vertex_buffers[i];
    }
    const std::array<Ref<GfxVertBuffer>, Mesh::MAX_BUFFER>& Mesh::get_gfx_vertex_buffers() const {
        return m_gfx_vertex_buffers;
    }
    const Ref<GfxIndexBuffer>& Mesh::get_gfx_index_buffer() const {
        return m_gfx_index_buffer;
    }
    GfxIndexType Mesh::get_index_type() const {
        return m_index_type;
    }
    GfxPrimType Mesh::get_prim_type() const {
        return m_prim_type;
    }
    GfxVertAttribsStreams Mesh::get_attribs() const {
        return m_attribs;
    }
    int Mesh::get_num_vertices() const {
        return m_num_vertices;
    }
    int Mesh::get_num_indices() const {
        return m_num_indices;
    }
    Aabbf Mesh::get_aabb() const {
        return m_aabb;
    }
    GfxVertBuffersSetup Mesh::get_gfx_vert_buffes_setup() const {
        GfxVertBuffersSetup setup;
        for (int i = 0; i < MAX_BUFFER; i++) {
            setup.buffers[i] = m_gfx_vertex_buffers[i].get();
            setup.offsets[i] = 0;
        }
        return setup;
    }

    void Mesh::register_class() {
        auto* cls = Class::register_class<Mesh>();
    }

}// namespace wmoge