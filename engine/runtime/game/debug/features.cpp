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

#include "features.hpp"

#include "ecs/ecs_world.hpp"
#include "game/debug/components.hpp"
#include "scene/scene.hpp"
#include "scene/scene_uuid.hpp"

namespace wmoge {

    Status WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugMesh)::setup_entity_typed(EcsArch& arch, const GmDebugMesh& desc, EntitySetupContext& context) {
        arch.set_component<GmDebugMeshComponent>();
        return WG_OK;
    }

    Status WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugMesh)::build_entity_typed(EcsEntity entity, const GmDebugMesh& desc, EntityBuildContext& context) {
        auto& mesh    = context.world->get_component_rw<GmDebugMeshComponent>(entity);
        mesh.mesh     = desc.mesh;
        mesh.dist_min = desc.dist_min;
        mesh.dist_max = desc.dist_max;
        mesh.color    = desc.color;
        return WG_OK;
    }

    Status WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugLabel)::setup_entity_typed(EcsArch& arch, const GmDebugLabel& desc, EntitySetupContext& context) {
        arch.set_component<GmDebugLabelComponent>();
        return WG_OK;
    }

    Status WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugLabel)::build_entity_typed(EcsEntity entity, const GmDebugLabel& desc, EntityBuildContext& context) {
        auto& label    = context.world->get_component_rw<GmDebugLabelComponent>(entity);
        label.dist_min = desc.dist_min;
        label.dist_max = desc.dist_max;
        label.text     = desc.text;
        label.color    = desc.color;
        label.size     = desc.size;
        return WG_OK;
    }

    Status WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugPrimitive)::setup_entity_typed(EcsArch& arch, const GmDebugPrimitive& desc, EntitySetupContext& context) {
        arch.set_component<GmDebugPrimitiveComponent>();
        return WG_OK;
    }

    Status WG_NAME_ENTITY_FEATURE_TRAIT(GmDebugPrimitive)::build_entity_typed(EcsEntity entity, const GmDebugPrimitive& desc, EntityBuildContext& context) {
        auto& prim    = context.world->get_component_rw<GmDebugPrimitiveComponent>(entity);
        prim.dist_min = desc.dist_min;
        prim.dist_max = desc.dist_max;
        prim.type     = desc.type;
        prim.color    = desc.color;
        return WG_OK;
    }

}// namespace wmoge