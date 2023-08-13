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
        m_local_to_world.push(LocalToWorldInfo{});
    }
    SceneTreeVisitorEmitScene::~SceneTreeVisitorEmitScene() {
        assert(m_names.size() == 1);
        assert(m_local_to_world.size() == 1);
        assert(m_transforms.empty());
        assert(m_entities.empty());
    }

    Status SceneTreeVisitorEmitScene::visit_begin(SceneNode& node) {
        push_node(node);

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_begin(SceneNodeFolder& node) {
        push_node(node);
        push_local_to_world(node.transform);

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_begin(SceneNodePrefab& node) {
        push_node(node);
        push_local_to_world(node.transform);

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_begin(SceneNodeEntity& node) {
        push_node(node);
        push_local_to_world(node.transform);

        auto* ecs_world = m_scene->get_ecs_world();

        EntityInfo info;
        info.entity_id = ecs_world->allocate_entity();
        info.entity_arch.set_component<EcsComponentLocalToWorld>();
        info.entity_arch.set_component<EcsComponentParent>();
        info.entity_arch.set_component<EcsComponentChildren>();
        info.entity_arch.set_component<EcsComponentName>();

        for (const auto& child : node.get_children()) {
            child->collect_arch(info.entity_arch);
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

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_begin(wmoge::SceneNodeComponent& node) {
        push_node(node);

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_begin(SceneNodeTransform& node) {
        push_node(node);

        // Push transform
        {
            TransformInfo info;
            info.transform = make_ref<SceneTransform>(m_scene->get_transforms());

            if (!m_transforms.empty()) {
                m_transforms.top().transform->add_child(info.transform);
            }
            if (!info.transform->is_linked()) {
                info.transform->set_layer(0);
            }

            info.transform->set_wt(get_l2w(), get_w2l());
            info.transform->update();

            m_transforms.push(info);
        }

        EntityInfo& info = m_entities.top();
        assert(info.entity_id.is_valid());
        assert(info.entity_arch.has_component<EcsComponentSceneTransform>());

        m_scene->get_ecs_world()->get_component_rw<EcsComponentSceneTransform>(info.entity_id).transform = m_transforms.top().transform;

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_begin(SceneNodeCamera& node) {
        if (!visit_begin((SceneNodeComponent&) node)) { return StatusCode::Error; }

        EntityInfo& info = m_entities.top();
        assert(info.entity_id.is_valid());
        assert(info.entity_arch.has_component<EcsComponentCamera>());

        Ref<Camera> camera = m_scene->get_cameras()->make_camera(SID(get_name_full()));
        camera->set_name(SID(get_name_full()));
        camera->set_fov(Math::deg_to_rad(node.fov));
        camera->set_near_far(node.near, node.far);
        camera->set_projection(node.projection);
        camera->set_color(node.color);

        m_scene->get_ecs_world()->get_component_rw<EcsComponentCamera>(info.entity_id).camera = camera;

        return StatusCode::Ok;
    }

    Status SceneTreeVisitorEmitScene::visit_end(SceneNode& node) {
        pop_node(node);

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_end(SceneNodeFolder& node) {
        pop_node(node);
        pop_local_to_world();

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_end(SceneNodePrefab& node) {
        pop_node(node);
        pop_local_to_world();

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_end(SceneNodeEntity& node) {
        assert(!m_entities.empty());
        m_entities.pop();

        pop_node(node);
        pop_local_to_world();

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_end(wmoge::SceneNodeComponent& node) {
        pop_node(node);

        return StatusCode::Ok;
    }
    Status SceneTreeVisitorEmitScene::visit_end(SceneNodeTransform& node) {
        assert(!m_transforms.empty());
        m_transforms.pop();

        return visit_end((SceneNodeComponent&) node);
    }
    Status SceneTreeVisitorEmitScene::visit_end(SceneNodeCamera& node) {
        return visit_end((SceneNodeComponent&) node);
    }

    void SceneTreeVisitorEmitScene::push_node(const SceneNode& node) {
        if (!node.get_name().empty()) {
            NameInfo info{};
            info.name      = node.get_name();
            info.name_full = get_name_full() + "/" + info.name.str();
            m_names.push(info);
        }
    }
    void SceneTreeVisitorEmitScene::pop_node(const SceneNode& node) {
        if (!node.get_name().empty()) {
            assert(m_names.size() > 1);
            m_names.pop();
        }
    }

    void SceneTreeVisitorEmitScene::push_local_to_world(const TransformEdt& transform) {
        LocalToWorldInfo info{};
        info.l2w = get_l2w() * transform.get_transform();
        info.w2l = transform.get_inverse_transform() * get_w2l();
        m_local_to_world.push(info);
    }
    void SceneTreeVisitorEmitScene::pop_local_to_world() {
        assert(m_local_to_world.size() > 1);
        m_local_to_world.pop();
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