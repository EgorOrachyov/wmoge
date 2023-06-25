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

#ifndef WMOGE_ECS_REGISTRY_HPP
#define WMOGE_ECS_REGISTRY_HPP

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

        [[nodiscard]] int                     get_component_idx(const StringId& name);
        [[nodiscard]] const EcsComponentInfo& get_component_info(const StringId& name);
        [[nodiscard]] const EcsComponentInfo& get_component_info(int idx);
        [[nodiscard]] MemPool&                get_component_pool(int idx);
        [[nodiscard]] int                     get_chunk_size() const { return m_chunk_size; }
        [[nodiscard]] int                     get_expand_size() const { return m_expand_size; }

        template<typename Component>
        void register_component();

    private:
        std::array<EcsComponentInfo, EcsLimits::MAX_COMPONENTS>         m_components_info;
        std::array<std::unique_ptr<MemPool>, EcsLimits::MAX_COMPONENTS> m_components_pool;
        fast_map<StringId, int>                                         m_components_name_to_idx;
        int                                                             m_chunk_size  = 16;
        int                                                             m_expand_size = 2;
    };

    template<typename Component>
    void EcsRegistry::register_component() {
        EcsComponentInfo& component_info = m_components_info[Component::IDX];

        component_info.name    = SID(Component::NAME);
        component_info.idx     = Component::IDX;
        component_info.size    = sizeof(Component);
        component_info.create  = [](EcsComponent* mem) { new (mem) Component(); };
        component_info.destroy = [](EcsComponent* mem) { static_cast<Component*>(mem)->~Component(); };

        m_components_name_to_idx[component_info.name] = component_info.idx;
        m_components_pool[component_info.idx]         = std::make_unique<MemPool>(component_info.size * m_chunk_size, m_expand_size);
    }

}// namespace wmoge

#endif//WMOGE_ECS_REGISTRY_HPP
