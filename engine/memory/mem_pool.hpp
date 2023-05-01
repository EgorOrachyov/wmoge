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

#ifndef WMOGE_MEM_POOL_HPP
#define WMOGE_MEM_POOL_HPP

#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class MemPool
     * @brief Pool of free memory blocks of equal size
     */
    class MemPool {
    public:
        static const std::size_t EXPAND_SIZE = 16;

        explicit MemPool(std::size_t chunk_size, std::size_t expand_size = EXPAND_SIZE);
        ~MemPool();

        void* allocate();
        void  free(void* mem);
        void  reset();

    private:
        std::vector<void*> m_buffers;
        std::vector<void*> m_free;
        std::size_t        m_allocated = 0;
        std::size_t        m_chunk_size;
        std::size_t        m_expand_size;
        std::mutex         m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_MEM_POOL_HPP
