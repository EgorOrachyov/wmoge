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

#include "asset/asset_ref.hpp"
#include "core/buffered_vector.hpp"
#include "core/class.hpp"
#include "core/ref.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_world.hpp"
#include "io/serialization.hpp"
#include "math/color.hpp"
#include "math/transform.hpp"
#include "platform/window.hpp"
#include "render/culling.hpp"
#include "render/graphics_pipeline.hpp"
#include "render/model.hpp"
#include "render/render_scene.hpp"
#include "scene/scene_components.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wmoge {

    /**
     * @class SceneDataSpatial
     * @brief Serializable struct with transform data for a scene entity
     */
    struct SceneDataSpatial {
        Transform3d        transform;
        std::optional<int> parent;
    };

    /**
     * @class SceneDataCamera
     * @brief Serializable struct with camera data for a scene entity
     */
    struct SceneDataCamera {
        Strid            name;
        Color4f          color      = Color::BLACK4f;
        float            fov        = 45.0f;
        float            near       = 0.1f;
        float            far        = 10000.0f;
        CameraProjection projection = CameraProjection::Perspective;

        void fill(EcsComponentCamera& component) const;
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
        Strid                               name;
        std::vector<EcsArch>                entities;
        SceneEntityVector<std::string>      names;
        SceneEntityVector<SceneDataSpatial> hier;
        SceneEntityVector<SceneDataCamera>  cameras;
        GraphicsPipelineSettings            pipeline;
    };

}// namespace wmoge