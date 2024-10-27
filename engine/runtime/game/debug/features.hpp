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

#include "core/uuid.hpp"
#include "game/debug/components.hpp"
#include "material/material.hpp"
#include "math/color.hpp"
#include "math/transform.hpp"
#include "mesh/mesh.hpp"
#include "scene/scene_feature_adapter.hpp"

#include <optional>

namespace wmoge {

    class GmDebugMeshFeature : public EntityFeature {
        WG_RTTI_CLASS(GmDebugMeshFeature, EntityFeature)

        AssetRef<Mesh>     mesh;
        AssetRef<Material> material;
        int                dist_min = 0;
        int                dist_max = 1000;
    };

    WG_RTTI_CLASS_BEGIN(GmDebugMeshFeature) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(mesh, {RttiOptional});
        WG_RTTI_FIELD(material, {RttiOptional});
        WG_RTTI_FIELD(dist_min, {RttiOptional});
        WG_RTTI_FIELD(dist_max, {RttiOptional});
    }
    WG_RTTI_END;

    class GmDebugLabelFeature : public EntityFeature {
        WG_RTTI_CLASS(GmDebugLabelFeature, EntityFeature)

        std::string text;
        Color4f     color    = Color::WHITE4f;
        float       size     = 1.0f;
        int         dist_min = 0;
        int         dist_max = 1000;
    };

    WG_RTTI_CLASS_BEGIN(GmDebugLabelFeature) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(text, {RttiOptional});
        WG_RTTI_FIELD(color, {RttiOptional});
        WG_RTTI_FIELD(size, {RttiOptional});
        WG_RTTI_FIELD(dist_min, {RttiOptional});
        WG_RTTI_FIELD(dist_max, {RttiOptional});
    }
    WG_RTTI_END;

    class GmDebugPrimitiveFeature : public EntityFeature {
        WG_RTTI_CLASS(GmDebugPrimitiveFeature, EntityFeature)

        GmDebugPrimitiveType type     = GmDebugPrimitiveType::Sphere;
        Color4f              color    = Color::WHITE4f;
        int                  dist_min = 0;
        int                  dist_max = 1000;
    };

    WG_RTTI_CLASS_BEGIN(GmDebugPrimitiveFeature) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(type, {RttiOptional});
        WG_RTTI_FIELD(color, {RttiOptional});
        WG_RTTI_FIELD(dist_min, {RttiOptional});
        WG_RTTI_FIELD(dist_max, {RttiOptional});
    }
    WG_RTTI_END;

    class GmDebugMeshFeatureTrait : public EntitySimpleFeatureTrait<GmDebugMeshFeature> {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmDebugMeshFeature& feature, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmDebugMeshFeature& feature, EntityBuildContext& context) override;
    };

    class GmDebugLabelFeatureTrait : public EntitySimpleFeatureTrait<GmDebugLabelFeature> {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmDebugLabelFeature& feature, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmDebugLabelFeature& feature, EntityBuildContext& context) override;
    };

    class GmDebugPrimitiveFeatureTrait : public EntitySimpleFeatureTrait<GmDebugPrimitiveFeature> {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmDebugPrimitiveFeature& feature, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmDebugPrimitiveFeature& feature, EntityBuildContext& context) override;
    };

}// namespace wmoge