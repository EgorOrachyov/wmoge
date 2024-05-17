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

#ifndef WMOGE_MEM_LINEAR_HPP
#define WMOGE_MEM_LINEAR_HPP

#include "core/buffered_vector.hpp"

#include <mutex>

namespace wmoge {

    /**
     * @class MemLinear
     * @brief Linear memory allocator with free at once semantic
     */
    class MemLinear {
    public:
        static const std::size_t DEFAULT_CAPACITY  = 1024 * 1024;
        static const std::size_t DEFAULT_ALIGNMENT = 16;

        explicit MemLinear(std::size_t capacity = DEFAULT_CAPACITY);
        ~MemLinear();

        void* allocate(std::size_t size, std::size_t alignment = DEFAULT_ALIGNMENT);
        void  reset();

    private:
        buffered_vector<void*> m_buffers;
        std::size_t            m_capacity;
        std::size_t            m_allocated       = 0;
        std::size_t            m_allocated_total = 0;
        std::mutex             m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_MEM_LINEAR_HPP
