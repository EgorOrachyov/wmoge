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

#ifndef WMOGE_ECS_WORLD_HPP
#define WMOGE_ECS_WORLD_HPP

#include "core/callback_queue.hpp"
#include "core/fast_map.hpp"
#include "core/task_manager.hpp"
#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_memory.hpp"
#include "ecs/ecs_system.hpp"

#include <cassert>
#include <deque>
#include <mutex>

namespace wmoge {

    /**
     * @class EcsWorld
     * @brief Container which manages created entities and components
     *
     * Typical usage of a system:
     *
     *  1. Allocate an entity using allocate method, save id
     *  2. Call make to construct an entity
     *  3. Process entity in updates
     *  4. Call destroy on entity
     *
     *  If you run in jobs or in systems update, for steps 2) and 4) use queue
     *  to schedule commands, which will be executed on `sync` step.
     */
    class EcsWorld {
    public:
        EcsWorld();

        /** @brief Allocates new entity for later creation */
        [[nodiscard]] EcsEntity allocate_entity();

        /** @brief Create new entity within work with requested archetype */
        void make_entity(const EcsEntity& entity, const EcsArch& arch);

        /** @brief Destroys entity by a handle */
        void destroy_entity(const EcsEntity& entity);

        /** @brief Checks whenever entity with given handle is alive in still the world */
        [[nodiscard]] bool is_alive(const EcsEntity& entity) const;

        /** @brief Return archetype of given entity by its handle */
        [[nodiscard]] EcsArch get_arch(const EcsEntity& entity) const;

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

        /** @brief Registers system within a world */
        void register_system(const std::shared_ptr<EcsSystem>& system);

        /** @brief Manual trigger of system execution */
        void execute_system(const std::shared_ptr<EcsSystem>& system);

        /** @brief Clear world destroying all entities */
        void clear();

        /** @brief Sync world, flushing all scheduled operations on it */
        void sync();

    private:
        std::vector<EcsEntityInfo> m_entity_info;       // entity info, accessed by entity idx
        std::deque<EcsEntity>      m_entity_pool;       // pool with free entities handles
        int                        m_entity_counter = 0;// total count of created entities

        fast_map<StringId, int>    m_system_to_idx;// map unique system name to idx
        std::vector<EcsSystemInfo> m_systems;      // registered systems info

        fast_map<EcsArch, int>      m_arch_to_idx; // arch to unique index
        std::vector<EcsArchStorage> m_arch_storage;// storage per arch, indexed by arch idx
        std::vector<EcsArch>        m_arch_by_idx; // arch mask, indexed by arch idx

        CallbackQueue m_queue;       // queue for async world operations, flushed on sync
        TaskManager*  m_task_manager;// manager for parallel system update

        mutable std::mutex m_mutex;
    };

    template<class Component>
    const Component& EcsWorld::get_component(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        return *(m_arch_storage[entity_info.arch].template get_component<Component>(entity_info.storage));
    }

    template<class Component>
    Component& EcsWorld::get_component_rw(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        return *(m_arch_storage[entity_info.arch].template get_component<Component>(entity_info.storage));
    }

    template<class Component>
    bool EcsWorld::has_component(const EcsEntity& entity) const {
        assert(entity.is_valid());
        assert(is_alive(entity));

        const EcsEntityInfo& entity_info = m_entity_info[entity.idx];
        return m_arch_by_idx[entity_info.arch].template has_component<Component>();
    }

}// namespace wmoge

#endif//WMOGE_ECS_WORLD_HPP
