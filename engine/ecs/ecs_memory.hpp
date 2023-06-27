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

#ifndef WMOGE_ECS_MEMORY_HPP
#define WMOGE_ECS_MEMORY_HPP

#include "ecs/ecs_component.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"

#include <memory>
#include <utility>

namespace wmoge {

    /**
     * @class EcsChunk
     * @brief Single chunk storing multiple entities tightly (cache-friendly in memory)
     */
    struct EcsChunk {
        std::array<std::uint8_t*, EcsLimits::MAX_COMPONENTS> components{};
        std::vector<EcsEntity>                               entity;
    };

    /**
     * @class EcsArchStorage
     * @brief Manages entities of a specific single archetype
     */
    class EcsArchStorage {
    public:
        explicit EcsArchStorage(EcsArch arch);
        ~EcsArchStorage();

        void make_entity(const EcsEntity& entity, std::uint32_t& storage_idx);
        void destroy_entity(const EcsEntity& entity, const std::uint32_t& storage_idx);

        template<typename Component>
        [[nodiscard]] Component* get_component(int storage_idx) const;
        [[nodiscard]] void*      get_component(int storage_idx, int idx) const;

    private:
        [[nodiscard]] std::pair<int, int> get_entity_creds(int idx) const;
        [[nodiscard]] int                 get_component_byte_offset(int entity_idx, int component_idx) const;

        void allocate_chunk(EcsChunk& chunk);
        void release_chunk(EcsChunk& chunk);
        void place(EcsChunk& chunk, int entity_idx, const EcsEntity& entity);
        void destroy(EcsChunk& chunk, int entity_idx, const EcsEntity& entity);

    private:
        std::vector<EcsChunk> m_chunks;         // storage with tightly packed entities data
        std::vector<int>      m_free_entity_ids;// pool with storage idx for new entities

        std::array<const EcsComponentInfo*, EcsLimits::MAX_COMPONENTS> m_components_info{};
        std::array<int, EcsLimits::MAX_COMPONENTS>                     m_components_size{};
        const EcsArch                                                  m_arch;
        class EcsRegistry*                                             m_registry;
        int                                                            m_chunk_size;
    };

    template<typename Component>
    Component* EcsArchStorage::get_component(int storage_idx) const {
        void* component_raw = get_component(storage_idx, Component::IDX);
        return static_cast<Component*>(component_raw);
    }

}// namespace wmoge

#endif//WMOGE_ECS_MEMORY_HPP
