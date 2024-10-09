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

#include "core/class.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/weak_ref.hpp"
#include "ecs/ecs_world.hpp"
#include "render/culling.hpp"
#include "render/graphics_pipeline.hpp"
#include "render/render_scene.hpp"
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
        Strid              name;
        class EcsRegistry* ecs_registry;
        class TaskManager* task_manager;
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
     * Editing of the scene done by a separate stucture, named SceneTree. Tree manages hierarchy of
     * nodes with extra editor information (not shared with final game). It follows SOLID principels,
     * gives flexibility and performance in the final game (where Godot, UE, CryEngine, Unity use
     * mix or editor and scene logic, what causes pure CPU performance of scene processing).
     * 
     * Update of this scene state, sinmulation, scene rendering is done externally.
     * Scene data is travered by a scene manager, and required operations performed there.
     * 
     * @see SceneNode
     * @see SceneTree
     */
    class Scene final : public RefCnt {
    public:
        Scene(const SceneCreateInfo& info);

        Status build(const SceneData& data);
        void   advance(float delta_time);
        void   clear();
        void   set_state(SceneState state);
        void   finalize();

        [[nodiscard]] const Strid&    get_name();
        [[nodiscard]] EcsWorld*       get_ecs_world();
        [[nodiscard]] CullingManager* get_culling_manager();
        [[nodiscard]] RenderScene*    get_render_scene();
        [[nodiscard]] float           get_time() const { return m_time; }
        [[nodiscard]] float           get_delta_time() const { return m_delta_time; }
        [[nodiscard]] bool            need_simulate() const { return m_need_simulate; }
        [[nodiscard]] bool            need_render() const { return m_need_render; }
        [[nodiscard]] int             get_frame_id() const { return m_frame_id; }
        [[nodiscard]] SceneState      get_state() const { return m_state; }

    private:
        std::unique_ptr<EcsWorld>       m_ecs_world;
        std::unique_ptr<CullingManager> m_culling_manager;
        std::unique_ptr<RenderScene>    m_render_scene;

        Strid m_name;
        float m_time          = 0.0f;
        float m_delta_time    = 0.0f;
        bool  m_need_simulate = true;
        bool  m_need_render   = true;
        int   m_frame_id      = -1;

        SceneState m_state = SceneState::Default;
    };

}// namespace wmoge