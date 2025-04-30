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

    struct GmDebugMesh {
        WG_RTTI_STRUCT(GmDebugMesh)

        AssetRef<Mesh> mesh;
        Color4f        color;
        int            dist_min = 0;
        int            dist_max = 1000;
    };

    WG_RTTI_STRUCT_BEGIN(GmDebugMesh) {
        WG_RTTI_FIELD(mesh, {RttiOptional});
        WG_RTTI_FIELD(color, {RttiOptional});
        WG_RTTI_FIELD(dist_min, {RttiOptional});
        WG_RTTI_FIELD(dist_max, {RttiOptional});
    }
    WG_RTTI_END;

    class GmDebugLabel {
        WG_RTTI_STRUCT(GmDebugLabel)

        std::string text;
        Color4f     color    = Color::WHITE4f;
        float       size     = 1.0f;
        int         dist_min = 0;
        int         dist_max = 1000;
    };

    WG_RTTI_STRUCT_BEGIN(GmDebugLabel) {
        WG_RTTI_FIELD(text, {RttiOptional});
        WG_RTTI_FIELD(color, {RttiOptional});
        WG_RTTI_FIELD(size, {RttiOptional});
        WG_RTTI_FIELD(dist_min, {RttiOptional});
        WG_RTTI_FIELD(dist_max, {RttiOptional});
    }
    WG_RTTI_END;

    class GmDebugPrimitive {
        WG_RTTI_STRUCT(GmDebugPrimitive)

        GmDebugPrimitiveType type     = GmDebugPrimitiveType::Sphere;
        Color4f              color    = Color::WHITE4f;
        int                  dist_min = 0;
        int                  dist_max = 1000;
    };

    WG_RTTI_STRUCT_BEGIN(GmDebugPrimitive) {
        WG_RTTI_FIELD(type, {RttiOptional});
        WG_RTTI_FIELD(color, {RttiOptional});
        WG_RTTI_FIELD(dist_min, {RttiOptional});
        WG_RTTI_FIELD(dist_max, {RttiOptional});
    }
    WG_RTTI_END;

    WG_DECL_ENTITY_FEATURE_AND_VECTOR(GmDebugMesh);
    WG_DECL_ENTITY_FEATURE_AND_VECTOR(GmDebugLabel);
    WG_DECL_ENTITY_FEATURE_AND_VECTOR(GmDebugPrimitive);

    class WG_DECL_ENTITY_FEATURE_TRAIT(GmDebugMesh) {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmDebugMesh& desc, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmDebugMesh& desc, EntityBuildContext& context) override;
    };

    class WG_DECL_ENTITY_FEATURE_TRAIT(GmDebugLabel) {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmDebugLabel& desc, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmDebugLabel& desc, EntityBuildContext& context) override;
    };

    class WG_DECL_ENTITY_FEATURE_TRAIT(GmDebugPrimitive) {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmDebugPrimitive& desc, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmDebugPrimitive& desc, EntityBuildContext& context) override;
    };

}// namespace wmoge