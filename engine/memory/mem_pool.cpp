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

#include "mem_pool.hpp"

#include <cassert>
#include <cstdlib>
#include <memory>

namespace wmoge {

    MemPool::MemPool(std::size_t chunk_size, std::size_t expand_size) {
        assert(chunk_size);
        assert(expand_size);

        m_chunk_size  = chunk_size;
        m_expand_size = expand_size;
    }

    MemPool::~MemPool() {
        assert(m_allocated == 0);

        for (auto mem : m_buffers) {
            std::free(mem);
        }
    }

    void* MemPool::allocate() {
        std::lock_guard guard(m_mutex);

        if (m_free.empty()) {
            m_buffers.push_back(std::malloc(m_chunk_size * m_expand_size));
            std::uint8_t* buffer = reinterpret_cast<std::uint8_t*>(m_buffers.back());

            for (std::size_t i = 0; i < m_expand_size; i++) {
                m_free.push_back(buffer + i * m_chunk_size);
            }
        }

        void* mem = m_free.back();
        m_free.pop_back();
        m_allocated += 1;
        return mem;
    }

    void MemPool::free(void* mem) {
        std::lock_guard guard(m_mutex);
        assert(m_allocated > 0);

        m_allocated -= 1;
        m_free.push_back(mem);
    }

    void MemPool::reset() {
        std::lock_guard guard(m_mutex);
        m_allocated = 0;
        m_free.clear();
        for (auto mem : m_buffers) {
            std::uint8_t* buffer = reinterpret_cast<std::uint8_t*>(mem);

            for (std::size_t i = 0; i < m_expand_size; i++) {
                m_free.push_back(buffer + i * m_chunk_size);
            }
        }
    }

}// namespace wmoge