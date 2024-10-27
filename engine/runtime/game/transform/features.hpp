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
#include "math/transform.hpp"
#include "scene/scene_feature_adapter.hpp"

#include <optional>

namespace wmoge {

    enum class GmTransformType {
        MovableHierarchical,
        Movable,
        NonMovable
    };

    class GmTransformFeature : public EntityFeature {
        WG_RTTI_CLASS(GmTransformFeature, EntityFeature)

        TransformEdt        transform;
        GmTransformType     type = GmTransformType::NonMovable;
        std::optional<UUID> parent;
        std::vector<UUID>   children;
    };

    WG_RTTI_CLASS_BEGIN(GmTransformFeature) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(transform, {RttiOptional});
        WG_RTTI_FIELD(type, {RttiOptional});
        WG_RTTI_FIELD(parent, {RttiOptional});
        WG_RTTI_FIELD(children, {RttiOptional});
    }
    WG_RTTI_END;

    class GmTransformFeatureTrait : public EntitySimpleFeatureTrait<GmTransformFeature> {
    public:
        Status setup_entity_typed(EcsArch& arch, const GmTransformFeature& feature, EntitySetupContext& context) override;
        Status build_entity_typed(EcsEntity entity, const GmTransformFeature& feature, EntityBuildContext& context) override;
    };

}// namespace wmoge