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

#ifndef WMOGE_SCENE_COMPONENTS_HPP
#define WMOGE_SCENE_COMPONENTS_HPP

#include "core/fast_vector.hpp"
#include "ecs/ecs_component.hpp"
#include "ecs/ecs_entity.hpp"
#include "math/aabb.hpp"
#include "math/color.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "render/render_mesh_skinned.hpp"
#include "render/render_mesh_static.hpp"
#include "render/render_object.hpp"
#include "render/visibility.hpp"
#include "scene/scene_camera.hpp"
#include "scene/scene_node.hpp"
#include "scene/scene_transform.hpp"

#include <memory>
#include <optional>

namespace wmoge {

    /**
     * @class EcsComponentChildren
     * @brief List of entity children, for complex objects
     */
    struct EcsComponentChildren {
        WG_ECS_COMPONENT(EcsComponentChildren, 0);

        fast_vector<EcsEntity> children;
    };

    /**
     * @class EcsComponentParent
     * @brief Parent entity, for complex objects
     */
    struct EcsComponentParent {
        WG_ECS_COMPONENT(EcsComponentParent, 1);

        EcsEntity parent;
    };

    /**
     * @class EcsComponentSceneTransform
     * @brief Node in a relative transform hierarchy of objects
     */
    struct EcsComponentSceneTransform {
        WG_ECS_COMPONENT(EcsComponentSceneTransform, 2);

        Ref<SceneTransform> transform;
    };

    /**
     * @class EcsComponentLocalToWorld
     * @brief Matrix to convert local to world coordinates of an object
     */
    struct EcsComponentLocalToWorld {
        WG_ECS_COMPONENT(EcsComponentLocalToWorld, 3);

        Mat4x4f matrix;
    };

    /**
     * @class EcsComponentLocalToParent
     * @brief Matrix to convert local to parent coordinates of an object
     */
    struct EcsComponentLocalToParent {
        WG_ECS_COMPONENT(EcsComponentLocalToParent, 4);

        Mat4x4f matrix;
    };

    /**
     * @class EcsComponentTag
     * @brief Unique tag for fast search of an entity
     */
    struct EcsComponentTag {
        WG_ECS_COMPONENT(EcsComponentName, 5);

        StringId tag;
    };

    /**
     * @class EcsComponentName
     * @brief Unique full name of entity on a scene
     */
    struct EcsComponentName {
        WG_ECS_COMPONENT(EcsComponentName, 6);

        std::string name;
    };

    /**
     * @class EcsComponentCamera
     * @brief Game camera component
     */
    struct EcsComponentCamera {
        WG_ECS_COMPONENT(EcsComponentCamera, 7);

        Ref<Camera> camera;
    };

    /**
     * @class EcsComponentLight
     * @brief Light source component
     */
    struct EcsComponentLight {
        WG_ECS_COMPONENT(EcsComponentLight, 8);

        /* std::unique_ptr<Light> light; */
    };

    /**
     * @class EcsComponentMeshStatic
     * @brief Static mesh renderer component
     */
    struct EcsComponentMeshStatic {
        WG_ECS_COMPONENT(EcsComponentMeshStatic, 9);

        std::unique_ptr<RenderMeshStatic> mesh;
        VisibilityItem                    vis_item     = VIS_ITEM_INVALID;
        int                               primitive_id = -1;
        bool                              dirty        = true;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_COMPONENTS_HPP
