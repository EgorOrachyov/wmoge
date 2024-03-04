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

#include "core/fast_vector.hpp"
#include "ecs/ecs_component.hpp"
#include "ecs/ecs_entity.hpp"
#include "math/aabb.hpp"
#include "math/color.hpp"
#include "math/mat.hpp"
#include "math/math_utils3d.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "render/camera.hpp"
#include "render/culling.hpp"
#include "render/light.hpp"
#include "render/model_instance.hpp"

#include <memory>
#include <optional>

namespace wmoge {

    /**
     * @class EcsComponentChildren
     * @brief List of entity children, for complex objects
     */
    struct EcsComponentChildren : EcsComponent<EcsComponentChildren> {
        WG_ECS_COMPONENT(EcsComponentChildren);

        fast_vector<EcsEntity> children;
    };

    /**
     * @class EcsComponentParent
     * @brief Parent entity, for complex objects
     */
    struct EcsComponentParent : EcsComponent<EcsComponentParent> {
        WG_ECS_COMPONENT(EcsComponentParent);

        EcsEntity parent;
    };

    /**
     * @class EcsComponentTransform
     * @brief Local transform of the entity in some hierarchy
     */
    struct EcsComponentTransform : EcsComponent<EcsComponentTransform> {
        WG_ECS_COMPONENT(EcsComponentTransform);

        Transform3d transform;
    };

    /**
     * @class EcsComponentTransformUpd
     * @brief Local transform update relative data
     */
    struct EcsComponentTransformUpd : EcsComponent<EcsComponentTransformUpd> {
        WG_ECS_COMPONENT(EcsComponentTransformUpd);

        int  batch_id           = 0;
        int  last_frame_updated = -1;
        bool is_dirty           = true;
    };

    /**
     * @class EcsComponentLocalToWorld
     * @brief Matrix to convert local to world coordinates of an object
     */
    struct EcsComponentLocalToWorld : EcsComponent<EcsComponentLocalToWorld> {
        WG_ECS_COMPONENT(EcsComponentLocalToWorld);

        Mat4x4f matrix = Math3d::identity();
    };

    /**
     * @class EcsComponentWorldToLocal
     * @brief Matrix to convert world to local coordinates of an object
     */
    struct EcsComponentWorldToLocal : EcsComponent<EcsComponentWorldToLocal> {
        WG_ECS_COMPONENT(EcsComponentWorldToLocal);

        Mat4x4f matrix = Math3d::identity();
    };

    /**
     * @class EcsComponentLocalToParent
     * @brief Matrix to convert local to parent coordinates of an object
     */
    struct EcsComponentLocalToParent : EcsComponent<EcsComponentLocalToParent> {
        WG_ECS_COMPONENT(EcsComponentLocalToParent);

        Mat4x4f matrix = Math3d::identity();
    };

    /**
     * @class EcsComponentAabbLocal
     * @brief Aabb volume of object in the local space
     */
    struct EcsComponentAabbLocal : EcsComponent<EcsComponentAabbLocal> {
        WG_ECS_COMPONENT(EcsComponentAabbLocal);

        Aabbf aabb;
    };

    /**
     * @class EcsComponentAabbWorld
     * @brief Aabb volume of object in the world space
     */
    struct EcsComponentAabbWorld : EcsComponent<EcsComponentAabbWorld> {
        WG_ECS_COMPONENT(EcsComponentAabbWorld);

        Aabbf aabb;
    };

    /**
     * @class EcsComponentTag
     * @brief Unique tag for fast search of an entity
     */
    struct EcsComponentTag : EcsComponent<EcsComponentTag> {
        WG_ECS_COMPONENT(EcsComponentTag);

        Strid tag;
    };

    /**
     * @class EcsComponentName
     * @brief Unique full name of entity on a scene
     */
    struct EcsComponentName : EcsComponent<EcsComponentName> {
        WG_ECS_COMPONENT(EcsComponentName);

        std::string name;
    };

    /**
     * @class EcsComponentCamera
     * @brief Game camera component
     */
    struct EcsComponentCamera : EcsComponent<EcsComponentCamera> {
        WG_ECS_COMPONENT(EcsComponentCamera);

        Camera camera;
    };

    /**
     * @class EcsComponentLight
     * @brief Light source component
     */
    struct EcsComponentLight : EcsComponent<EcsComponentLight> {
        WG_ECS_COMPONENT(EcsComponentLight);

        Light light;
    };

    /**
     * @class EcsComponentModel
     * @brief A renderable model
     */
    struct EcsComponentModel : EcsComponent<EcsComponentModel> {
        WG_ECS_COMPONENT(EcsComponentModel);

        ModelInstance instance;
    };

    /**
     * @class EcsComponentCullingItem
     * @brief Item for culling (shared for geometry, lights, etc.)
     */
    struct EcsComponentCullingItem : EcsComponent<EcsComponentCullingItem> {
        WG_ECS_COMPONENT(EcsComponentCullingItem);

        CullingItem item;
    };

}// namespace wmoge