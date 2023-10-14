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

#include "core/engine.hpp"
#include "core/log.hpp"
#include "core/task.hpp"
#include "core/task_parallel_for.hpp"
#include "debug/profiler.hpp"
#include "math/math_utils.hpp"

namespace wmoge {

    EcsWorld::EcsWorld() {
        m_task_manager = Engine::instance()->task_manager();
    }

    EcsEntity EcsWorld::allocate_entity() {
        WG_AUTO_PROFILE_ECS("EcsWorld::allocate_entity");

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
        WG_AUTO_PROFILE_ECS("EcsWorld::make_entity");

        assert(arch.any());

        m_entity_info.resize(m_entity_counter);

        if (m_arch_to_idx.find(arch) == m_arch_to_idx.end()) {
            const int arch_idx  = int(m_arch_storage.size());
            m_arch_to_idx[arch] = arch_idx;
            m_arch_by_idx.emplace_back(arch);
            m_arch_storage.emplace_back() = std::make_unique<EcsArchStorage>(arch);

            for (EcsSystemInfo& system_info : m_systems) {
                auto filter = system_info.query.affected();

                if ((filter & arch) == filter) {
                    system_info.filtered_arch.push_back(arch_idx);
                }
            }
        }

        assert(m_arch_to_idx.find(arch) != m_arch_to_idx.end());

        const int arch_idx = m_arch_to_idx[arch];

        EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        entity_info.arch           = arch_idx;
        entity_info.gen            = entity.gen;
        entity_info.state          = EcsEntityState::Alive;

        m_arch_storage[arch_idx]->make_entity(entity, entity_info.storage);
    }

    void EcsWorld::destroy_entity(const EcsEntity& entity) {
        WG_AUTO_PROFILE_ECS("EcsWorld::destroy_entity");

        assert(entity.is_valid());
        assert(entity.idx < m_entity_info.size());

        EcsEntityInfo& entity_info = m_entity_info[entity.idx];

        m_arch_storage[entity_info.arch]->destroy_entity(entity_info.storage);
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

    std::vector<int> EcsWorld::filter_arch_idx(const EcsQuery& query) {
        WG_AUTO_PROFILE_ECS("EcsWorld::filter_arch_idx");

        std::vector<int> filtered_arch;
        const auto       filter = query.affected();

        for (int arch_idx = 0; arch_idx < m_arch_storage.size(); arch_idx++) {
            if ((filter & m_arch_by_idx[arch_idx]) == filter) {
                filtered_arch.push_back(arch_idx);
            }
        }

        return filtered_arch;
    }

    void EcsWorld::register_system(const std::shared_ptr<EcsSystem>& system) {
        WG_AUTO_PROFILE_ECS("EcsWorld::register_system");

        assert(system);
        assert(m_system_to_idx.find(system->get_name()) == m_system_to_idx.end());

        m_system_to_idx[system->get_name()] = int(m_systems.size());

        EcsSystemInfo& system_info = m_systems.emplace_back();
        system_info.query          = system->get_query();
        system_info.system         = system;
        system_info.exec_mode      = system->get_exec_mode();
        system_info.filtered_arch  = filter_arch_idx(system->get_query());
    }

    void EcsWorld::execute_system(const std::shared_ptr<EcsSystem>& system) {
        WG_AUTO_PROFILE_ECS("EcsWorld::execute_system");

        assert(system);
        assert(m_system_to_idx.find(system->get_name()) != m_system_to_idx.end());

        EcsSystemInfo& system_info = m_systems[m_system_to_idx[system->get_name()]];

        switch (system_info.exec_mode) {
            case EcsSystemExecMode::OnMain: {
                for (const int arch_idx : system_info.filtered_arch) {
                    EcsArchStorage& storage      = *m_arch_storage[arch_idx];
                    const int       size         = storage.get_size();
                    const int       start_entity = 0;
                    const int       count        = size;

                    system_info.system->process_batch(*this, storage, start_entity, count);
                }

                break;
            }

            case EcsSystemExecMode::OnWorkers: {
                TaskParallelFor task(system->get_name(), [&](TaskContext&, int batch_id, int batch_count) {
                    for (const int arch_idx : system_info.filtered_arch) {
                        EcsArchStorage& storage          = *m_arch_storage[arch_idx];
                        const int       size             = storage.get_size();
                        const auto [start_entity, count] = Math::batch_start_count(size, batch_id, batch_count);

                        system_info.system->process_batch(*this, storage, start_entity, count);
                    }
                    return 0;
                });

                task.schedule(m_task_manager->get_num_workers(), 1).wait_completed();

                break;
            }

            default:
                WG_LOG_ERROR("unknown system exec mode");
        }
    }

    void EcsWorld::execute_system(EcsSystem& system) {
        WG_AUTO_PROFILE_ECS("EcsWorld::execute_system");

        const std::vector<int> filtered_arch = filter_arch_idx(system.get_query());

        switch (system.get_exec_mode()) {
            case EcsSystemExecMode::OnMain: {
                for (const int arch_idx : filtered_arch) {
                    EcsArchStorage& storage      = *m_arch_storage[arch_idx];
                    const int       size         = storage.get_size();
                    const int       start_entity = 0;
                    const int       count        = size;

                    system.process_batch(*this, storage, start_entity, count);
                }

                break;
            }

            case EcsSystemExecMode::OnWorkers: {
                TaskParallelFor task(system.get_name(), [&](TaskContext&, int batch_id, int batch_count) {
                    for (const int arch_idx : filtered_arch) {
                        EcsArchStorage& storage          = *m_arch_storage[arch_idx];
                        const int       size             = storage.get_size();
                        const auto [start_entity, count] = Math::batch_start_count(size, batch_id, batch_count);

                        system.process_batch(*this, storage, start_entity, count);
                    }
                    return 0;
                });

                task.schedule(m_task_manager->get_num_workers(), 1).wait_completed();

                break;
            }

            default:
                WG_LOG_ERROR("unknown system exec mode");
        }
    }

    void EcsWorld::each(const EcsQuery& query, const std::function<void(EcsEntity)>& func) {
        WG_AUTO_PROFILE_ECS("EcsWorld::each");

        const auto filter = query.affected();

        for (int arch_idx = 0; arch_idx < m_arch_storage.size(); arch_idx++) {
            if ((filter & m_arch_by_idx[arch_idx]) == filter) {
                EcsArchStorage& storage      = *m_arch_storage[arch_idx];
                const int       storage_size = storage.get_size();

                for (int i = 0; i < storage_size; i++) {
                    func(storage.get_entity(i));
                }
            }
        }
    }

    void EcsWorld::clear() {
        WG_AUTO_PROFILE_ECS("EcsWorld::clear");

        m_queue.clear();

        for (std::unique_ptr<EcsArchStorage>& storage : m_arch_storage) {
            storage->clear();
        }

        m_entity_info.clear();
        m_entity_pool.clear();
        m_entity_counter = 0;
    }

    void EcsWorld::sync() {
        WG_AUTO_PROFILE_ECS("EcsWorld::sync");

        m_queue.flush();
    }

}// namespace wmoge