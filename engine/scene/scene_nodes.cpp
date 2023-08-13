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
    Status SceneNodeFolder::accept_visitor(class SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    Status SceneNodeFolder::read_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneNode::read_from_yaml(node)) return StatusCode::Error;

        WG_YAML_READ_AS_OPT(node, "transform", transform);

        return StatusCode::Ok;
    }
    Status SceneNodeFolder::write_to_yaml(YamlNodeRef node) const {
        if (!SceneNode::write_to_yaml(node)) return StatusCode::Error;

        WG_YAML_WRITE_AS(node, "transform", transform);

        return StatusCode::Ok;
    }
    Status SceneNodeFolder::copy_to(Object& other) const {
        SceneNode::copy_to(other);
        auto* ptr      = dynamic_cast<SceneNodeFolder*>(&other);
        ptr->transform = transform;
        return StatusCode::Ok;
    }

    void SceneNodePrefab::register_class() {
        auto* cls = Class::register_class<SceneNodePrefab>();
    }
    Status SceneNodePrefab::accept_visitor(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    Status SceneNodePrefab::read_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneNode::read_from_yaml(node)) return StatusCode::Error;

        StringId prefab_resource;

        WG_YAML_READ_AS_OPT(node, "transform", transform);
        WG_YAML_READ_AS(node, "prefab", prefab_resource);

        prefab = Engine::instance()->resource_manager()->load(prefab_resource).cast<Prefab>();
        if (!prefab) {
            WG_LOG_ERROR("failed to load prefab " << prefab_resource);
            return StatusCode::Error;
        }

        return StatusCode::Ok;
    }
    Status SceneNodePrefab::write_to_yaml(YamlNodeRef node) const {
        if (!SceneNode::write_to_yaml(node)) return StatusCode::Error;

        WG_YAML_WRITE_AS(node, "transform", transform);
        WG_YAML_WRITE_AS(node, "prefab", prefab->get_name());

        return StatusCode::Ok;
    }
    Status SceneNodePrefab::copy_to(Object& other) const {
        SceneNode::copy_to(other);
        auto* ptr      = dynamic_cast<SceneNodePrefab*>(&other);
        ptr->transform = transform;
        ptr->prefab    = prefab;
        return StatusCode::Ok;
    }

    void SceneNodeEntity::register_class() {
        auto* cls = Class::register_class<SceneNodeEntity>();
    }
    Status SceneNodeEntity::accept_visitor(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    Status SceneNodeEntity::read_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneNode::read_from_yaml(node)) return StatusCode::Error;

        WG_YAML_READ_AS_OPT(node, "transform", transform);

        return StatusCode::Ok;
    }
    Status SceneNodeEntity::write_to_yaml(YamlNodeRef node) const {
        if (!SceneNode::write_to_yaml(node)) return StatusCode::Error;

        WG_YAML_WRITE_AS(node, "transform", transform);

        return StatusCode::Ok;
    }
    Status SceneNodeEntity::copy_to(Object& other) const {
        SceneNode::copy_to(other);
        auto* ptr      = dynamic_cast<SceneNodeEntity*>(&other);
        ptr->transform = transform;
        return StatusCode::Ok;
    }

    void SceneNodeComponent::register_class() {
        auto* cls = Class::register_class<SceneNodeComponent>();
    }
    Status SceneNodeComponent::accept_visitor(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }

    void SceneNodeTransform::register_class() {
        auto* cls = Class::register_class<SceneNodeTransform>();
    }
    Status SceneNodeTransform::accept_visitor(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    void SceneNodeTransform::collect_arch(EcsArch& arch) const {
        arch.set_component<EcsComponentSceneTransform>();
    }

    void SceneNodeCamera::register_class() {
        auto* cls = Class::register_class<SceneNodeCamera>();
    }
    Status SceneNodeCamera::read_from_yaml(const YamlConstNodeRef& node) {
        if (!SceneNode::read_from_yaml(node)) return StatusCode::Error;

        WG_YAML_READ_AS_OPT(node, "color", color);
        WG_YAML_READ_AS_OPT(node, "viewport", viewport);
        WG_YAML_READ_AS_OPT(node, "fov", fov);
        WG_YAML_READ_AS_OPT(node, "near", near);
        WG_YAML_READ_AS_OPT(node, "far", far);
        WG_YAML_READ_AS_OPT(node, "target", target);
        WG_YAML_READ_AS_OPT(node, "projection", projection);

        return StatusCode::Ok;
    }
    Status SceneNodeCamera::write_to_yaml(YamlNodeRef node) const {
        if (!SceneNode::write_to_yaml(node)) return StatusCode::Error;

        WG_YAML_WRITE_AS(node, "color", color);
        WG_YAML_WRITE_AS(node, "viewport", viewport);
        WG_YAML_WRITE_AS(node, "fov", fov);
        WG_YAML_WRITE_AS(node, "near", near);
        WG_YAML_WRITE_AS(node, "far", far);
        WG_YAML_WRITE_AS(node, "target", target);
        WG_YAML_WRITE_AS(node, "projection", projection);

        return StatusCode::Ok;
    }
    Status SceneNodeCamera::accept_visitor(SceneTreeVisitor& visitor) {
        return visitor.visit(*this);
    }
    void SceneNodeCamera::collect_arch(EcsArch& arch) const {
        arch.set_component<EcsComponentCamera>();
    }
    Status SceneNodeCamera::copy_to(Object& other) const {
        SceneNode::copy_to(other);
        auto* camera       = dynamic_cast<SceneNodeCamera*>(&other);
        camera->color      = color;
        camera->viewport   = viewport;
        camera->fov        = fov;
        camera->near       = near;
        camera->far        = far;
        camera->target     = target;
        camera->projection = projection;
        return StatusCode::Ok;
    }

}// namespace wmoge