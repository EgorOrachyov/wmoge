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

#ifndef WMOGE_SCENE_HPP
#define WMOGE_SCENE_HPP

#include "core/class.hpp"
#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_world.hpp"
#include "math/transform.hpp"
#include "platform/window.hpp"
#include "resource/model.hpp"
#include "resource/resource_ref.hpp"
#include "scene/scene_camera.hpp"
#include "scene/scene_transform.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class SceneDataCamera
     * @brief Serializable struct with camera data for a scene object
     */
    struct SceneDataCamera {
        StringId         name;
        Color4f          color      = Color::BLACK4f;
        Vec4f            viewport   = Vec4f(0, 0, 1, 1);
        float            fov        = 45.0f;
        float            near       = 0.1f;
        float            far        = 10000.0f;
        CameraProjection projection = CameraProjection::Perspective;

        friend Status yaml_read(const YamlConstNodeRef& node, SceneDataCamera& data);
        friend Status yaml_write(YamlNodeRef node, const SceneDataCamera& data);
    };

    /**
     * @class SceneDataMeshStatic
     * @brief Serializable struct with static mesh info to attach to object
     */
    struct SceneDataMeshStatic {
        ResourceRefHard<Model> model;

        friend Status yaml_read(const YamlConstNodeRef& node, SceneDataMeshStatic& data);
        friend Status yaml_write(YamlNodeRef node, const SceneDataMeshStatic& data);
    };

    /**
     * @class SceneDataMeshSkinned
     * @brief Serializable struct with skinned mesh info to attach to object
     */
    struct SceneDataMeshSkinned {
    };

    /**
     * @class SceneDataAudioSource
     * @brief
     */
    struct SceneDataAudioSource {
    };

    /**
     * @class SceneDataAudioListener
     * @brief
     */
    struct SceneDataAudioListener {
    };

    /**
     * @class SceneDataAudioLuaScript
     * @brief
     */
    struct SceneDataAudioLuaScript {
    };

    /** @brief Index used to reference entities in this struct */
    using SceneEntityIndex = int;

    /** @brief Vector with data mapped to entity by index */
    template<typename T>
    using SceneEntityVector = std::vector<std::pair<SceneEntityIndex, T>>;

    /**
     * @class SceneData
     * @brief Serializable struct with a scene data for a runtime scene
     */
    struct SceneData final {
        StringId                                   name;
        std::vector<EcsArch>                       entities;
        std::vector<std::string>                   names;
        std::vector<TransformEdt>                  transforms;
        SceneEntityVector<SceneDataCamera>         cameras;
        SceneEntityVector<SceneDataMeshStatic>     meshes_static;
        SceneEntityVector<SceneDataMeshSkinned>    meshes_skinned;
        SceneEntityVector<SceneDataAudioSource>    audio_sources;
        SceneEntityVector<SceneDataAudioListener>  autio_listeners;
        SceneEntityVector<SceneDataAudioLuaScript> lua_scripts;

        friend Status yaml_read(const YamlConstNodeRef& node, SceneData& data);
        friend Status yaml_write(YamlNodeRef node, const SceneData& data);
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
     * Editing of the scene used by a separate stucture, named SceneTree. Tree manages hierarchy of
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
        Scene(StringId name = StringId());
        ~Scene() override = default;

        Status build(const SceneData& data);
        void   add_camera(EcsEntity entity, const SceneDataCamera& data);
        void   add_mesh_static(EcsEntity entity, const SceneDataMeshStatic& data);
        void   advance(float delta_time);
        void   clear();

        [[nodiscard]] const StringId&        get_name();
        [[nodiscard]] EcsWorld*              get_ecs_world();
        [[nodiscard]] SceneTransformManager* get_transforms();
        [[nodiscard]] CameraManager*         get_cameras();
        [[nodiscard]] float                  get_time() const { return m_time; }
        [[nodiscard]] float                  get_time_factor() const { return m_time_factor; }
        [[nodiscard]] float                  get_delta_time() const { return m_delta_time; }
        [[nodiscard]] bool                   need_simulate() const { return m_need_simulate; }
        [[nodiscard]] bool                   need_render() const { return m_need_render; }

    private:
        std::unique_ptr<EcsWorld>              m_ecs_world;
        std::unique_ptr<SceneTransformManager> m_transforms;
        std::unique_ptr<CameraManager>         m_cameras;
        StringId                               m_name;
        float                                  m_time          = 0.0f;
        float                                  m_time_factor   = 1.0f;
        float                                  m_delta_time    = 0.0f;
        bool                                   m_need_simulate = true;
        bool                                   m_need_render   = true;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_HPP
