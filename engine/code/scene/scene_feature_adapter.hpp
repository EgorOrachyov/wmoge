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

#include "scene/scene_feature.hpp"

#include <utility>
#include <vector>

namespace wmoge {

    template<typename T>
    class EntitySimpleFeature : public EntityFeature {
    public:
        T elem;
    };

    template<typename T>
    class EntitySimpleFeatureVector : public EntityFeatureVector {
    public:
        std::vector<std::pair<int, T>> elems;
    };

    template<typename T, typename FeatureType, typename VectorType>
    class EntitySimpleFeatureTrait : public EntityFeatureTrait {
    public:
        virtual Status setup_entity_typed(EcsArch& arch, const T& desc, EntitySetupContext& context) { return StatusCode::NotImplemented; }
        virtual Status build_entity_typed(EcsEntity entity, const T& desc, EntityBuildContext& context) { return StatusCode::NotImplemented; }

        RttiSubclass<EntityFeature> get_feature_type() override {
            return FeatureType::get_class_static();
        }

        Status fill_requirements(std::vector<RttiSubclass<EntityFeature>>& required_features) override {
            return WG_OK;
        }

        Status setup_entity(EcsArch& arch, const EntityFeature& feature, EntitySetupContext& context) override {
            return setup_entity_typed(arch, dynamic_cast<const FeatureType&>(feature).elem, context);
        }

        Status build_entity(EcsEntity entity, const EntityFeature& feature, EntityBuildContext& context) override {
            return build_entity_typed(entity, dynamic_cast<const FeatureType&>(feature).elem, context);
        }

        Status build_entities(array_view<EcsEntity> entities, const EntityFeatureVector& features, EntityBuildContext& context) override {
            const auto& elems = dynamic_cast<const VectorType&>(features).elems;
            for (const std::pair<int, T>& entry : elems) {
                WG_CHECKED(build_entity_typed(entities[entry.first], entry.second, context));
            }
            return WG_OK;
        }
    };

#define WG_NAME_ENTITY_FEATURE(type) \
    type##Feature

#define WG_DECL_ENTITY_FEATURE(type)                         \
    class type##Feature : public EntitySimpleFeature<type> { \
        WG_RTTI_CLASS(type##Feature, EntityFeature)          \
    };                                                       \
    WG_RTTI_CLASS_BEGIN(type##Feature) {                     \
        WG_RTTI_FACTORY();                                   \
        WG_RTTI_FIELD(elem, {RttiInline, RttiUiInline});     \
    }                                                        \
    WG_RTTI_END;

#define WG_NAME_ENTITY_FEATURE_VECTOR(type) \
    type##FeatureVector

#define WG_DECL_ENTITY_FEATURE_VECTOR(type)                              \
    class type##FeatureVector : public EntitySimpleFeatureVector<type> { \
        WG_RTTI_CLASS(type##FeatureVector, EntityFeatureVector)          \
    };                                                                   \
    WG_RTTI_CLASS_BEGIN(type##FeatureVector) {                           \
        WG_RTTI_FACTORY();                                               \
        WG_RTTI_FIELD(elems, {RttiInline, RttiUiInline});                \
    }                                                                    \
    WG_RTTI_END;

#define WG_DECL_ENTITY_FEATURE_AND_VECTOR(type) \
    WG_DECL_ENTITY_FEATURE(type)                \
    WG_DECL_ENTITY_FEATURE_VECTOR(type)

#define WG_NAME_ENTITY_FEATURE_TRAIT(type) \
    type##FeatureTrait

#define WG_DECL_ENTITY_FEATURE_TRAIT(type) \
    type##FeatureTrait : public EntitySimpleFeatureTrait<type, type##Feature, type##FeatureVector>

}// namespace wmoge