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

#include "scene_node_props.hpp"

#include "core/class.hpp"

namespace wmoge {

    void NodePropSpatial::fill_arch(EcsArch& arch) {
        arch.set_component<EcsComponentParent>();
        arch.set_component<EcsComponentChildren>();
        arch.set_component<EcsComponentTransform>();
        arch.set_component<EcsComponentTransformUpd>();
        arch.set_component<EcsComponentLocalToWorld>();
        arch.set_component<EcsComponentWorldToLocal>();
    }

    void NodePropSpatial::add_components(Entity entity, Entity parent) {
        SceneNode* node = get_node();

        if (parent.is_valid()) {
            auto& ecs_parent  = entity.get_component<EcsComponentParent>();
            ecs_parent.parent = parent.get_ecs_id();

            if (parent.has_component<EcsComponentChildren>()) {
                auto& ecs_children = parent.get_component<EcsComponentChildren>();
                ecs_children.children.push_back(entity.get_ecs_id());
            }
            if (parent.has_component<EcsComponentTransformUpd>()) {
                auto& ecs_parent_transform_upd  = parent.get_component<EcsComponentTransformUpd>();
                auto& ecs_self_transform_upd    = entity.get_component<EcsComponentTransformUpd>();
                ecs_self_transform_upd.batch_id = ecs_parent_transform_upd.batch_id + 1;
            }
        }

        auto& ecs_transform     = entity.get_component<EcsComponentTransform>();
        ecs_transform.transform = node->get_transform().to_transform3d();
    }

    void NodePropSpatial::process_event(const EventSceneNode& event) {
        SceneNode* node   = get_node();
        Entity     entity = node->get_entity();

        if (event.notification == SceneNodeNotification::TransformUpdated) {
            auto& ecs_transform     = entity.get_component<EcsComponentTransform>();
            ecs_transform.transform = node->get_transform().to_transform3d();

            auto& ecs_transform_upd    = entity.get_component<EcsComponentTransformUpd>();
            ecs_transform_upd.is_dirty = true;
        }
    }

    Status NodePropSpatial::read_from_yaml(const YamlConstNodeRef& node) {
        WG_YAML_READ(node, params);
        return StatusCode::Ok;
    }

    void NodePropSpatial::register_class() {
        auto* cls = Class::register_class<NodePropSpatial>();
    }

    void NodePropCamera::fill_arch(EcsArch& arch) {
        arch.set_component<EcsComponentCamera>();
    }

    void NodePropCamera::add_components(Entity entity, Entity parent) {
        params.fill(entity.get_component<EcsComponentCamera>());
    }

    Status NodePropCamera::read_from_yaml(const YamlConstNodeRef& node) {
        WG_YAML_READ(node, params);
        return StatusCode::Ok;
    }

    void NodePropCamera::register_class() {
        auto* cls = Class::register_class<NodePropCamera>();
    }

}// namespace wmoge