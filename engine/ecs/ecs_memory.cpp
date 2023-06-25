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

    struct EcsChunk {
        std::array<std::uint8_t*, EcsLimits::MAX_COMPONENTS> components{};
        std::unique_ptr<std::atomic_uint32_t[]>              generations;
        std::unique_ptr<std::atomic_bool[]>                  used;
    };

    EcsArchStorage::EcsArchStorage(EcsArch arch, int arch_idx)
        : m_arch(arch),
          m_arch_idx(arch_idx),
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

    EcsEntity EcsArchStorage::make_entity() {
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

        assert(!chunk.used[entity_idx].load());

        create_components(chunk, entity_idx);
        mark_used(chunk, entity_idx, true);

        EcsEntity entity;
        entity.arch = m_arch_idx;
        entity.idx  = idx;
        entity.gen  = chunk.generations[entity_idx].load();

        assert(entity.is_valid());

        m_free_entity_ids.pop_back();

        return entity;
    }
    void EcsArchStorage::destroy_entity(const EcsEntity& entity) {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const int idx                      = entity.idx;
        const auto [chunk_idx, entity_idx] = get_entity_creds(idx);
        EcsChunk& chunk                    = m_chunks[chunk_idx];

        destroy_components(chunk, entity_idx);
        mark_used(chunk, entity_idx, false);
        update_generation(chunk, entity_idx, entity.gen);

        m_free_entity_ids.push_back(idx);
    }
    bool EcsArchStorage::is_alive(const EcsEntity& entity) const {
        assert(entity.is_valid());

        const auto [chunk_idx, entity_idx] = get_entity_creds(entity.idx);

        assert(chunk_idx < m_chunks.size());
        assert(entity_idx < m_chunk_size);

        const EcsChunk& chunk = m_chunks[chunk_idx];

        assert(chunk.used[entity_idx].load());

        return chunk.generations[entity_idx].load() <= entity.gen;
    }

    void* EcsArchStorage::get_component(const EcsEntity& entity, int idx) {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const auto [chunk_idx, entity_idx] = get_entity_creds(entity.idx);

        assert(chunk_idx < m_chunks.size());
        assert(entity_idx < m_chunk_size);

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
        chunk.generations = std::unique_ptr<std::atomic_uint32_t[]>(new std::atomic_uint32_t[m_chunk_size]);
        chunk.used        = std::unique_ptr<std::atomic_bool[]>(new std::atomic_bool[m_chunk_size]);

        for (int i = 0; i < m_chunk_size; i++) {
            chunk.generations[i].store(0);
            chunk.used[i].store(false);
        }

        m_arch.for_each_component([&](int idx) {
            chunk.components[idx] = static_cast<std::uint8_t*>(m_registry->get_component_pool(idx).allocate());
        });
    }
    void EcsArchStorage::release_chunk(EcsChunk& chunk) {
        auto all_not_used = [&]() {
            for (int i = 0; i < m_chunk_size; i++) {
                if (chunk.used[i].load()) return false;
            }

            return true;
        };

        assert(all_not_used());

        chunk.generations.reset();
        chunk.used.reset();

        m_arch.for_each_component([&](int idx) {
            m_registry->get_component_pool(idx).free(chunk.components[idx]);
        });
    }
    void EcsArchStorage::create_components(EcsChunk& chunk, int entity_idx) {
        m_arch.for_each_component([&](int idx) {
            void* component_raw = chunk.components[idx] + get_component_byte_offset(entity_idx, idx);
            m_components_info[idx]->create(static_cast<EcsComponent*>(component_raw));
        });
    }
    void EcsArchStorage::destroy_components(EcsChunk& chunk, int entity_idx) {
        m_arch.for_each_component([&](int idx) {
            void* component_raw = chunk.components[idx] + get_component_byte_offset(entity_idx, idx);
            m_components_info[idx]->destroy(static_cast<EcsComponent*>(component_raw));
        });
    }
    void EcsArchStorage::mark_used(EcsChunk& chunk, int entity_idx, bool used) {
        chunk.used[entity_idx].store(used);
    }
    void EcsArchStorage::update_generation(EcsChunk& chunk, int entity_idx, std::uint32_t generation) {
        std::uint32_t prev = chunk.generations[entity_idx].fetch_add((generation + 1) % EcsLimits::MAX_GENERATIONS_PER_ARC);
        assert(prev == generation);
    }

}// namespace wmoge