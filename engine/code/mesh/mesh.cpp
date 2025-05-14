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
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Mesh::~Mesh() {
        if (m_callback) {
            (*m_callback)(this);
        }
    }

    Mesh::Mesh(MeshDesc&& desc)
        : m_desc(std::move(desc)) {
    }

    void Mesh::set_mesh_callback(CallbackRef callback) {
        m_callback = std::move(callback);
    }
    void Mesh::set_gfx_vertex_buffers(std::vector<Ref<GfxVertBuffer>> gfx_vertex_buffers) {
        m_gfx_vertex_buffers = std::move(gfx_vertex_buffers);
    }
    void Mesh::set_gfx_index_buffers(std::vector<Ref<GfxIndexBuffer>> gfx_index_buffers) {
        m_gfx_index_buffers = std::move(gfx_index_buffers);
    }

    void Mesh::release_gfx_buffers() {
        m_gfx_vertex_buffers.clear();
        m_gfx_index_buffers.clear();
    }

    GfxVertBuffersSetup Mesh::get_vert_buffers_setup(int chunk_id) const {
        GfxVertBuffersSetup setup;
        const MeshChunk&    chunk = get_chunk(chunk_id);
        for (int i = 0; i < chunk.vert_stream_count; i++) {
            const MeshVertStream& stream = m_desc.vert_streams[chunk.vert_stream_offset + i];
            setup.buffers[i]             = m_gfx_vertex_buffers[stream.buffer].get();
            setup.offsets[i]             = stream.offset;
        }
        return setup;
    }
    GfxIndexBufferSetup Mesh::get_index_buffer_setup(int chunk_id) const {
        GfxIndexBufferSetup setup;
        const MeshChunk&    chunk = get_chunk(chunk_id);
        if (chunk.index_stream != -1) {
            const MeshIndexStream& stream = m_desc.index_streams[chunk.index_stream];
            setup.buffer                  = m_gfx_index_buffers[stream.buffer].get();
            setup.offset                  = stream.offset;
            setup.index_type              = stream.index_type;
        }
        return setup;
    }

    const MeshChunk& Mesh::get_chunk(int i) const {
        assert(i < m_desc.chunks.size());
        return m_desc.chunks[i];
    }
    const Ref<GfxVertBuffer>& Mesh::get_gfx_vertex_buffer(int i) const {
        assert(i < m_gfx_vertex_buffers.size());
        return m_gfx_vertex_buffers[i];
    }
    const Ref<GfxIndexBuffer>& Mesh::get_gfx_index_buffer(int i) const {
        assert(i < m_gfx_vertex_buffers.size());
        return m_gfx_index_buffers[i];
    }
    const MeshVertStream& Mesh::get_vert_stream(int i) const {
        assert(i < m_desc.vert_streams.size());
        return m_desc.vert_streams[i];
    }
    const MeshIndexStream& Mesh::get_index_stream(int i) const {
        assert(i < m_desc.index_streams.size());
        return m_desc.index_streams[i];
    }

}// namespace wmoge