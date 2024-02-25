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

#include "ecs/ecs_entity.hpp"
#include "ecs/ecs_world.hpp"
#include "scene/scene.hpp"

namespace wmoge {

    /**
     * @class Entity
     * @brief Wrapper to work with a scene entity in object-oriented classic fasshion
    */
    class Entity {
    public:
        Entity() = default;
        Entity(EcsEntity ecs_entity, Scene* scene);
        ~Entity() = default;

        template<typename Component>
        Component& get_or_create_component();

        template<typename Component>
        Component& get_component();

        template<typename Component>
        bool has_component() const;

        void reset();

        [[nodiscard]] bool is_valid() const { return m_entity.is_valid(); }
        [[nodiscard]] bool is_invalid() const { return m_entity.is_invalid(); }

        [[nodiscard]] EcsEntity get_ecs_id() const { return m_entity; }
        [[nodiscard]] Scene*    get_scene() const { return m_scene; }

    private:
        EcsEntity m_entity;
        Scene*    m_scene = nullptr;
    };

    template<typename Component>
    inline Component& Entity::get_or_create_component() {
        return m_scene->get_ecs_world()->get_or_create_component<Component>(m_entity);
    }

    template<typename Component>
    inline Component& Entity::get_component() {
        return m_scene->get_ecs_world()->get_component_rw<Component>(m_entity);
    }

    template<typename Component>
    inline bool Entity::has_component() const {
        return m_scene->get_ecs_world()->has_component<Component>(m_entity);
    }

}// namespace wmoge