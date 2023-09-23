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

#ifndef WMOGE_SCENE_IO_HPP
#define WMOGE_SCENE_IO_HPP

#include "core/string_id.hpp"
#include "ecs/ecs_core.hpp"
#include "io/archive.hpp"
#include "io/yaml.hpp"
#include "math/color.hpp"
#include "math/math_utils.hpp"
#include "math/math_utils3d.hpp"
#include "render/render_camera.hpp"

#include <string>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class SceneDataCamera
     * @brief Serializable struct with camera data for a scene object
     */
    struct SceneDataCamera {
        Color4f          color      = Color::BLACK4f;
        Vec4f            viewport   = Vec4f(0, 0, 1, 1);
        float            fov        = 45.0f;
        float            near       = 0.1f;
        float            far        = 10000.0f;
        StringId         target     = SID("primary");
        CameraProjection projection = CameraProjection::Perspective;

        friend Status yaml_read(const YamlConstNodeRef& node, SceneDataCamera& data);
        friend Status yaml_write(YamlNodeRef node, const SceneDataCamera& data);
    };

    /**
     * @class SceneDataTransform
     * @brief
     */
    struct SceneDataTransform {
    };

    /**
     * @class SceneDataMeshStatic
     * @brief
     */
    struct SceneDataMeshStatic {
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

    /**
     * @class SceneData
     * @brief Serializable struct with a scene data for a runtime scene
     */
    struct SceneData {
        // used to reference entities in this struct
        using EntityIndex = int;

        // vector with data mapped to entity by index
        template<typename T>
        using EntityVector = std::vector<std::pair<EntityIndex, T>>;

        std::string              scene_name;
        std::vector<EcsArch>     entities;
        std::vector<std::string> entities_names;

        EntityVector<SceneDataCamera>         cameras;
        EntityVector<SceneDataTransform>      transforms;
        EntityVector<SceneDataMeshStatic>     meshes_static;
        EntityVector<SceneDataAudioSource>    audio_sources;
        EntityVector<SceneDataAudioListener>  autio_listeners;
        EntityVector<SceneDataAudioLuaScript> lua_scripts;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_IO_HPP
