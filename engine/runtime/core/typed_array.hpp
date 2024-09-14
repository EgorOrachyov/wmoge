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

#include "core/ref.hpp"
#include "io/property_tree.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class TypedArray
     * @brief Reference-counted typed array for safe copy and passing into scripting system
     */
    template<typename T>
    class TypedArray {
    public:
        using buffer_type    = std::vector<T>;
        using iterator       = typename buffer_type::iterator;
        using const_iterator = typename buffer_type::const_iterator;

        TypedArray() : m_storage(make_ref<Storage>()) {}
        TypedArray(buffer_type buffer) : TypedArray() { store(std::move(buffer)); }
        TypedArray(const TypedArray&) = default;
        TypedArray(TypedArray&&)      = default;
        ~TypedArray()                 = default;

        TypedArray& operator=(const TypedArray&) = default;
        TypedArray& operator=(TypedArray&&)      = default;

        T&       operator[](std::size_t i) { return m_storage->buffer[i]; }
        const T& operator[](std::size_t i) const { return m_storage->buffer[i]; }

        T&       get(std::size_t i) { return m_storage->buffer[i]; }
        const T& get(std::size_t i) const { return m_storage->buffer[i]; }
        void     set(std::size_t i, const T& value) { m_storage->buffer[i] = value; }
        void     move(std::size_t i, T&& value) { m_storage->buffer[i] = std::move(value); }
        void     store(buffer_type buffer) { m_storage->buffer = std::move(buffer); }
        void     resize(std::size_t count) { m_storage->buffer.resize(count); }
        void     clear() { m_storage->buffer.clear(); }

        TypedArray<T> copy() const {
            TypedArray<T> copied;
            copied.m_storage->buffer = m_storage->buffer;
            return copied;
        }

        iterator       begin() { return m_storage->buffer.begin(); }
        const_iterator begin() const { return m_storage->buffer.cbegin(); }
        iterator       end() { return m_storage->buffer.end(); }
        const_iterator end() const { return m_storage->buffer.cend(); }

        [[nodiscard]] std::size_t size() const { return m_storage->buffer.size(); }
        [[nodiscard]] bool        is_empty() const { return m_storage->buffer.empty(); }
        [[nodiscard]] T*          data() { return m_storage->buffer.data(); }
        [[nodiscard]] const T*    data() const { return m_storage->buffer.data(); }

    private:
        struct Storage final : public RefCnt {
            buffer_type buffer;
        };

        Ref<Storage> m_storage;
    };

    static_assert(sizeof(TypedArray<int>) <= sizeof(void*), "Typed array must fit size of native pointer");

}// namespace wmoge