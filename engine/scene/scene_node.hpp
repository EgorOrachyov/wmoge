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

#ifndef WMOGE_SCENE_NODE_HPP
#define WMOGE_SCENE_NODE_HPP

#include "core/array_view.hpp"
#include "core/class.hpp"
#include "core/object.hpp"
#include "core/uuid.hpp"
#include "ecs/ecs_core.hpp"
#include "ecs/ecs_entity.hpp"
#include "io/yaml.hpp"
#include "math/transform.hpp"
#include "scene/scene_property.hpp"
#include "scene/scene_transform.hpp"

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
     * @see Prefab
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
        SceneNode(const StringId& name, SceneNodeType type);

        void                               set_name(const StringId& name);
        void                               set_uuid(const UUID& uuid);
        void                               set_transform(const TransformEdt& transform);
        void                               set_properties(std::vector<Ref<SceneProperty>> props);
        void                               set_tree(class SceneTree* tree);
        void                               add_child(const Ref<SceneNode>& child);
        void                               remove_child(const Ref<SceneNode>& child);
        bool                               is_child(const Ref<SceneNode>& node) const;
        bool                               contains(const Ref<SceneNode>& node) const;
        void                               each(const std::function<void(const Ref<SceneNode>&)>& visitor);
        std::optional<Ref<SceneNode>>      find_child(const std::string& name);
        std::optional<Ref<SceneNode>>      find_child_recursive(const std::string& path);
        std::vector<Ref<SceneProperty>>    copy_properties() const;
        bool                               has_parent() const;
        bool                               has_prefab() const;
        bool                               has_entity() const;
        bool                               has_tree() const;
        bool                               has_scene() const;
        std::optional<Ref<SceneTransform>> get_hier_transform() const;
        std::optional<Ref<SceneTransform>> get_hier_transform_parent() const;

        [[nodiscard]] ArrayView<const Ref<SceneNode>>     get_children() const { return m_children; }
        [[nodiscard]] ArrayView<const Ref<SceneProperty>> get_properties() const { return m_properties; }
        [[nodiscard]] class SceneNode*                    get_parent() const { return m_parent; }
        [[nodiscard]] const StringId&                     get_name() const { return m_name; }
        [[nodiscard]] const std::string&                  get_path() const { return m_path; }
        [[nodiscard]] const UUID&                         get_uuid() const { return m_uuid; }
        [[nodiscard]] const Ref<class Prefab>&            get_prefab() const { return m_prefab; }
        [[nodiscard]] const SceneNodeType                 get_type() const { return m_type; }
        [[nodiscard]] const TransformEdt&                 get_transform() const { return m_transform; }
        [[nodiscard]] const EcsEntity&                    get_entity() const { return m_entity; }
        [[nodiscard]] class SceneTree*                    get_tree() const { return m_tree; }
        [[nodiscard]] class Scene*                        get_scene() const;

        Status copy_to(Object& other) const override;

    private:
        friend class SceneTree;
        void make_entity();
        void delete_entity();
        void remake_entity();
        void sync_name();
        void sync_transform();

    private:
        std::vector<Ref<SceneNode>>     m_children;
        std::vector<Ref<SceneProperty>> m_properties;
        SceneNode*                      m_parent = nullptr;
        StringId                        m_name   = SID("");
        std::string                     m_path;
        UUID                            m_uuid = UUID::generate();
        Ref<class Prefab>               m_prefab;
        SceneNodeType                   m_type = SceneNodeType::Object;
        TransformEdt                    m_transform;
        EcsEntity                       m_entity;
        class SceneTree*                m_tree = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_NODE_HPP
