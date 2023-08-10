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

#include "scene_nodes.hpp"

#include "core/engine.hpp"
#include "io/yaml.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_tree_visitor.hpp"

namespace wmoge {

    void SceneNodeFolder::register_class() {
        auto* cls = Class::register_class<SceneNodeFolder>();
    }
    bool SceneNodeFolder::on_visit(class SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    bool SceneNodeFolder::on_yaml_read(const YamlConstNodeRef& node) {
        if (!SceneNode::on_yaml_read(node)) return false;

        WG_YAML_READ_AS_OPT(node, "transform", transform);

        return true;
    }
    bool SceneNodeFolder::on_yaml_write(YamlNodeRef node) const {
        if (!SceneNode::on_yaml_write(node)) return false;

        WG_YAML_WRITE_AS(node, "transform", transform);

        return true;
    }
    void SceneNodeFolder::copy_to(SceneNode& other) const {
        SceneNode::copy_to(other);
        auto* ptr      = dynamic_cast<SceneNodeFolder*>(&other);
        ptr->transform = transform;
    }

    void SceneNodePrefab::register_class() {
        auto* cls = Class::register_class<SceneNodePrefab>();
    }
    bool SceneNodePrefab::on_visit(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    bool SceneNodePrefab::on_yaml_read(const YamlConstNodeRef& node) {
        if (!SceneNode::on_yaml_read(node)) return false;

        StringId prefab_resource;

        WG_YAML_READ_AS_OPT(node, "transform", transform);
        WG_YAML_READ_AS(node, "prefab", prefab_resource);

        prefab = Engine::instance()->resource_manager()->load(prefab_resource).cast<Prefab>();
        if (!prefab) {
            WG_LOG_ERROR("failed to load prefab " << prefab_resource);
            return false;
        }

        return true;
    }
    bool SceneNodePrefab::on_yaml_write(YamlNodeRef node) const {
        if (!SceneNode::on_yaml_write(node)) return false;

        WG_YAML_WRITE_AS(node, "transform", transform);
        WG_YAML_WRITE_AS(node, "prefab", prefab->get_name());

        return true;
    }
    void SceneNodePrefab::copy_to(SceneNode& other) const {
        SceneNode::copy_to(other);
        auto* ptr      = dynamic_cast<SceneNodePrefab*>(&other);
        ptr->transform = transform;
        ptr->prefab    = prefab;
    }

    void SceneNodeEntity::register_class() {
        auto* cls = Class::register_class<SceneNodeEntity>();
    }
    bool SceneNodeEntity::on_visit(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    bool SceneNodeEntity::on_yaml_read(const YamlConstNodeRef& node) {
        if (!SceneNode::on_yaml_read(node)) return false;

        WG_YAML_READ_AS_OPT(node, "transform", transform);

        return true;
    }
    bool SceneNodeEntity::on_yaml_write(YamlNodeRef node) const {
        if (!SceneNode::on_yaml_write(node)) return false;

        WG_YAML_WRITE_AS(node, "transform", transform);

        return true;
    }
    void SceneNodeEntity::copy_to(SceneNode& other) const {
        SceneNode::copy_to(other);
        auto* ptr      = dynamic_cast<SceneNodeEntity*>(&other);
        ptr->transform = transform;
    }

    void SceneNodeComponent::register_class() {
        auto* cls = Class::register_class<SceneNodeComponent>();
    }
    bool SceneNodeComponent::on_visit(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }

    void SceneNodeTransform::register_class() {
        auto* cls = Class::register_class<SceneNodeTransform>();
    }
    bool SceneNodeTransform::on_visit(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    void SceneNodeTransform::on_ecs_arch_collect(EcsArch& arch) const {
        arch.set_component<EcsComponentSceneTransform>();
    }

    void SceneNodeCamera::register_class() {
        auto* cls = Class::register_class<SceneNodeCamera>();
    }
    bool SceneNodeCamera::on_yaml_read(const YamlConstNodeRef& node) {
        if (!SceneNode::on_yaml_read(node)) return false;

        WG_YAML_READ_AS_OPT(node, "color", color);
        WG_YAML_READ_AS_OPT(node, "viewport", viewport);
        WG_YAML_READ_AS_OPT(node, "fov", fov);
        WG_YAML_READ_AS_OPT(node, "near", near);
        WG_YAML_READ_AS_OPT(node, "far", far);
        WG_YAML_READ_AS_OPT(node, "target", target);
        WG_YAML_READ_AS_OPT(node, "projection", projection);

        return true;
    }
    bool SceneNodeCamera::on_yaml_write(YamlNodeRef node) const {
        if (!SceneNode::on_yaml_write(node)) return false;

        WG_YAML_WRITE_AS(node, "color", color);
        WG_YAML_WRITE_AS(node, "viewport", viewport);
        WG_YAML_WRITE_AS(node, "fov", fov);
        WG_YAML_WRITE_AS(node, "near", near);
        WG_YAML_WRITE_AS(node, "far", far);
        WG_YAML_WRITE_AS(node, "target", target);
        WG_YAML_WRITE_AS(node, "projection", projection);

        return true;
    }
    bool SceneNodeCamera::on_visit(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    void SceneNodeCamera::on_ecs_arch_collect(EcsArch& arch) const {
        arch.set_component<EcsComponentCamera>();
    }
    void SceneNodeCamera::copy_to(SceneNode& other) const {
        SceneNode::copy_to(other);
        auto* camera       = dynamic_cast<SceneNodeCamera*>(&other);
        camera->color      = color;
        camera->viewport   = viewport;
        camera->fov        = fov;
        camera->near       = near;
        camera->far        = far;
        camera->target     = target;
        camera->projection = projection;
    }

}// namespace wmoge