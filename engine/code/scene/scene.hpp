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

#include "core/any_storage.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/weak_ref.hpp"
#include "scene/scene_data.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wmoge {

    /** @brief State of an scene */
    enum class SceneState {
        Default,//< Scene created, not started yet
        Playing,//< Scene started and active
        Paused, //< Scene started and suspended
        Finished//< Scene finished and ready for deletion
    };

    /**
     * @class SceneCreateInfo 
     * @brief Info for scene construction
     */
    struct SceneCreateInfo {
        Strid name;
    };

    /**
     * @class Scene
     * @brief Scene objects container representing running game state
     * 
     * Scene is a data container for a runtime scene data, required for game simulation and rendering.
     * The scene itself is a plain data container of game objects whitout any simulation or drawing loging.
     * Game objects stored as entities edintified by simple numeric ids. Components of entities stored
     * in a ECS world in an optimized fashion, what gives fast processing and low overhed.
     * 
     * Scene data is optimized for runtime simulation, fast deserialization, not for the editing.
     * Editing of the scene done by a separate stucture, managed outside. Editing metadata stored
     * only for editor scenes, not affecting final runtime performace. It follows SOLID principels,
     * gives flexibility and performance in the final game (where Godot, UE, CryEngine, Unity use
     * mix or editor and scene logic, what causes poor CPU performance of scene processing).
     * 
     * Update of this scene state, sinmulation, scene rendering is done externally.
     * Scene data is travered by a scene manager, and required operations performed there.
     */
    class Scene final : public RefCnt {
    public:
        Scene(SceneCreateInfo& info);

        void advance(float delta_time);
        void set_state(SceneState state);
        void finalize();

        template<typename T>
        T* get();

        template<typename T>
        void add(std::shared_ptr<T> attribute);

        [[nodiscard]] const Strid& get_name();
        [[nodiscard]] float        get_time() const { return m_time; }
        [[nodiscard]] float        get_delta_time() const { return m_delta_time; }
        [[nodiscard]] int          get_frame_id() const { return m_frame_id; }
        [[nodiscard]] SceneState   get_state() const { return m_state; }

    private:
        AnyStorage m_attributes;
        SceneState m_state = SceneState::Default;
        Strid      m_name;
        float      m_time       = 0.0f;
        float      m_delta_time = 0.0f;
        int        m_frame_id   = -1;
    };

    template<typename T>
    inline T* Scene::get() {
        return m_attributes.get<T*>();
    }

    template<typename T>
    inline void Scene::add(std::shared_ptr<T> attribute) {
        T* attribute_ptr = attribute.get();
        m_attributes.add<std::shared_ptr<T>>(std::move(attribute));
        m_attributes.add<T*>(attribute_ptr);
    }

    using SceneRef = Ref<Scene>;

}// namespace wmoge