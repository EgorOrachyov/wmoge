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

#include <atomic>
#include <memory>
#include <utility>

namespace wmoge {

    /**
     * @class EcsArchStorage
     * @brief Manages entities of a specific single archetype
     */
    class EcsArchStorage {
    public:
        explicit EcsArchStorage(EcsArch arch, int arch_idx);
        EcsArchStorage(const EcsArchStorage&) = delete;
        EcsArchStorage(EcsArchStorage&&)      = delete;
        ~EcsArchStorage();

        EcsEntity                    make_entity();
        void                         destroy_entity(const EcsEntity& entity);
        [[nodiscard]] bool           is_alive(const EcsEntity& entity) const;
        [[nodiscard]] int            get_arch_idx() const { return m_arch_idx; }
        [[nodiscard]] const EcsArch& get_arch() const { return m_arch; }

        template<typename Component>
        Component* get_component(const EcsEntity& entity);
        void*      get_component(const EcsEntity& entity, int idx);

    private:
        [[nodiscard]] std::pair<int, int> get_entity_creds(int idx) const;
        [[nodiscard]] int                 get_component_byte_offset(int entity_idx, int component_idx) const;

        void allocate_chunk(struct EcsChunk& chunk);
        void release_chunk(struct EcsChunk& chunk);
        void create_components(struct EcsChunk& chunk, int entity_idx);
        void destroy_components(struct EcsChunk& chunk, int entity_idx);
        void mark_used(struct EcsChunk& chunk, int entity_idx, bool used);
        void update_generation(struct EcsChunk& chunk, int entity_idx, std::uint32_t generation);

    private:
        std::vector<EcsChunk>                                          m_chunks;
        std::vector<int>                                               m_free_entity_ids;
        std::array<int, EcsLimits::MAX_COMPONENTS>                     m_components_size{};
        std::array<const EcsComponentInfo*, EcsLimits::MAX_COMPONENTS> m_components_info{};
        const int                                                      m_arch_idx;
        const EcsArch                                                  m_arch;
        class EcsRegistry*                                             m_registry;
        int                                                            m_chunk_size;
    };

    template<typename Component>
    Component* EcsArchStorage::get_component(const EcsEntity& entity) {
        void* component_raw = get_component(entity, Component::IDX);
        return static_cast<Component*>(component_raw);
    }

}// namespace wmoge

#endif//WMOGE_ECS_MEMORY_HPP
