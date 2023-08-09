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

#include "scene_tree_visitors.hpp"

namespace wmoge {

    SceneTreeVisitorEmitScene::SceneTreeVisitorEmitScene(const Ref<Scene>& scene) : m_scene(scene) {
        m_names.push(NameInfo{});
        m_entities.push(EntityInfo{});
        m_local_to_world.push(LocalToWorldInfo{});
    }
    SceneTreeVisitorEmitScene::~SceneTreeVisitorEmitScene() {
        assert(m_names.size() == 1);
        assert(m_entities.size() == 1);
        assert(m_local_to_world.size() == 1);
        assert(m_transforms.empty());
    }

    bool SceneTreeVisitorEmitScene::visit_begin(SceneNode& node) {
        if (!node.get_name().empty()) {
            NameInfo info{};
            info.name      = node.get_name();
            info.name_full = get_name_full() + "/" + info.name.str();
            m_names.push(info);
        }

        LocalToWorldInfo info{};
        info.l2w = get_l2w() * node.get_transform().get_transform();
        info.w2l = node.get_transform().get_inverse_transform() * get_w2l();
        m_local_to_world.push(info);

        return true;
    }
    bool SceneTreeVisitorEmitScene::visit_begin(SceneNodeFolder& node) {
        return visit_begin((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_begin(SceneNodeTransform& node) {
        if (!visit_begin((SceneNode&) node)) { return false; }

        TransformInfo info;
        info.transform = make_ref<SceneTransform>(m_scene->get_transforms());
        info.transform->set_transform(node.get_transform().get_transform(), node.get_transform().get_inverse_transform());

        if (!m_transforms.empty()) {
            m_transforms.top().transform->add_child(info.transform);
        }

        m_transforms.push(info);

        return true;
    }
    bool SceneTreeVisitorEmitScene::visit_begin(SceneNodePrefab& node) {
        return false;
    }
    bool SceneTreeVisitorEmitScene::visit_begin(SceneNodeEntity& node) {
        if (!visit_begin((SceneNode&) node)) { return false; }

        auto* ecs_world = m_scene->get_ecs_world();

        EntityInfo info;
        info.entity_id = ecs_world->allocate_entity();
        info.entity_arch.set_component<EcsComponentLocalToWorld>();
        info.entity_arch.set_component<EcsComponentParent>();
        info.entity_arch.set_component<EcsComponentChildren>();
        info.entity_arch.set_component<EcsComponentName>();

        if (!m_transforms.empty()) {
            info.entity_arch.set_component<EcsComponentSceneTransform>();
        }

        for (const auto& child : node.get_children()) {
            info.entity_arch |= child->get_arch();
        }

        ecs_world->make_entity(info.entity_id, info.entity_arch);

        if (!m_entities.empty()) {
            const auto parent_entity_id = m_entities.top().entity_id;
            const auto child_entity_id  = info.entity_id;

            ecs_world->get_component_rw<EcsComponentParent>(child_entity_id).parent = parent_entity_id;
            ecs_world->get_component_rw<EcsComponentChildren>(parent_entity_id).children.push_back(child_entity_id);
        }

        if (!m_transforms.empty()) {
            const auto entity_id = info.entity_id;
            auto&      transform = m_transforms.top().transform;

            ecs_world->get_component_rw<EcsComponentSceneTransform>(entity_id).transform = transform;
        }

        if (m_local_to_world.empty()) {
            const auto entity_id = info.entity_id;
            const auto l2w       = get_l2w();

            ecs_world->get_component_rw<EcsComponentLocalToWorld>(entity_id).matrix = l2w;
        }

        if (has_name_full()) {
            const auto entity_id = info.entity_id;

            ecs_world->get_component_rw<EcsComponentName>(entity_id).name = get_name_full();
        }

        m_entities.push(info);

        return true;
    }
    bool SceneTreeVisitorEmitScene::visit_begin(wmoge::SceneNodeComponent& node) {
        assert(!m_entities.empty());

        return visit_begin((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_begin(SceneNodeCamera& node) {
        if (!visit_begin((SceneNodeComponent&) node)) { return false; }

        EntityInfo& info = m_entities.top();
        assert(info.entity_id.is_valid());
        assert(info.entity_arch.has_component<EcsComponentCamera>());

        Ref<Camera> camera = make_ref<Camera>(m_scene->get_cameras());
        camera->set_name(SID(get_name_full()));
        camera->set_fov(node.fov);
        camera->set_near_far(node.near, node.far);
        camera->set_projection(node.projection);
        camera->set_color(node.color);

        m_scene->get_ecs_world()->get_component_rw<EcsComponentCamera>(info.entity_id).camera = camera;

        return true;
    }

    bool SceneTreeVisitorEmitScene::visit_end(SceneNode& node) {
        if (!node.get_name().empty()) {
            assert(m_names.size() > 1);
            m_names.pop();
        }

        assert(m_local_to_world.size() > 1);
        m_local_to_world.pop();

        return true;
    }
    bool SceneTreeVisitorEmitScene::visit_end(SceneNodeFolder& node) {
        return visit_end((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_end(SceneNodeTransform& node) {
        assert(!m_transforms.empty());
        m_transforms.pop();

        return visit_end((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_end(SceneNodePrefab& node) {
        return visit_end((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_end(SceneNodeEntity& node) {
        assert(!m_entities.empty());
        m_entities.pop();

        return visit_end((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_end(wmoge::SceneNodeComponent& node) {
        return visit_end((SceneNode&) node);
    }
    bool SceneTreeVisitorEmitScene::visit_end(SceneNodeCamera& node) {
        return visit_end((SceneNodeComponent&) node);
    }

    const std::string& SceneTreeVisitorEmitScene::get_name_full() const {
        assert(!m_names.empty());
        return m_names.top().name_full;
    }
    bool SceneTreeVisitorEmitScene::has_name_full() const {
        assert(!m_names.empty());
        return !m_names.top().name_full.empty();
    }
    Mat4x4f SceneTreeVisitorEmitScene::get_l2w() const {
        assert(!m_local_to_world.empty());
        return m_local_to_world.top().l2w;
    }
    Mat4x4f SceneTreeVisitorEmitScene::get_w2l() const {
        assert(!m_local_to_world.empty());
        return m_local_to_world.top().w2l;
    }

}// namespace wmoge