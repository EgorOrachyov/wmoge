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

#include "ecs_world.hpp"

namespace wmoge {

    EcsEntity EcsWorld::allocate_entity() {
        std::lock_guard guard(m_mutex);

        if (m_entity_pool.empty()) {
            m_entity_pool.emplace_back(std::uint32_t(m_entity_counter++), 0);
        }

        assert(!m_entity_pool.empty());

        const EcsEntity entity = m_entity_pool.front();
        m_entity_pool.pop_front();

        return entity;
    }

    void EcsWorld::make_entity(const EcsEntity& entity, const EcsArch& arch) {
        assert(arch.any());

        m_entity_info.resize(m_entity_counter);

        if (m_arch_to_idx.find(arch) == m_arch_to_idx.end()) {
            const int arch_idx  = int(m_arch_storage.size());
            m_arch_to_idx[arch] = arch_idx;
            m_arch_by_idx.emplace_back(arch);
            m_arch_storage.emplace_back(arch);
        }

        assert(m_arch_to_idx.find(arch) != m_arch_to_idx.end());

        const int arch_idx = m_arch_to_idx[arch];

        EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        entity_info.arch           = arch_idx;
        entity_info.gen            = entity.gen;
        entity_info.state          = EcsEntityState::Alive;

        m_arch_storage[arch_idx].make_entity(entity, entity_info.storage);
    }

    void EcsWorld::destroy_entity(const EcsEntity& entity) {
        assert(entity.is_valid());
        assert(entity.idx < m_entity_info.size());

        EcsEntityInfo& entity_info = m_entity_info[entity.idx];

        m_arch_storage[entity_info.arch].destroy_entity(entity, entity_info.storage);
        m_entity_pool.emplace_back(entity.idx, (entity.gen + 1) % EcsLimits::MAX_GENERATIONS_PER_ARC);

        entity_info = EcsEntityInfo{};
    }

    bool EcsWorld::is_alive(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(entity.idx < m_entity_info.size());

        return m_entity_info[entity.idx].gen <= entity.gen;
    }

    EcsArch EcsWorld::get_arch(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(entity.idx < m_entity_info.size());

        return m_arch_by_idx[m_entity_info[entity.idx].arch];
    }

    CallbackQueue* EcsWorld::queue() {
        return &m_queue;
    }

    void EcsWorld::sync() {
        m_queue.flush();
    }

}// namespace wmoge