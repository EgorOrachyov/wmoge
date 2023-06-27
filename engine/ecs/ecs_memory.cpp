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

#include "core/engine.hpp"
#include "ecs/ecs_registry.hpp"

namespace wmoge {

    EcsArchStorage::EcsArchStorage(EcsArch arch)
        : m_arch(arch),
          m_registry(Engine::instance()->ecs_registry()) {

        m_chunk_size = m_registry->get_chunk_size();

        m_components_info.fill(nullptr);
        m_components_size.fill(-1);

        m_arch.for_each_component([&](int idx) {
            m_components_info[idx] = &m_registry->get_component_info(idx);
            m_components_size[idx] = m_components_info[idx]->size;
        });
    }

    EcsArchStorage::~EcsArchStorage() {
        for (auto& chunk : m_chunks) {
            release_chunk(chunk);
        }
    }

    void EcsArchStorage::make_entity(const EcsEntity& entity, std::uint32_t& storage_idx) {
        assert(entity.is_valid());

        if (m_free_entity_ids.empty()) {
            const int start_id = int(m_chunks.size()) * m_chunk_size;
            const int end_id   = start_id + m_chunk_size;

            for (int i = end_id - 1; i >= start_id; i--) {
                m_free_entity_ids.push_back(i);
            }

            m_chunks.emplace_back();
            allocate_chunk(m_chunks.back());
        }

        assert(!m_free_entity_ids.empty());

        const int idx                      = m_free_entity_ids.back();
        const auto [chunk_idx, entity_idx] = get_entity_creds(idx);
        EcsChunk& chunk                    = m_chunks[chunk_idx];

        place(chunk, entity_idx, entity);

        m_free_entity_ids.pop_back();
        storage_idx = entity_idx;
    }
    void EcsArchStorage::destroy_entity(const EcsEntity& entity, const std::uint32_t& storage_idx) {
        assert(entity.is_valid());

        const int idx                      = int(storage_idx);
        const auto [chunk_idx, entity_idx] = get_entity_creds(idx);
        EcsChunk& chunk                    = m_chunks[chunk_idx];

        destroy(chunk, entity_idx, entity);

        m_free_entity_ids.push_back(idx);
    }

    void* EcsArchStorage::get_component(int storage_idx, int idx) const {
        const auto [chunk_idx, entity_idx] = get_entity_creds(storage_idx);

        assert(chunk_idx < m_chunks.size());
        assert(entity_idx < m_chunk_size);
        assert(m_chunks[chunk_idx].entity[entity_idx].is_valid());

        return m_chunks[chunk_idx].components[idx] + get_component_byte_offset(entity_idx, idx);
    }

    std::pair<int, int> EcsArchStorage::get_entity_creds(int idx) const {
        return {idx / m_chunk_size, idx % m_chunk_size};
    }
    int EcsArchStorage::get_component_byte_offset(int entity_idx, int component_idx) const {
        return entity_idx * m_components_size[component_idx];
    }
    void EcsArchStorage::allocate_chunk(EcsChunk& chunk) {
        chunk.components.fill(nullptr);
        chunk.entity.resize(m_chunk_size, EcsEntity{});

        m_arch.for_each_component([&](int idx) {
            chunk.components[idx] = static_cast<std::uint8_t*>(m_registry->get_component_pool(idx).allocate());
        });
    }
    void EcsArchStorage::release_chunk(EcsChunk& chunk) {
        auto all_not_used = [&]() {
            for (int i = 0; i < m_chunk_size; i++) {
                if (chunk.entity[i].is_valid()) return false;
            }

            return true;
        };

        assert(all_not_used());

        m_arch.for_each_component([&](int idx) {
            m_registry->get_component_pool(idx).free(chunk.components[idx]);
        });
    }

    void EcsArchStorage::place(struct EcsChunk& chunk, int entity_idx, const EcsEntity& entity) {
        assert(chunk.entity[entity_idx].is_invalid());

        chunk.entity[entity_idx] = entity;

        m_arch.for_each_component([&](int idx) {
            void* component_raw = chunk.components[idx] + get_component_byte_offset(entity_idx, idx);
            m_components_info[idx]->create(static_cast<EcsComponent*>(component_raw));
        });
    }
    void EcsArchStorage::destroy(struct EcsChunk& chunk, int entity_idx, const EcsEntity& entity) {
        assert(chunk.entity[entity_idx].is_valid());
        assert(chunk.entity[entity_idx] == entity);

        chunk.entity[entity_idx] = EcsEntity{};

        m_arch.for_each_component([&](int idx) {
            void* component_raw = chunk.components[idx] + get_component_byte_offset(entity_idx, idx);
            m_components_info[idx]->destroy(static_cast<EcsComponent*>(component_raw));
        });
    }

}// namespace wmoge