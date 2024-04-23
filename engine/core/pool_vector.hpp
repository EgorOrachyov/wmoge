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

#include "core/buffered_vector.hpp"
#include "io/yaml.hpp"

#include <array>
#include <cinttypes>
#include <cstddef>
#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class pool_vector
     * @brief Typed container as well as vector but values stored in chunks with persistent mem address in grow operation
     */
    template<typename T, int NodeCapacity = 128>
    class pool_vector {
    public:
        static constexpr std::size_t NODE_CAPACITY = NodeCapacity;

        pool_vector() = default;

        pool_vector(const pool_vector& other) {
            reserve(other.size());
            for (const auto& value : other) {
                emplace_back(value);
            }
        }

        pool_vector(pool_vector&& other) {
            m_nodes      = std::move(other.m_nodes);
            m_size       = other.m_size;
            other.m_size = 0;
        }

        ~pool_vector();

        template<typename... TArgs>
        T&   emplace_back(TArgs&&... args);
        void push_back(const T& element);
        void push_back(T&& element);
        void clear();
        void reserve(std::size_t size);

        T&       operator[](std::size_t i);
        const T& operator[](std::size_t i) const;

        [[nodiscard]] std::size_t size() const { return m_size; }
        [[nodiscard]] std::size_t capacity() const { return m_nodes.size() * NODE_CAPACITY; }
        [[nodiscard]] bool        is_empty() const { return m_size == 0; }

        struct Item {
            std::uint8_t mem[sizeof(T)];
        };

        template<typename ItemType>
        struct Node {
            std::array<Item, NodeCapacity> items;
        };

        template<typename ItemType>
        using NodePtr = std::unique_ptr<Node<ItemType>>;

        template<typename IterType>
        class Iterator {
        public:
            Iterator(NodePtr<IterType>* nodes, std::size_t i) {
                m_nodes = nodes;
                m_i     = i;
            };

            bool operator==(const Iterator& other) const {
                return m_i == other.m_i;
            }
            bool operator!=(const Iterator& other) const {
                return m_i != other.m_i;
            }
            IterType& operator*() {
                return (IterType*) m_nodes[m_i / NODE_CAPACITY]->items[m_i % NODE_CAPACITY].mem;
            }
            void operator++() {
                m_i += 1;
            }

        private:
            NodePtr<IterType>* m_nodes = nullptr;
            std::size_t        m_i     = 0;
        };

        using iterator       = Iterator<T>;
        using const_iterator = Iterator<const T>;

        iterator       begin() { return iterator(m_nodes.data(), 0); }
        iterator       end() { return iterator(m_nodes.data(), m_size); }
        const_iterator begin() const { return iterator(m_nodes.data(), 0); }
        const_iterator end() const { return iterator(m_nodes.data(), m_size); }

    private:
        void* allocate_back();

    private:
        buffered_vector<NodePtr<T>> m_nodes;
        std::size_t                 m_size = 0;
    };

    template<typename T, int NodeCapacity>
    inline pool_vector<T, NodeCapacity>::~pool_vector() {
        clear();
    }

    template<typename T, int NodeCapacity>
    template<typename... TArgs>
    inline T& pool_vector<T, NodeCapacity>::emplace_back(TArgs&&... args) {
        void* mem = allocate_back();
        return *(new (mem) T(std::forward<TArgs...>(args...)));
    }

    template<typename T, int NodeCapacity>
    inline void pool_vector<T, NodeCapacity>::push_back(const T& element) {
        void* mem = allocate_back();
        new (mem) T(element);
    }

    template<typename T, int NodeCapacity>
    inline void pool_vector<T, NodeCapacity>::push_back(T&& element) {
        void* mem = allocate_back();
        new (mem) T(std::move(element));
    }

    template<typename T, int NodeCapacity>
    inline void pool_vector<T, NodeCapacity>::clear() {
        for (std::size_t i = 0; i < m_size; i++) {
            std::size_t node_idx = i / NODE_CAPACITY;
            std::size_t item_idx = i % NODE_CAPACITY;
            ((T*) (m_nodes[node_idx]->items[item_idx].mem))->~T();
        }

        m_nodes.clear();
        m_size = 0;
    }

    template<typename T, int NodeCapacity>
    inline void pool_vector<T, NodeCapacity>::reserve(std::size_t size) {
        if (m_size >= size) {
            return;
        }

        const std::size_t required_nodes_count = (size + NODE_CAPACITY - 1) / NODE_CAPACITY;
        m_nodes.reserve(required_nodes_count);
    }

    template<typename T, int NodeCapacity>
    inline T& pool_vector<T, NodeCapacity>::operator[](std::size_t i) {
        std::size_t node_idx = m_size / NODE_CAPACITY;
        std::size_t item_idx = m_size % NODE_CAPACITY;
        return m_nodes[node_idx]->items[item_idx].mem;
    }

    template<typename T, int NodeCapacity>
    inline const T& pool_vector<T, NodeCapacity>::operator[](std::size_t i) const {
        std::size_t node_idx = m_size / NODE_CAPACITY;
        std::size_t item_idx = m_size % NODE_CAPACITY;
        return m_nodes[node_idx]->items[item_idx].mem;
    }

    template<typename T, int NodeCapacity>
    inline void* pool_vector<T, NodeCapacity>::allocate_back() {
        if (capacity() == size()) {
            m_nodes.push_back(std::make_unique<Node<T>>());
        }
        std::size_t node_idx = m_size / NODE_CAPACITY;
        std::size_t item_idx = m_size % NODE_CAPACITY;
        m_size += 1;
        return m_nodes[node_idx]->items[item_idx].mem;
    }

    template<typename T, std::size_t NodeCapacity>
    Status yaml_read(YamlConstNodeRef node, pool_vector<T, NodeCapacity>& vec) {
        vec.reserve(node.num_children());
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            T element;
            WG_YAML_READ(child, element);
            vec.push_back(std::move(element));
        }
        return StatusCode::Ok;
    }

    template<typename T, std::size_t NodeCapacity>
    Status yaml_write(YamlNodeRef node, const pool_vector<T, NodeCapacity>& vec) {
        WG_YAML_SEQ(node);
        for (const T& value : vec) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(child, value);
        }
        return StatusCode::Ok;
    }

}// namespace wmoge