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

#pragma once

#include "asset/asset_ref.hpp"
#include "core/array_view.hpp"
#include "core/class.hpp"
#include "core/object.hpp"
#include "core/uuid.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "event/event_scene.hpp"
#include "io/serialization.hpp"
#include "math/math_utils3d.hpp"
#include "math/transform.hpp"
#include "scene/scene_entity.hpp"

#include <functional>
#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @brief Type of node in the tree (hint for editor in most cases)
     */
    enum class SceneNodeType {
        Object = 0,
        Folder,
        Layer,
        Prefabed
    };

    /**
     * @class SceneNodeProp
     * @brief An property of the node which can be assigned to node to customize it
    */
    class SceneNodeProp : public Object {
    public:
        WG_OBJECT(SceneNodeProp, Object)

        virtual void fill_arch(EcsArch& arch) {}
        virtual void add_components(Entity entity, Entity parent) {}
        virtual void process_event(const EventSceneNode& event) {}

        void                           set_node(class SceneNode* node) { m_node = node; }
        [[nodiscard]] bool             has_node() const { return m_node != nullptr; }
        [[nodiscard]] class SceneNode* get_node() const { return m_node; }

    private:
        class SceneNode* m_node = nullptr;
    };

    /**
     * @class SceneNodeData
     * @brief Serializable struct with scene tree single node data
     */
    struct SceneNodeData {
        Strid                           name;
        UUID                            uuid;
        SceneNodeType                   type;
        TransformEdt                    transform;
        AssetId                         prefab;
        std::vector<Ref<SceneNodeProp>> properties;
        std::optional<UUID>             parent;

        WG_IO_DECLARE(SceneNodeData);
    };

    /**
     * @class SceneNodesData
     * @brief Serializable hierarchy of nodes
     */
    struct SceneNodesData {
        std::vector<SceneNodeData> nodes;

        WG_IO_DECLARE(SceneNodesData);
    };

    /**
     * @class SceneNode
     * @brief Represents single node (object) in an editable tree hierarchy of scene objects
     * 
     * Node represents a single game object (entity) in a hierachy of a scene objects.
     * Object has name (not required to unique), full path, which depend on its hierarchy, 
     * editable transform in a transform hierarchy on nodes, and an optional set of object
     * properties.
     * 
     * Each property describes an unique visual, audio, behaivour of the object. Properties
     * are automatically serizalized and translated into optimized ecs components and runtime
     * scene.
     * 
     * @see SceneProperty
     * @see SceneTree
     * @see ScenePrefab
     */
    class SceneNode : public Object {
    public:
        WG_OBJECT(SceneNode, Object)

        /**
         * @brief Creates new scene node with desired name and type (hint)
         * 
         * @param name Node name to show in editor
         * @param type Node type to hint in editor 
         */
        SceneNode(const Strid& name, SceneNodeType type);

        Status                          build(const std::vector<SceneNodeData>& nodes);
        Status                          dump(std::vector<SceneNodeData>& nodes);
        void                            enter_tree(class SceneTree* tree);
        void                            exit_tree();
        void                            process_event(const EventSceneNode& event);
        void                            dispatch_to_props(const EventSceneNode& event);
        void                            dispatch_to_children(const EventSceneNode& event);
        void                            set_name(const Strid& name);
        void                            set_uuid(const UUID& uuid);
        void                            set_transform(const TransformEdt& transform);
        void                            set_properties(std::vector<Ref<SceneNodeProp>> props);
        void                            add_child(const Ref<SceneNode>& child);
        void                            remove_child(const Ref<SceneNode>& child);
        bool                            is_child(const Ref<SceneNode>& node) const;
        bool                            contains(const Ref<SceneNode>& node) const;
        void                            each(const std::function<void(const Ref<SceneNode>&)>& visitor);
        std::optional<Ref<SceneNode>>   find_child(const std::string& name);
        std::optional<Ref<SceneNode>>   find_child_recursive(const std::string& path);
        std::vector<Ref<SceneNodeProp>> copy_properties() const;
        std::vector<Ref<SceneNode>>     get_nodes();
        bool                            has_parent() const;
        bool                            has_prefab() const;
        bool                            has_entity() const;
        bool                            has_tree() const;
        bool                            has_scene() const;
        Entity                          instantiate_entity(class Scene* scene, Entity parent);

        [[nodiscard]] array_view<const Ref<SceneNode>>     get_children() const { return m_children; }
        [[nodiscard]] array_view<const Ref<SceneNodeProp>> get_properties() const { return m_properties; }
        [[nodiscard]] class SceneNode*                     get_parent() const { return m_parent; }
        [[nodiscard]] const Strid&                         get_name() const { return m_name; }
        [[nodiscard]] const std::string&                   get_path() const { return m_path; }
        [[nodiscard]] const UUID&                          get_uuid() const { return m_uuid; }
        [[nodiscard]] const Ref<class ScenePrefab>&        get_prefab() const { return m_prefab; }
        [[nodiscard]] const SceneNodeType                  get_type() const { return m_type; }
        [[nodiscard]] const TransformEdt&                  get_transform() const { return m_transform; }
        [[nodiscard]] const Mat4x4f&                       get_l2w() const { return m_l2w; }
        [[nodiscard]] const Mat4x4f&                       get_w2l() const { return m_w2l; }
        [[nodiscard]] const Entity&                        get_entity() const { return m_entity; }
        [[nodiscard]] class SceneTree*                     get_tree() const { return m_tree; }
        [[nodiscard]] class Scene*                         get_scene() const;

        Status copy_to(Object& other) const override;

    private:
        std::vector<Ref<SceneNode>>     m_children;
        std::vector<Ref<SceneNodeProp>> m_properties;
        SceneNode*                      m_parent = nullptr;
        Strid                           m_name   = SID("");
        std::string                     m_path;
        UUID                            m_uuid = UUID::generate();
        Ref<class ScenePrefab>          m_prefab;
        SceneNodeType                   m_type = SceneNodeType::Object;
        TransformEdt                    m_transform;               // local editable transform
        Mat4x4f                         m_l2w = Math3d::identity();// cached in hier
        Mat4x4f                         m_w2l = Math3d::identity();// cached in hier
        Entity                          m_entity;
        class SceneTree*                m_tree = nullptr;
    };

}// namespace wmoge