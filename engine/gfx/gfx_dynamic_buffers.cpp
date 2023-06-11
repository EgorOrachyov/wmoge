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

#include "gfx/gfx_dynamic_buffers.hpp"

#include "core/engine.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "math/math_utils.hpp"

#include <cassert>

namespace wmoge {

    GfxDynBuffer::GfxDynBuffer(int size, int alignment, const StringId& name) {
        m_default_chunk_size = size;
        m_alignment          = alignment;
    }
    GfxDynAllocation<GfxBuffer, void> GfxDynBuffer::allocate_base(int bytes_to_allocate) {
        std::lock_guard guard(m_mutex);

        const int required_size = int(Math::align(bytes_to_allocate, m_alignment));

        do {
            if (m_current_chunk >= m_chunks.size()) {
                auto& chunk = m_chunks.emplace_back();

                int size = int(Math::round_to_pow2(Math::max(m_default_chunk_size, required_size)));

                chunk.buffer = make_buffer(size, SID(m_name.str() + "-" + StringUtils::from_int(m_current_chunk)));
                chunk.offset = 0;
            }
            if (!m_chunks[m_current_chunk].mapping) {
                auto& chunk = m_chunks[m_current_chunk];

                chunk.mapping = map_buffer(chunk.buffer);
                assert(chunk.mapping);
            }
            if (m_chunks[m_current_chunk].offset + required_size <= m_chunks[m_current_chunk].buffer->size()) {
                auto& chunk = m_chunks[m_current_chunk];

                GfxDynAllocation<GfxBuffer, void> allocation{};
                allocation.buffer = chunk.buffer.get();
                allocation.offset = chunk.offset;
                allocation.ptr    = (std::uint8_t*) chunk.mapping + chunk.offset;

                chunk.offset += required_size;

                return allocation;
            }
            m_current_chunk += 1;
        } while (true);
    }
    void GfxDynBuffer::flush() {
        std::lock_guard guard(m_mutex);

        if (m_current_chunk < m_chunks.size()) {
            for (int i = 0; i <= m_current_chunk; i++) {
                auto& chunk = m_chunks[i];

                unmap_buffer(chunk.buffer);
                chunk.mapping = nullptr;
            }

            m_current_chunk += 1;
        }
    }
    void GfxDynBuffer::recycle() {
        std::lock_guard guard(m_mutex);

        m_current_chunk = 0;

        int pool_size = 0;
        for (auto& chunk : m_chunks) {
            chunk.offset  = 0;
            chunk.mapping = nullptr;
            pool_size += chunk.buffer->size();
        }

        const int recycle_size = int(Math::round_to_pow2(pool_size));
        if (pool_size > 0 && recycle_size > pool_size) {
            m_chunks.clear();

            auto& chunk  = m_chunks.emplace_back();
            chunk.buffer = make_buffer(recycle_size, SID(m_name.str() + "-" + StringUtils::from_int(m_current_chunk)));
            chunk.offset = 0;
        }
    }

    GfxDynVertBuffer::GfxDynVertBuffer(int size, int alignment, const StringId& name) : GfxDynBuffer(size, alignment, name) {
    }
    Ref<GfxBuffer> GfxDynVertBuffer::make_buffer(int size, const StringId& name) {
        return Engine::instance()->gfx_driver()->make_vert_buffer(size, GfxMemUsage::GpuLocal, name);
    }
    void* GfxDynVertBuffer::map_buffer(const Ref<GfxBuffer>& buffer) {
        return Engine::instance()->gfx_ctx()->map_vert_buffer(buffer.cast<GfxVertBuffer>());
    }
    void GfxDynVertBuffer::unmap_buffer(const Ref<GfxBuffer>& buffer) {
        Engine::instance()->gfx_ctx()->unmap_vert_buffer(buffer.cast<GfxVertBuffer>());
    }

    GfxDynIndexBuffer::GfxDynIndexBuffer(int size, int alignment, const StringId& name) : GfxDynBuffer(size, alignment, name) {
    }
    Ref<GfxBuffer> GfxDynIndexBuffer::make_buffer(int size, const StringId& name) {
        return Engine::instance()->gfx_driver()->make_vert_buffer(size, GfxMemUsage::GpuLocal, name);
    }
    void* GfxDynIndexBuffer::map_buffer(const Ref<GfxBuffer>& buffer) {
        return Engine::instance()->gfx_ctx()->map_index_buffer(buffer.cast<GfxIndexBuffer>());
    }
    void GfxDynIndexBuffer::unmap_buffer(const Ref<GfxBuffer>& buffer) {
        Engine::instance()->gfx_ctx()->unmap_index_buffer(buffer.cast<GfxIndexBuffer>());
    }

    GfxDynUniformBuffer::GfxDynUniformBuffer(int size, int alignment, const StringId& name) : GfxDynBuffer(size, alignment, name) {
    }
    Ref<GfxBuffer> GfxDynUniformBuffer::make_buffer(int size, const StringId& name) {
        return Engine::instance()->gfx_driver()->make_uniform_buffer(size, GfxMemUsage::GpuLocal, name);
    }
    void* GfxDynUniformBuffer::map_buffer(const Ref<GfxBuffer>& buffer) {
        return Engine::instance()->gfx_ctx()->map_uniform_buffer(buffer.cast<GfxUniformBuffer>());
    }
    void GfxDynUniformBuffer::unmap_buffer(const Ref<GfxBuffer>& buffer) {
        Engine::instance()->gfx_ctx()->unmap_uniform_buffer(buffer.cast<GfxUniformBuffer>());
    }

}// namespace wmoge