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

#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "memory/mem_pool.hpp"

#include <memory>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class EcsPool
     * @brief Pool for chunks of ecs data
     */
    class EcsPool {
    public:
        EcsPool() = default;
        EcsPool(int element_size, int chunk_size, MemPool* pool);
        ~EcsPool();

        void  acquire_chunk();
        void* get_element_raw(int idx) const;

        template<typename T>
        T* get_element(int idx) const {
            assert(sizeof(T) == m_element_size);
            return (T*) get_element_raw(idx);
        }

    private:
        std::vector<void*> m_chunks;
        MemPool*           m_pool         = nullptr;
        int                m_element_size = 0;
        int                m_chunk_size   = 0;
    };

    /**
     * @class EcsArchStorage
     * @brief Manages entities of a specific single archetype
     */
    class EcsArchStorage {
    public:
        explicit EcsArchStorage(EcsArch arch);
        EcsArchStorage(const EcsArchStorage&) = delete;
        EcsArchStorage(EcsArchStorage&&)      = delete;
        ~EcsArchStorage();

        void make_entity(const EcsEntity& entity, std::uint32_t& entity_idx);
        void destroy_entity(const std::uint32_t& entity_idx, bool& was_swapped);
        void clear();

        template<typename Component>
        [[nodiscard]] Component* get_component(int entity_idx) const;
        [[nodiscard]] void*      get_component(int entity_idx, int component_idx) const;
        [[nodiscard]] EcsEntity  get_entity(int entity_idx) const;

        [[nodiscard]] int get_size() const { return m_size; }
        [[nodiscard]] int get_capacity() const { return m_capacity; }

    private:
        std::array<EcsPool, EcsLimits::MAX_COMPONENTS + 1>             m_pool{};
        std::array<const EcsComponentInfo*, EcsLimits::MAX_COMPONENTS> m_components_info{};

        const EcsArch m_arch;

        int m_chunk_size = 0;// num entities data within single chunk
        int m_size       = 0;// count of allocated entities
        int m_capacity   = 0;// capacity of allocated chunks
    };

    template<typename Component>
    Component* EcsArchStorage::get_component(int entity_idx) const {
        void* component_raw = get_component(entity_idx, Component::IDX);
        return static_cast<Component*>(component_raw);
    }

}// namespace wmoge
