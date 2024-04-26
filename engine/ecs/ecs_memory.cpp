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

#include "ecs_memory.hpp"

#include "ecs/ecs_registry.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    EcsPool::EcsPool(int element_size, int chunk_size, MemPool* pool) {
        m_element_size = element_size;
        m_chunk_size   = chunk_size;
        m_pool         = pool;
    }
    EcsPool::~EcsPool() {
        if (m_pool) {
            for (auto chunk : m_chunks) {
                m_pool->free(chunk);
            }
        }
    }
    void EcsPool::acquire_chunk() {
        WG_AUTO_PROFILE_ECS("EcsPool::acquire_chunk");
        m_chunks.push_back(m_pool->allocate());
    }
    void* EcsPool::get_element_raw(int idx) const {
        return ((std::uint8_t*) m_chunks[idx / m_chunk_size]) + m_element_size * (idx % m_chunk_size);
    }

    EcsArchStorage::EcsArchStorage(EcsArch arch) : m_arch(arch) {
        WG_AUTO_PROFILE_ECS("EcsArchStorage::EcsArchStorage");

        EcsRegistry* registry = Engine::instance()->ecs_registry();

        m_chunk_size  = registry->get_chunk_size();
        m_pool.back() = EcsPool(sizeof(EcsEntity), m_chunk_size, &registry->get_entity_pool());

        m_components_info.fill(nullptr);
        m_arch.for_each_component([&](int idx) {
            m_components_info[idx] = &registry->get_component_info(idx);
            m_pool[idx]            = EcsPool(m_components_info[idx]->size, m_chunk_size, &registry->get_component_pool(idx));
        });
    }

    EcsArchStorage::~EcsArchStorage() {
        WG_AUTO_PROFILE_ECS("EcsArchStorage::~EcsArchStorage");
        clear();
    }

    void EcsArchStorage::make_entity(const EcsEntity& entity, std::uint32_t& out_entity_idx) {
        WG_AUTO_PROFILE_ECS("EcsArchStorage::make_entity");

        assert(entity.is_valid());

        if (m_size == m_capacity) {
            m_capacity += m_chunk_size;
            m_pool.back().acquire_chunk();
            m_arch.for_each_component([&](int component_idx) {
                m_pool[component_idx].acquire_chunk();
            });
        }

        const int entity_idx = m_size;

        assert(m_size < m_capacity);

        *m_pool.back().get_element<EcsEntity>(entity_idx) = entity;
        m_arch.for_each_component([&](int component_idx) {
            m_components_info[component_idx]->create(m_pool[component_idx].get_element_raw(entity_idx));
        });

        out_entity_idx = entity_idx;
        m_size += 1;
    }
    void EcsArchStorage::destroy_entity(const std::uint32_t& in_entity_idx, bool& was_swapped) {
        WG_AUTO_PROFILE_ECS("EcsArchStorage::destroy_entity");

        assert(int(in_entity_idx) < m_size);
        assert(m_size > 0);

        const int entity_idx = int(in_entity_idx);

        assert(m_pool.back().get_element<EcsEntity>(entity_idx)->is_valid());

        const int last_entity = m_size - 1;

        if (m_size > 1 && entity_idx != last_entity) {
            auto& entity_pool = m_pool.back();
            std::swap(*entity_pool.get_element<EcsEntity>(entity_idx),
                      *entity_pool.get_element<EcsEntity>(last_entity));

            m_arch.for_each_component([&](int component_idx) {
                auto& component_pool = m_pool[component_idx];
                m_components_info[component_idx]->swap(component_pool.get_element_raw(entity_idx),
                                                       component_pool.get_element_raw(last_entity));
            });

            was_swapped = true;
        }

        m_arch.for_each_component([&](int component_idx) {
            m_components_info[component_idx]->destroy(m_pool[component_idx].get_element_raw(last_entity));
        });

        m_size -= 1;
    }

    void EcsArchStorage::clear() {
        WG_AUTO_PROFILE_ECS("EcsArchStorage::clear");

        for (int entity_idx = 0; entity_idx < m_size; entity_idx++) {
            *m_pool.back().get_element<EcsEntity>(entity_idx) = EcsEntity();
            m_arch.for_each_component([&](int component_idx) {
                m_components_info[component_idx]->destroy(m_pool[component_idx].get_element_raw(entity_idx));
            });
        }

        m_size = 0;
    }

    void* EcsArchStorage::get_component(int entity_idx, int component_idx) const {
        assert(entity_idx < m_size);
        assert(component_idx < EcsLimits::MAX_COMPONENTS);
        assert(m_components_info[component_idx]);
        assert(m_pool.back().get_element<EcsEntity>(entity_idx)->is_valid());

        return m_pool[component_idx].get_element_raw(entity_idx);
    }
    EcsEntity EcsArchStorage::get_entity(int entity_idx) const {
        assert(entity_idx < m_size);
        assert(m_pool.back().get_element<EcsEntity>(entity_idx)->is_valid());

        return *m_pool.back().get_element<EcsEntity>(entity_idx);
    }

}// namespace wmoge