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
#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_memory.hpp"

#include <cassert>
#include <mutex>

namespace wmoge {

    /**
     * @class EcsWorld
     * @brief Container which manages created entities and components
     */
    class EcsWorld {
    public:
        /** @brief Create new entity within work with requested archetype */
        EcsEntity make_entity(const EcsArch& arch);

        /** @brief Destroys entity by a handle */
        void destroy_entity(const EcsEntity& entity);

        /** @brief Checks whenever entity with given handle is alive in still the world */
        bool is_alive(const EcsEntity& entity) const;

        /** @brief Return archetype of given entity by its handle */
        EcsArch get_arch(const EcsEntity& entity) const;

        template<class Component>
        const Component& get_component(const EcsEntity& entity) const;

        template<class Component>
        Component& get_component_rw(const EcsEntity& entity) const;

        template<class Component>
        bool has_component(const EcsEntity& entity) const;

        void sync();

    private:
        std::vector<std::unique_ptr<EcsArchStorage>> m_storage;
        fast_map<EcsArch, int>                       m_storage_idx;
        CallbackQueue                                m_queue;
    };

    template<class Component>
    const Component& EcsWorld::get_component(const EcsEntity& entity) const {
        assert(entity.is_invalid());
        assert(entity.arch < m_storage.size());

        return *m_storage[entity.arch]->get_component<Component>(entity);
    }

    template<class Component>
    Component& EcsWorld::get_component_rw(const EcsEntity& entity) const {
        assert(entity.is_invalid());
        assert(entity.arch < m_storage.size());

        return *m_storage[entity.arch]->get_component<Component>(entity);
    }

    template<class Component>
    bool EcsWorld::has_component(const EcsEntity& entity) const {
        assert(entity.is_invalid());
        assert(entity.arch < m_storage.size());

        return get_arch(entity).template has_component<Component>();
    }

}// namespace wmoge

#endif//WMOGE_ECS_WORLD_HPP
