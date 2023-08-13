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

#ifndef WMOGE_SCENE_TREE_VISITORS_HPP
#define WMOGE_SCENE_TREE_VISITORS_HPP

#include "scene/scene.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_tree.hpp"
#include "scene/scene_tree_visitor.hpp"

#include <stack>

namespace wmoge {

    /**
     * @class SceneTreeVisitorEmitScene
     * @brief Traverse tree and emit ecs-scene
     */
    class SceneTreeVisitorEmitScene : public SceneTreeVisitorSplit {
    public:
        /**
         * @brief Create visitor for a selected scene
         *
         * @param scene Scene to process
         */
        SceneTreeVisitorEmitScene(const Ref<Scene>& scene);

        ~SceneTreeVisitorEmitScene() override;

        Status visit_begin(SceneNode& node) override;
        Status visit_begin(SceneNodeFolder& node) override;
        Status visit_begin(SceneNodePrefab& node) override;
        Status visit_begin(SceneNodeEntity& node) override;
        Status visit_begin(SceneNodeComponent& node) override;
        Status visit_begin(SceneNodeTransform& node) override;
        Status visit_begin(SceneNodeCamera& node) override;

        Status visit_end(SceneNode& node) override;
        Status visit_end(SceneNodeFolder& node) override;
        Status visit_end(SceneNodePrefab& node) override;
        Status visit_end(SceneNodeEntity& node) override;
        Status visit_end(SceneNodeComponent& node) override;
        Status visit_end(SceneNodeTransform& node) override;
        Status visit_end(SceneNodeCamera& node) override;

        /** @brief Currently processed entity object on scene */
        struct EntityInfo {
            EcsEntity entity_id{};
            EcsArch   entity_arch{};
        };

        /** @brief Chunk of name to append to get full objects names */
        struct NameInfo {
            StringId    name;
            std::string name_full;
        };

        /** @brief Transformation matrices hierarchy */
        struct LocalToWorldInfo {
            Mat4x4f l2w = Math3d::identity();
            Mat4x4f w2l = Math3d::identity();
        };

        /** @brief Node in runtime transform hierarchy */
        struct TransformInfo {
            Ref<SceneTransform> transform;
        };

    private:
        void push_node(const SceneNode& node);
        void pop_node(const SceneNode& node);
        void push_local_to_world(const TransformEdt& transform);
        void pop_local_to_world();

        [[nodiscard]] const std::string& get_name_full() const;
        [[nodiscard]] bool               has_name_full() const;
        [[nodiscard]] Mat4x4f            get_l2w() const;
        [[nodiscard]] Mat4x4f            get_w2l() const;

    private:
        std::stack<EntityInfo>       m_entities;
        std::stack<NameInfo>         m_names;
        std::stack<LocalToWorldInfo> m_local_to_world;
        std::stack<TransformInfo>    m_transforms;

        Ref<Scene> m_scene;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TREE_VISITORS_HPP
