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

#include "mem_linear.hpp"

#include "math/math_utils.hpp"

#include <cassert>
#include <cstdlib>
#include <memory>

namespace wmoge {

    MemLinear::MemLinear(std::size_t capacity) {
        assert(capacity);

        m_capacity = capacity;
        m_buffers.push_back(std::malloc(capacity));
    }

    MemLinear::~MemLinear() {
        assert(m_allocated == 0);
        assert(m_allocated_total == 0);

        for (auto mem : m_buffers) {
            std::free(mem);
        }
    }

    void* MemLinear::allocate(std::size_t size, std::size_t alignment) {
        std::lock_guard guard(m_mutex);

        assert(size);
        assert(size < m_capacity);
        assert(alignment);

        std::size_t required = Math::align(size, alignment);

        if (required + m_allocated > m_capacity) {
            m_allocated = 0;
            m_buffers.push_back(std::malloc(m_capacity));
        }

        void* mem = reinterpret_cast<std::uint8_t*>(m_buffers.back()) + m_allocated;
        m_allocated += required;
        m_allocated_total += required;
        return mem;
    }

    void MemLinear::reset() {
        std::lock_guard guard(m_mutex);

        assert(m_buffers.size() >= 1);

        for (std::size_t i = 1; i < m_buffers.size(); i++) {
            std::free(m_buffers[i]);
        }

        m_buffers.resize(1);
        m_allocated       = 0;
        m_allocated_total = 0;
    }

}// namespace wmoge