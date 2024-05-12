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

#pragma once

#include "svector.hpp"

#include <cassert>
#include <cstddef>
#include <vector>

namespace wmoge {

    /**
     * @class array_view
     * @brief View to elements of array
     *
     * @tparam T Type of elements
     */
    template<typename T>
    class array_view {
    public:
        array_view() = default;
        array_view(T* data, std::size_t size) : m_data(data), m_size(size) {}
        array_view(std::vector<T>& vector) : m_data(vector.data()), m_size(vector.size()) {}
        template<typename M>
        array_view(const std::vector<M>& vector) : m_data(vector.data()), m_size(vector.size()) {}
        template<std::size_t MinCapacity>
        array_view(ankerl::svector<T, MinCapacity>& vector) : m_data(vector.data()), m_size(vector.size()) {}
        template<typename M, std::size_t MinCapacity>
        array_view(const ankerl::svector<M, MinCapacity>& vector) : m_data(vector.data()), m_size(vector.size()) {}

        T& operator[](const std::size_t i) {
            assert(i < size());
            assert(m_data);
            return m_data[i];
        }
        const T& operator[](const std::size_t i) const {
            assert(i < size());
            assert(m_data);
            return m_data[i];
        }

        [[nodiscard]] std::size_t size() const { return m_size; }
        [[nodiscard]] T*          data() { return m_data; }
        [[nodiscard]] const T*    data() const { return m_data; }
        [[nodiscard]] bool        empty() const { return !m_size; }

        const T* begin() const { return m_data; }
        const T* end() const { return m_data + m_size; }
        T*       begin() { return m_data; }
        T*       end() { return m_data + m_size; }

    private:
        T*          m_data = nullptr;
        std::size_t m_size = 0;
    };

}// namespace wmoge