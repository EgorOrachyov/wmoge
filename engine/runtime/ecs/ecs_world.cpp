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

#include "core/log.hpp"
#include "core/task.hpp"
#include "core/task_parallel_for.hpp"
#include "math/math_utils.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    EcsWorld::EcsWorld(EcsRegistry* ecs_registry) {
        m_ecs_registry = ecs_registry;
    }

    EcsWorld::~EcsWorld() {
        clear();
    }

    EcsEntity EcsWorld::allocate_entity() {
        WG_PROFILE_CPU_ECS("EcsWorld::allocate_entity");

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
        WG_PROFILE_CPU_ECS("EcsWorld::make_entity");

        assert(entity.is_valid());

        m_entity_info.resize(m_entity_counter);

        register_arch(arch);

        assert(m_arch_to_idx.find(arch) != m_arch_to_idx.end());
        const int arch_idx = m_arch_to_idx[arch];

        EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        entity_info.arch           = arch_idx;
        entity_info.gen            = entity.gen;
        entity_info.state          = EcsEntityState::Alive;

        m_arch_storage[arch_idx]->make_entity(entity, entity_info.storage);
    }

    void EcsWorld::swap_entity(const EcsEntity& left, const EcsEntity& right) {
        WG_PROFILE_CPU_ECS("EcsWorld::swap_entity");

        assert(left.is_valid());
        assert(left.idx < m_entity_info.size());
        assert(right.is_valid());
        assert(right.idx < m_entity_info.size());

        const EcsEntityInfo& right_info = m_entity_info[right.idx];
        const EcsEntityInfo& left_info  = m_entity_info[left.idx];

        EcsArchStorage* right_storage = m_arch_storage[right_info.arch].get();
        EcsArchStorage* left_storage  = m_arch_storage[left_info.arch].get();

        const EcsArch right_arch = m_arch_by_idx[right_info.arch];
        const EcsArch left_arch  = m_arch_by_idx[left_info.arch];

        EcsArch(right_arch & left_arch).for_each_component([&](int idx) {
            const EcsComponentInfo& info = m_ecs_registry->get_component_info(idx);
            info.swap(right_storage->get_component(right_info.storage, idx), left_storage->get_component(left_info.storage, idx));
        });
    }

    void EcsWorld::rearch_entity(const EcsEntity& entity, const EcsArch& new_arch) {
        WG_PROFILE_CPU_ECS("EcsWorld::rearch_entity");

        assert(new_arch.any());
        assert(entity.is_valid());
        assert(entity.idx < m_entity_info.size());

        EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        EcsArch        prev_arch   = m_arch_by_idx[entity_info.arch];

        if (prev_arch == new_arch) {
            return;
        }

        EcsEntity tmp_entity = allocate_entity();
        make_entity(tmp_entity, new_arch);
        swap_entity(tmp_entity, entity);

        EcsEntityInfo& tmp_entity_info = m_entity_info[tmp_entity.idx];
        std::swap(tmp_entity_info.arch, entity_info.arch);
        std::swap(tmp_entity_info.storage, entity_info.storage);

        destroy_entity(tmp_entity);
    }

    void EcsWorld::destroy_entity(const EcsEntity& entity) {
        WG_PROFILE_CPU_ECS("EcsWorld::destroy_entity");

        assert(entity.is_valid());
        assert(entity.idx < m_entity_info.size());

        EcsEntityInfo&  entity_info    = m_entity_info[entity.idx];
        EcsArch         entity_arch    = m_arch_by_idx[entity_info.arch];
        EcsArchStorage* entity_storage = m_arch_storage[entity_info.arch].get();

        for (auto& iter : m_on_destroy) {
            const EcsAccess&         query = iter.first;
            const EcsQueuryFunction& func  = iter.second;
            if (query.match(entity_arch)) {
                EcsQueryContext context(*entity_storage, query, entity_info.storage, 1);
                func(context);
            }
        }

        bool need_swap = false;
        entity_storage->destroy_entity(entity_info.storage, need_swap);

        if (need_swap) {
            const EcsEntity entity_to_swap = m_arch_storage[entity_info.arch]->get_entity(entity_info.storage);
            assert(m_entity_info[entity_to_swap.idx].arch == entity_info.arch);
            m_entity_info[entity_to_swap.idx].storage = entity_info.storage;
        }

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

    void EcsWorld::register_arch(EcsArch arch) {
        if (m_arch_to_idx.find(arch) == m_arch_to_idx.end()) {
            const int arch_idx  = int(m_arch_storage.size());
            m_arch_to_idx[arch] = arch_idx;
            m_arch_by_idx.emplace_back(arch);
            m_arch_storage.emplace_back() = std::make_unique<EcsArchStorage>(m_ecs_registry, arch);
        }
    }

    void EcsWorld::on_destroy(const EcsAccess& access, const EcsQueuryFunction& func) {
        m_on_destroy.emplace_back(access, func);
    }

    void EcsWorld::execute(const EcsAccess& access, const EcsQueuryFunction& func) {
        WG_PROFILE_CPU_ECS("EcsWorld::execute");

        for (int arch_idx = 0; arch_idx < m_arch_storage.size(); arch_idx++) {
            if (!access.match(m_arch_by_idx[arch_idx])) {
                continue;
            }

            EcsArchStorage& storage = *m_arch_storage[arch_idx];
            const int       start   = 0;
            const int       conut   = storage.get_size();

            EcsQueryContext context(storage, access, start, conut);
            func(context);
        }
    }

    Async EcsWorld::execute_async(TaskManager* task_manager, Async depends_on, const EcsAccess& access, const EcsQueuryFunction& func) {
        WG_PROFILE_CPU_ECS("EcsWorld::execute_async");

        Task task(access.name, [access, func, this](TaskContext&) {
            for (int arch_idx = 0; arch_idx < m_arch_storage.size(); arch_idx++) {
                if (!access.match(m_arch_by_idx[arch_idx])) {
                    continue;
                }

                EcsArchStorage& storage = *m_arch_storage[arch_idx];
                const int       start   = 0;
                const int       conut   = storage.get_size();

                EcsQueryContext context(storage, access, start, conut);
                func(context);
            }

            return 0;
        });

        return task.schedule(task_manager, depends_on).as_async();
    }

    Async EcsWorld::execute_parallel(TaskManager* task_manager, Async depends_on, const EcsAccess& access, const EcsQueuryFunction& func) {
        WG_PROFILE_CPU_ECS("EcsWorld::execute_parallel");

        TaskParallelFor task(access.name, [access, func, this](TaskContext&, int batch_id, int batch_count) {
            for (int arch_idx = 0; arch_idx < m_arch_storage.size(); arch_idx++) {
                if (!access.match(m_arch_by_idx[arch_idx])) {
                    continue;
                }

                EcsArchStorage& storage   = *m_arch_storage[arch_idx];
                const auto [start, count] = Math::batch_start_count(storage.get_size(), batch_id, batch_count);

                EcsQueryContext context(storage, access, start, count);
                func(context);
            }

            return 0;
        });

        return task.schedule(task_manager, task_manager->get_num_workers(), 1, depends_on).as_async();
    }

    void EcsWorld::clear() {
        WG_PROFILE_CPU_ECS("EcsWorld::clear");

        m_queue.clear();

        for (std::uint32_t idx = 0; idx < std::uint32_t(m_entity_info.size()); idx++) {
            const EcsEntityInfo& enity_info = m_entity_info[idx];

            if (enity_info.state == EcsEntityState::Alive) {
                EcsEntity entity(idx, enity_info.gen);
                destroy_entity(entity);
            }
        }

        for (std::unique_ptr<EcsArchStorage>& storage : m_arch_storage) {
            assert(storage->get_size() == 0);
            storage->clear();
        }

        m_entity_info.clear();
        m_entity_pool.clear();
        m_entity_counter = 0;
    }

    void EcsWorld::sync() {
        WG_PROFILE_CPU_ECS("EcsWorld::sync");

        m_queue.flush();
    }

}// namespace wmoge