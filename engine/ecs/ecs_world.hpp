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

#include "core/callback_queue.hpp"
#include "core/fast_map.hpp"
#include "core/synchronization.hpp"
#include "core/task_manager.hpp"
#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_memory.hpp"
#include "ecs/ecs_system.hpp"

#include <cassert>
#include <deque>
#include <functional>
#include <mutex>

namespace wmoge {

    /**
     * @class EcsWorld
     * @brief Container which manages created entities and components
     *
     * Typical usage of a world:
     *
     *  1. Allocate an entity using allocate method, save entity id
     *  2. Call make to construct an entity
     *  3. Process entity in updates, systems, queries
     *  4. Call destroy on entity
     *
     *  If you run in jobs or in systems update, for steps 2) and 4) use queue
     *  to schedule commands, which will be executed on `sync` step.
     */
    class EcsWorld {
    public:
        EcsWorld();
        ~EcsWorld();

        /** @brief Allocates new entity for later creation */
        [[nodiscard]] EcsEntity allocate_entity();

        /** @brief Create new entity within world with requested archetype */
        void make_entity(const EcsEntity& entity, const EcsArch& arch);

        /** @brief Swap entity components data to other entity */
        void swap_entity(const EcsEntity& dst, const EcsEntity& src);

        /** @brief Change existing entity arch to a new archetype */
        void rearch_entity(const EcsEntity& entity, const EcsArch& new_arch);

        /** @brief Destroys entity by a handle */
        void destroy_entity(const EcsEntity& entity);

        /** @brief Checks whenever entity with given handle is still alive in the world */
        [[nodiscard]] bool is_alive(const EcsEntity& entity) const;

        /** @brief Return archetype of given entity by its handle */
        [[nodiscard]] EcsArch get_arch(const EcsEntity& entity) const;

        /** @brief Returns component existing or new (possibly changes enity arch) */
        template<class Component>
        [[nodiscard]] Component& get_or_create_component(const EcsEntity& entity);

        /** @brief Returns component for read-only operations */
        template<class Component>
        [[nodiscard]] const Component& get_component(const EcsEntity& entity) const;

        /** @brief Returns component for read-write operations */
        template<class Component>
        [[nodiscard]] Component& get_component_rw(const EcsEntity& entity) const;

        /** @brief Query if entity has given component */
        template<class Component>
        [[nodiscard]] bool has_component(const EcsEntity& entity) const;

        /** @brief Return queue to schedule async commands to execute on next sync */
        [[nodiscard]] CallbackQueue* queue();

        /** @brief Ids of matching arch for given query */
        std::vector<int> filter_arch_idx(const EcsQuery& query);

        /** @brief Possibly add new arch to the world allocating space */
        void register_arch(EcsArch arch);

        /** @brief Registers system within a world */
        void register_system(const std::shared_ptr<EcsSystem>& system);

        /** @brief Manual trigger of system execution */
        void execute_system(const std::shared_ptr<EcsSystem>& system);

        /** @brief Manual trigger of system execution */
        void execute_system(EcsSystem& system);

        /** @brief Exec function for each entity matching query*/
        void each(const EcsQuery& query, const std::function<void(EcsEntity)>& func);

        /** @brief Clear world destroying all entities */
        void clear();

        /** @brief Sync world, flushing all scheduled operations on it */
        void sync();

        /** @brief Sets world specific attribute to access external context of the world */
        template<typename T>
        void set_attribute(int slot, T& attribute);

        /** @brief Get world specific attribute to access external context of the world */
        template<typename T>
        T& get_attribute(int slot = 0);

    private:
        std::vector<EcsEntityInfo>                   m_entity_info;       // entity info, accessed by entity idx
        std::deque<EcsEntity>                        m_entity_pool;       // pool with free entities handles
        int                                          m_entity_counter = 0;// total count of created entities
        fast_map<StringId, int>                      m_system_to_idx;     // map unique system name to idx
        std::vector<EcsSystemInfo>                   m_systems;           // registered systems info
        fast_map<EcsArch, int>                       m_arch_to_idx;       // arch to unique index
        std::vector<std::unique_ptr<EcsArchStorage>> m_arch_storage;      // storage per arch, indexed by arch idx
        std::vector<EcsArch>                         m_arch_by_idx;       // arch mask, indexed by arch idx
        fast_vector<void*>                           m_attributes;        // custom attributes to access context within world

        CallbackQueue m_queue;       // queue for async world operations, flushed on sync
        TaskManager*  m_task_manager;// manager for parallel system update

        SpinMutex m_mutex;
    };

    template<class Component>
    inline Component& EcsWorld::get_or_create_component(const EcsEntity& entity) {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        const EcsArch&       enity_arch  = m_arch_by_idx[entity_info.arch];

        if (!enity_arch.has_component<Component>()) {
            EcsArch new_arch = enity_arch;
            new_arch.set_component<Component>();
            rearch_entity(entity, new_arch);
        }

        return get_component_rw<Component>(entity);
    }

    template<class Component>
    const Component& EcsWorld::get_component(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        return *(m_arch_storage[entity_info.arch]->template get_component<Component>(entity_info.storage));
    }

    template<class Component>
    Component& EcsWorld::get_component_rw(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        return *(m_arch_storage[entity_info.arch]->template get_component<Component>(entity_info.storage));
    }

    template<class Component>
    bool EcsWorld::has_component(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        return m_arch_by_idx[entity_info.arch].template has_component<Component>();
    }

    template<typename T>
    inline void EcsWorld::set_attribute(int slot, T& attribute) {
        if (slot >= m_attributes.size()) m_attributes.resize(slot + 1);
        m_attributes[slot] = &attribute;
    }

    template<typename T>
    inline T& EcsWorld::get_attribute(int slot) {
        assert(slot < m_attributes.size());
        return *(static_cast<T*>(m_attributes[slot]));
    }

}// namespace wmoge
