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

#include "core/string_utils.hpp"
#include "gfx/gfx_driver.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    WG_IO_BEGIN(MeshChunk)
    WG_IO_FIELD(name);
    WG_IO_FIELD(aabb);
    WG_IO_FIELD(attribs);
    WG_IO_FIELD(prim_type);
    WG_IO_FIELD(elem_count);
    WG_IO_FIELD(vert_stream_offset);
    WG_IO_FIELD(vert_stream_count);
    WG_IO_FIELD(index_stream);
    WG_IO_FIELD(parent);
    WG_IO_FIELD(children);
    WG_IO_END(MeshChunk)

    WG_IO_BEGIN(MeshFile)
    WG_IO_FIELD(chunks);
    WG_IO_FIELD(vertex_buffers);
    WG_IO_FIELD(index_buffers);
    WG_IO_FIELD(vert_streams);
    WG_IO_FIELD(index_streams);
    WG_IO_FIELD(roots);
    WG_IO_FIELD(aabb);
    WG_IO_END(MeshFile)

    void Mesh::add_chunk(const MeshChunk& mesh_chunk) {
        if (mesh_chunk.parent == -1) {
            m_roots.push_back(int(m_chunks.size()));
        }
        m_chunks.push_back(mesh_chunk);
    }
    void Mesh::add_vertex_buffer(Ref<Data> buffer) {
        m_vertex_buffers.push_back(std::move(buffer));
    }
    void Mesh::add_index_buffer(Ref<Data> buffer) {
        m_index_buffers.push_back(std::move(buffer));
    }
    void Mesh::add_vert_stream(const GfxVertStream& stream) {
        m_vert_streams.push_back(stream);
    }
    void Mesh::add_intex_stream(const GfxIndexStream& stream) {
        m_index_streams.push_back(stream);
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
        // auto* engine     = Engine::instance();
        // auto* gfx_driver = engine->gfx_driver();
        // auto* gfx_ctx    = engine->gfx_ctx();

        GfxMemUsage mem_usage = GfxMemUsage::GpuLocal;

        m_gfx_vertex_buffers.resize(m_vertex_buffers.size());

        for (int i = 0; i < m_vertex_buffers.size(); ++i) {
            const int   size = static_cast<int>(m_vertex_buffers[i]->size());
            const Strid name = SID(get_name().str() + "_" + StringUtils::from_int(i));
            // m_gfx_vertex_buffers[i] = gfx_driver->make_vert_buffer(size, mem_usage, name);
            // gfx_ctx->update_vert_buffer(m_gfx_vertex_buffers[i], 0, size, m_vertex_buffers[i]);
        }

        m_gfx_index_buffers.resize(m_index_buffers.size());

        for (int i = 0; i < m_index_buffers.size(); ++i) {
            const int   size = static_cast<int>(m_index_buffers[i]->size());
            const Strid name = SID(get_name().str() + "_" + StringUtils::from_int(i));
            // m_gfx_index_buffers[i] = gfx_driver->make_index_buffer(size, mem_usage, name);
            // gfx_ctx->update_index_buffer(m_gfx_index_buffers[i], 0, size, m_index_buffers[i]);
        }
    }

    GfxVertBuffersSetup Mesh::get_vert_buffers_setup(int chunk_id) const {
        GfxVertBuffersSetup setup;
        const MeshChunk&    chunk = get_chunk(chunk_id);
        for (int i = 0; i < chunk.vert_stream_count; i++) {
            const GfxVertStream& stream = m_vert_streams[chunk.vert_stream_offset + i];
            setup.buffers[i]            = m_gfx_vertex_buffers[stream.buffer].get();
            setup.offsets[i]            = stream.offset;
        }
        return setup;
    }
    GfxIndexBufferSetup Mesh::get_index_buffer_setup(int chunk_id) const {
        GfxIndexBufferSetup setup;
        const MeshChunk&    chunk = get_chunk(chunk_id);
        if (chunk.index_stream != -1) {
            const GfxIndexStream& stream = m_index_streams[chunk.index_stream];
            setup.buffer                 = m_gfx_index_buffers[stream.buffer].get();
            setup.offset                 = stream.offset;
            setup.index_type             = stream.index_type;
        }
        return setup;
    }

    array_view<const MeshChunk> Mesh::get_chunks() const {
        return m_chunks;
    }
    const MeshChunk& Mesh::get_chunk(int i) const {
        assert(i < m_chunks.size());
        return m_chunks[i];
    }
    const Ref<GfxVertBuffer>& Mesh::get_gfx_vertex_buffers(int i) const {
        assert(i < m_gfx_vertex_buffers.size());
        return m_gfx_vertex_buffers[i];
    }
    const Ref<GfxIndexBuffer>& Mesh::get_gfx_index_buffers(int i) const {
        assert(i < m_gfx_vertex_buffers.size());
        return m_gfx_index_buffers[i];
    }
    const GfxVertStream& Mesh::get_vert_streams(int i) const {
        assert(i < m_gfx_vertex_buffers.size());
        return m_vert_streams[i];
    }
    const GfxIndexStream& Mesh::get_index_streams(int i) const {
        assert(i < m_gfx_vertex_buffers.size());
        return m_index_streams[i];
    }
    array_view<const int> Mesh::get_roots() const {
        return m_roots;
    }
    Aabbf Mesh::get_aabb() const {
        return m_aabb;
    }

}// namespace wmoge