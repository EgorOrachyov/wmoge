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

#include "core/array_view.hpp"
#include "core/uuid.hpp"
#include "ecs/ecs_entity.hpp"
#include "rtti/traits.hpp"

namespace wmoge {

    /**
     * @class EntityFeature
     * @brief Describes single aspect/feature of a particular ecs entity
     */
    class EntityFeature : public RttiObject {
    public:
        WG_RTTI_CLASS(EntityFeature, RttiObject)

        EntityFeature() = default;
    };

    WG_RTTI_CLASS_BEGIN(EntityFeature) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class EntityFeatureVector
     * @brief Optimized storage for features of a group of entities
     */
    class EntityFeatureVector : public RttiObject {
    public:
        WG_RTTI_CLASS(EntityFeatureVector, RttiObject)

        EntityFeatureVector() = default;
    };

    WG_RTTI_CLASS_BEGIN(EntityFeatureVector) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /** @brief Context passed to trait on entity setup */
    struct EntitySetupContext {
        class EcsWorld* world = nullptr;
        class Scene*    scene = nullptr;
    };

    /** @brief Context passed to trait on entity creation */
    struct EntityBuildContext {
        class EcsWorld*     world = nullptr;
        class Scene*        scene = nullptr;
        class SceneUuidMap* uuid  = nullptr;
    };

    /**
     * @class EntityFeatureTrait
     * @brief Entity trait responsible for handling partical features on the entity
     */
    class EntityFeatureTrait : public RefCnt {
    public:
        virtual RttiSubclass<EntityFeature> get_feature_type() { return {}; }
        virtual Status                      fill_requirements(std::vector<RttiSubclass<EntityFeature>>& required_features) { return StatusCode::NotImplemented; }
        virtual Status                      setup_entity(EcsArch& arch, const EntityFeature& feature, EntitySetupContext& context) { return StatusCode::NotImplemented; }
        virtual Status                      build_entity(EcsEntity entity, const EntityFeature& feature, EntityBuildContext& context) { return StatusCode::NotImplemented; }
        virtual Status                      build_entities(array_view<EcsEntity> entities, const EntityFeatureVector& features, EntityBuildContext& context) { return StatusCode::NotImplemented; }
    };

}// namespace wmoge