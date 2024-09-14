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

#include <unordered_map>
#include <vector>

namespace wmoge {

    /**
     * @class TypedMap
     * @brief Reference-counted typed map for safe copy and passing into scripting system
     */
    template<typename K, typename V, typename H>
    class TypedMap {
    public:
        using buffer_type    = std::unordered_map<K, V, H>;
        using entry_type     = std::pair<K, V>;
        using list_type      = std::vector<entry_type>;
        using iterator       = typename buffer_type::iterator;
        using const_iterator = typename buffer_type::const_iterator;

        TypedMap() : m_storage(make_ref<Storage>()) {}
        TypedMap(buffer_type buffer) : TypedMap() { store(std::move(buffer)); }
        TypedMap(const TypedMap&) = default;
        TypedMap(TypedMap&&)      = default;
        ~TypedMap()               = default;

        TypedMap& operator=(const TypedMap&) = default;
        TypedMap& operator=(TypedMap&&)      = default;

        V& operator[](const K& key) { return m_storage->buffer[key]; }

        void insert(entry_type entry) { m_storage->buffer.insert(std::move(entry)); }
        void store(buffer_type buffer) { m_storage->buffer = std::move(buffer); }
        void reserve(std::size_t count) { m_storage->buffer.reserve(count); }
        void clear() { m_storage->buffer.clear(); }

        TypedMap<K, V, H> copy() const {
            TypedMap<K, V, H> copied;
            copied.m_storage->buffer = m_storage->buffer;
            return copied;
        }

        void to_list(list_type& list) {
            list.reserve(size());
            for (const auto& entry : *this) {
                list.push_back(entry);
            }
        }

        V&             find_or_insert(const K& key, const V& value) { return *m_storage->buffer.emplace(key, value).first; }
        iterator       find(const K& key) { return m_storage->buffer.find(key); }
        const_iterator find(const K& key) const { return m_storage->buffer.find(key); }
        iterator       begin() { return m_storage->buffer.begin(); }
        const_iterator begin() const { return m_storage->buffer.cbegin(); }
        iterator       end() { return m_storage->buffer.end(); }
        const_iterator end() const { return m_storage->buffer.cend(); }

        [[nodiscard]] std::size_t size() const { return m_storage->buffer.size(); }
        [[nodiscard]] bool        is_empty() const { return m_storage->buffer.empty(); }
        [[nodiscard]] bool        contains(const K& key) const { return find(key) != end(); }

    private:
        struct Storage final : public RefCnt {
            buffer_type buffer;
        };

        Ref<Storage> m_storage;
    };

    static_assert(sizeof(TypedMap<int, int, std::hash<int>>) <= sizeof(void*), "Typed map must fit size of native pointer");

}// namespace wmoge