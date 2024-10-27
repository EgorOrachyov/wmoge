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
#include "game/transform/components.hpp"
#include "scene/scene.hpp"
#include "scene/scene_uuid.hpp"

namespace wmoge {

    Status GmTransformFeatureTrait::setup_entity_typed(EcsArch& arch, const GmTransformFeature& feature, EntitySetupContext& context) {
        if (feature.type == GmTransformType::MovableHierarchical) {
            arch.set_component<GmParentComponent>();
            arch.set_component<GmChildrenComponent>();
            arch.set_component<GmMatLocalComponent>();
            arch.set_component<GmMatLocalToWorldComponent>();
            arch.set_component<GmTransformComponent>();
            arch.set_component<GmTransformFrameComponent>();
        }
        if (feature.type == GmTransformType::Movable) {
            arch.set_component<GmMatLocalComponent>();
            arch.set_component<GmMatLocalToWorldComponent>();
            arch.set_component<GmTransformComponent>();
            arch.set_component<GmTransformFrameComponent>();
        }
        if (feature.type == GmTransformType::NonMovable) {
            arch.set_component<GmMatLocalToWorldComponent>();
        }

        return WG_OK;
    }

    Status GmTransformFeatureTrait::build_entity_typed(EcsEntity entity, const GmTransformFeature& feature, EntityBuildContext& context) {
        EcsWorld*     world = context.world;
        SceneUuidMap* uuid  = context.uuid;

        if (feature.type == GmTransformType::MovableHierarchical) {
            auto& children = world->get_component_rw<GmChildrenComponent>(entity);
            auto& mat_l    = world->get_component_rw<GmMatLocalComponent>(entity);
            auto& trsf     = world->get_component_rw<GmTransformComponent>(entity);

            trsf.t  = feature.transform.to_transform3d();
            mat_l.m = Math3d ::to_m3x4f(trsf.t.to_mat4x4());

            if (feature.parent) {
                auto& parent = world->get_component_rw<GmParentComponent>(entity);
                parent.id    = uuid->find_entity(*feature.parent).value_or(EcsEntity());
            }
            if (!feature.children.empty()) {
                auto& children = world->get_component_rw<GmChildrenComponent>(entity);
                children.ids.reserve(feature.children.size());
                for (const UUID id : feature.children) {
                    if (auto entity = uuid->find_entity(id)) {
                        children.ids.push_back(*entity);
                    }
                }
            }
        }
        if (feature.type == GmTransformType::Movable) {
            auto& mat_l = world->get_component_rw<GmMatLocalComponent>(entity);
            auto& trsf  = world->get_component_rw<GmTransformComponent>(entity);

            trsf.t  = feature.transform.to_transform3d();
            mat_l.m = Math3d ::to_m3x4f(trsf.t.to_mat4x4());
        }
        if (feature.type == GmTransformType::NonMovable) {
            auto& mat_l2w = world->get_component_rw<GmMatLocalToWorldComponent>(entity);
            mat_l2w.m     = Math3d ::to_m3x4f(feature.transform.to_mat4x4());
        }

        return WG_OK;
    }

}// namespace wmoge