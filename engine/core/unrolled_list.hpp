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

#ifndef WMOGE_UNROLLED_LIST
#define WMOGE_UNROLLED_LIST

#include "core/fast_vector.hpp"
#include "io/yaml.hpp"

#include <cinttypes>
#include <cstddef>
#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class UnrolledList
     * @brief Typed container as well as vector but values stored in chunks with persistent mem address in grow operation
     */
    template<typename T, int NodeCapacity = 128>
    class UnrolledList {
    public:
        static constexpr std::size_t NODE_CAPACITY = NodeCapacity;
        static constexpr std::size_t NODE_SIZE     = sizeof(T[NODE_CAPACITY]);

        UnrolledList()                    = default;
        UnrolledList(const UnrolledList&) = delete;
        UnrolledList(UnrolledList&&)      = delete;
        ~UnrolledList();

        template<typename... TArgs>
        T&   emplace_back(TArgs&&... args);
        void push_back(T&& element);
        void clear();
        void free();
        void reserve(std::size_t size);

        [[nodiscard]] const fast_vector<T*>&  nodes() const { return m_nodes; }
        [[nodiscard]] const fast_vector<int>& sizes() const { return m_nodes_size; }
        [[nodiscard]] std::size_t             size() const { return m_size; }
        [[nodiscard]] bool                    is_empty() const { return m_size == 0; }

        template<typename IterType>
        class Iterator {
        public:
            Iterator(IterType** nodes, const int* nodes_size, std::size_t nodes_count) {
                m_node_curr  = nodes;
                m_node_end   = m_node_curr + nodes_count;
                m_node_sizes = nodes_size;

                if (m_node_curr != m_node_end) {
                    m_elem_curr = *m_node_curr;
                    m_elem_end  = m_elem_curr + *m_node_sizes;
                }
            };

            Iterator(IterType** nodes) {
                m_node_curr = m_node_end = nodes;
            }

            bool operator==(const Iterator& other) const {
                return m_node_curr == other.m_node_curr && m_elem_curr == other.m_elem_curr;
            }
            bool operator!=(const Iterator& other) const {
                return m_node_curr != other.m_node_curr || m_elem_curr != other.m_elem_curr;
            }
            IterType& operator*() {
                return *m_elem_curr;
            }
            void operator++() {
                if (m_elem_curr != m_elem_end) {
                    m_elem_curr++;
                }
                if (m_elem_curr == m_elem_end) {
                    m_elem_curr = nullptr;
                    m_elem_end  = nullptr;

                    if (m_node_curr != m_node_end) {
                        m_node_curr++;
                        m_node_sizes++;
                    }
                    if (m_node_curr != m_node_end) {
                        m_elem_curr = *m_node_curr;
                        m_elem_end  = m_elem_curr + *m_node_sizes;
                    }
                }
            }

        private:
            IterType** m_node_curr  = nullptr;
            IterType** m_node_end   = nullptr;
            const int* m_node_sizes = nullptr;
            IterType*  m_elem_curr  = nullptr;
            IterType*  m_elem_end   = nullptr;
        };

        using iterator       = Iterator<T>;
        using const_iterator = Iterator<const T>;

        iterator       begin() { return iterator(m_nodes.data(), m_nodes_size.data(), m_nodes.size()); }
        iterator       end() { return iterator(m_nodes.data() + m_nodes.size()); }
        const_iterator begin() const { return const_iterator(m_nodes.data(), m_nodes_size.data(), m_nodes.size()); }
        const_iterator end() const { return const_iterator(m_nodes.data() + m_nodes.size()); }

    private:
        void* allocate_back();
        void* allocate_pool();

    private:
        fast_vector<T*>    m_nodes;
        fast_vector<int>   m_nodes_size;
        fast_vector<void*> m_pool;
        std::size_t        m_size = 0;
    };

    template<typename T, int NodeCapacity>
    inline UnrolledList<T, NodeCapacity>::~UnrolledList() {
        free();
    }

    template<typename T, int NodeCapacity>
    template<typename... TArgs>
    inline T& UnrolledList<T, NodeCapacity>::emplace_back(TArgs&&... args) {
        void* mem = allocate_back();
        return *(new (mem) T(std::forward<TArgs...>(args...)));
    }

    template<typename T, int NodeCapacity>
    inline void UnrolledList<T, NodeCapacity>::push_back(T&& element) {
        void* mem = allocate_back();
        new (mem) T(std::forward<T>(element));
    }

    template<typename T, int NodeCapacity>
    inline void UnrolledList<T, NodeCapacity>::clear() {
        for (std::size_t i = 0; i < m_nodes.size(); i++) {
            T*                elements       = m_nodes[i];
            const std::size_t elements_count = m_nodes_size[i];

            for (std::size_t element_idx = 0; element_idx < elements_count; element_idx++) {
                elements[element_idx].~T();
            }

            m_pool.push_back(elements);
        }

        m_nodes.clear();
        m_nodes_size.clear();
        m_size = 0;
    }

    template<typename T, int NodeCapacity>
    inline void UnrolledList<T, NodeCapacity>::free() {
        clear();

        for (void* ptr : m_pool) {
            std::free(ptr);
        }

        m_pool.clear();
    }

    template<typename T, int NodeCapacity>
    inline void UnrolledList<T, NodeCapacity>::reserve(std::size_t size) {
        if (m_size >= size) {
            return;
        }

        const std::size_t required_nodes_count = (size + NODE_CAPACITY - 1) / NODE_CAPACITY;
        m_nodes.reserve(required_nodes_count);
        m_nodes_size.reserve(required_nodes_count);

        for (std::size_t node_idx = m_nodes.size(); node_idx < required_nodes_count; node_idx++) {
            m_nodes.push_back(static_cast<T*>(allocate_pool()));
            m_nodes_size.push_back(0);
        }
    }

    template<typename T, int NodeCapacity>
    inline void* UnrolledList<T, NodeCapacity>::allocate_back() {
        if (m_nodes.empty() || m_nodes_size.back() == NODE_CAPACITY) {
            m_nodes.push_back(static_cast<T*>(allocate_pool()));
            m_nodes_size.push_back(0);
        }

        m_size += 1;
        const int offset = m_nodes_size.back()++;
        return m_nodes.back() + offset;
    }

    template<typename T, int NodeCapacity>
    inline void* UnrolledList<T, NodeCapacity>::allocate_pool() {
        if (m_pool.empty()) {
            m_pool.push_back(std::malloc(NODE_SIZE));
        }

        void* mem = m_pool.back();
        m_pool.pop_back();
        return mem;
    }

    template<typename T, std::size_t NodeCapacity>
    Status yaml_read(const YamlConstNodeRef& node, UnrolledList<T, NodeCapacity>& list) {
        list.reserve(node.num_children());
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            T element;
            WG_YAML_READ(child, element);
            list.push_back(std::move(element));
        }
        return StatusCode::Ok;
    }

    template<typename T, std::size_t NodeCapacity>
    Status yaml_write(YamlNodeRef node, const UnrolledList<T, NodeCapacity>& list) {
        WG_YAML_SEQ(node);
        for (const T& value : list) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(child, value);
        }
        return StatusCode::Ok;
    }

}// namespace wmoge

#endif//WMOGE_UNROLLED_LIST