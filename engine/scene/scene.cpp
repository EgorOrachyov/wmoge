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

#include "scene.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, SceneDataCamera& data) {
        WG_YAML_READ_AS_OPT(node, "color", data.color);
        WG_YAML_READ_AS_OPT(node, "viewport", data.viewport);
        WG_YAML_READ_AS_OPT(node, "fov", data.fov);
        WG_YAML_READ_AS_OPT(node, "near", data.near);
        WG_YAML_READ_AS_OPT(node, "far", data.far);
        WG_YAML_READ_AS_OPT(node, "target", data.target);
        WG_YAML_READ_AS_OPT(node, "projection", data.projection);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const SceneDataCamera& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "color", data.color);
        WG_YAML_WRITE_AS(node, "viewport", data.viewport);
        WG_YAML_WRITE_AS(node, "fov", data.fov);
        WG_YAML_WRITE_AS(node, "near", data.near);
        WG_YAML_WRITE_AS(node, "far", data.far);
        WG_YAML_WRITE_AS(node, "target", data.target);
        WG_YAML_WRITE_AS(node, "projection", data.projection);

        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, SceneData& data) {
        WG_YAML_READ_AS(node, "name", data.name);
        WG_YAML_READ_AS(node, "names", data.names);
        WG_YAML_READ_AS(node, "transforms", data.transforms);
        WG_YAML_READ_AS(node, "cameras", data.cameras);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const SceneData& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "name", data.name);
        WG_YAML_WRITE_AS(node, "names", data.names);
        WG_YAML_WRITE_AS(node, "transforms", data.transforms);
        WG_YAML_WRITE_AS(node, "cameras", data.cameras);

        return StatusCode::Ok;
    }

    Scene::Scene(StringId name) {
        m_name       = name;
        m_transforms = std::make_unique<SceneTransformManager>();
        m_ecs_world  = std::make_unique<EcsWorld>();
        m_cameras    = std::make_unique<CameraManager>();
    }
    void Scene::advance(float delta_time) {
        m_delta_time = delta_time * m_time_factor;
        m_time += m_delta_time;
    }
    void Scene::clear() {
        m_ecs_world->clear();
    }
    const StringId& Scene::get_name() {
        return m_name;
    }
    EcsWorld* Scene::get_ecs_world() {
        return m_ecs_world.get();
    }
    SceneTransformManager* Scene::get_transforms() {
        return m_transforms.get();
    }
    CameraManager* Scene::get_cameras() {
        return m_cameras.get();
    }

}// namespace wmoge