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

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_system.hpp"
#include "memory/mem_pool.hpp"

#include <array>
#include <mutex>

namespace wmoge {

    /**
     * @class EcsRegistry
     * @brief Manages global ecs state and static info
     */
    class EcsRegistry {
    public:
        EcsRegistry();
        EcsRegistry(const EcsRegistry&) = delete;
        EcsRegistry(EcsRegistry&&)      = delete;
        ~EcsRegistry()                  = default;

        [[nodiscard]] int                     get_component_idx(const Strid& name);
        [[nodiscard]] const EcsComponentInfo& get_component_info(const Strid& name);
        [[nodiscard]] const EcsComponentInfo& get_component_info(int idx);
        [[nodiscard]] MemPool&                get_component_pool(int idx);
        [[nodiscard]] MemPool&                get_entity_pool();
        [[nodiscard]] int                     get_chunk_size() const { return m_chunk_size; }
        [[nodiscard]] int                     get_expand_size() const { return m_expand_size; }

        template<typename Component>
        void register_component();

    private:
        std::array<EcsComponentInfo, EcsLimits::MAX_COMPONENTS>         m_components_info;       // type info of component, indexed by component id
        std::array<std::unique_ptr<MemPool>, EcsLimits::MAX_COMPONENTS> m_components_pool;       // pools to allocate components chunks
        fast_map<Strid, int>                                            m_components_name_to_idx;// resolve component name to its idx
        std::unique_ptr<MemPool>                                        m_entity_pool;           // pool to allocate entities chunks

        int m_chunk_size      = 16;// num of components of a single type sequentially allocate in one chunk
        int m_expand_size     = 2; // num of chunks in a single pooled allocation in head
        int m_component_types = 0; // total num of registered components
    };

    template<typename Component>
    void EcsRegistry::register_component() {
        const int component_idx = m_component_types++;
        assert(component_idx < EcsLimits::MAX_COMPONENTS);

        EcsComponentInfo& component_info = m_components_info[component_idx];
        assert(component_info.name == Strid());
        assert(component_info.idx == -1);
        assert(component_info.size == -1);

        Component::bind(component_idx, SID(Component::NAME_CSTR));

        component_info.name = Component::NAME;
        component_info.idx  = Component::IDX;
        component_info.size = sizeof(Component);

        component_info.create = [](void* mem) -> void {
            new (mem) Component();
        };

        component_info.destroy = [](void* mem) -> void {
            static_cast<Component*>(mem)->~Component();
        };

        component_info.swap = [](void* mem1, void* mem2) -> void {
            Component& cmp1 = *(static_cast<Component*>(mem1));
            Component& cmp2 = *(static_cast<Component*>(mem2));
            std::swap(cmp1, cmp2);
        };

        m_components_name_to_idx[component_info.name] = component_info.idx;
        m_components_pool[component_info.idx]         = std::make_unique<MemPool>(component_info.size * m_chunk_size, m_expand_size);
    }

}// namespace wmoge