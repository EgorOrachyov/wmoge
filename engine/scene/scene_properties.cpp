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

#include "scene_properties.hpp"

namespace wmoge {

    void ScenePropCamera::collect_arch(EcsArch& arch, SceneNode& owner) {
        arch.set_component<EcsComponentCamera>();
    }
    void ScenePropCamera::on_make_entity(EcsEntity entity, SceneNode& owner) {
        settings.name = owner.get_name();
        owner.get_scene()->add_camera(entity, settings);
    }
    void ScenePropCamera::on_delete_entity(EcsEntity entity, SceneNode& owner) {
        // no special action
    }
    Status ScenePropCamera::copy_to(Object& other) const {
        auto* ptr     = dynamic_cast<ScenePropCamera*>(&other);
        ptr->settings = settings;
        return StatusCode::Ok;
    }
    Status ScenePropCamera::read_from_yaml(const YamlConstNodeRef& node) {
        WG_YAML_READ(node, settings);
        return StatusCode::Ok;
    }
    Status ScenePropCamera::write_to_yaml(YamlNodeRef node) const {
        WG_YAML_MAP(node);
        WG_YAML_WRITE(node, settings);
        return StatusCode::Ok;
    }
    void ScenePropCamera::register_class() {
        auto* cls = Class::register_class<ScenePropCamera>();
    }

    void ScenePropMeshStatic::collect_arch(EcsArch& arch, SceneNode& owner) {
        arch.set_component<EcsComponentMeshStatic>();
    }
    void ScenePropMeshStatic::on_make_entity(EcsEntity entity, SceneNode& owner) {
        owner.get_scene()->add_mesh_static(entity, settings);
    }
    void ScenePropMeshStatic::on_delete_entity(EcsEntity entity, SceneNode& owner) {
        // no special action
    }
    Status ScenePropMeshStatic::copy_to(Object& other) const {
        auto* ptr     = dynamic_cast<ScenePropMeshStatic*>(&other);
        ptr->settings = settings;
        return StatusCode::Ok;
    }
    Status ScenePropMeshStatic::read_from_yaml(const YamlConstNodeRef& node) {
        WG_YAML_READ(node, settings);
        return StatusCode::Ok;
    }
    Status ScenePropMeshStatic::write_to_yaml(YamlNodeRef node) const {
        WG_YAML_MAP(node);
        WG_YAML_WRITE(node, settings);
        return StatusCode::Ok;
    }
    void ScenePropMeshStatic::register_class() {
        auto* cls = Class::register_class<ScenePropMeshStatic>();
    }

    void ScenePropAudioSource::register_class() {
        auto* cls = Class::register_class<ScenePropAudioSource>();
    }

    void ScenePropAudioListener::register_class() {
        auto* cls = Class::register_class<ScenePropAudioListener>();
    }

    void ScenePropLuaScript::register_class() {
        auto* cls = Class::register_class<ScenePropLuaScript>();
    }

}// namespace wmoge