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

#ifndef WMOGE_SCENE_TREE_HPP
#define WMOGE_SCENE_TREE_HPP

#include "core/object.hpp"
#include "core/string_id.hpp"
#include "io/yaml.hpp"
#include "scene/scene.hpp"
#include "scene/scene_node.hpp"
#include "scene/scene_properties.hpp"
#include "scene/scene_property.hpp"

#include <functional>
#include <optional>
#include <string>

namespace wmoge {

    /**
     * @class SceneNodeData
     * @brief Serializable struct with scene tree single node data
     */
    struct SceneNodeData {
        StringId                        name;
        UUID                            uuid;
        SceneNodeType                   type;
        TransformEdt                    transform;
        ResourceId                      prefab;
        std::vector<Ref<SceneProperty>> properties;
        std::optional<UUID>             parent;

        friend Status yaml_read(const YamlConstNodeRef& node, SceneNodeData& data);
        friend Status yaml_write(YamlNodeRef node, const SceneNodeData& data);
    };

    /**
     * @class SceneTreeData
     * @brief Serializable struct with an editable scene tree data
     */
    struct SceneTreeData {
        std::vector<SceneNodeData> nodes;
        GraphicsPipelineSettings   pipeline_settings;

        friend Status yaml_read(const YamlConstNodeRef& node, SceneTreeData& data);
        friend Status yaml_write(YamlNodeRef node, const SceneTreeData& data);
    };

    /**
     * @class SceneTree
     * @brief Editable tree of a scene objects
     *
     * Scene tree represents a hierarchy of a scene nodes or a game objects.
     * It allows to add, remove, rename, copy, paste, move nodes, edit their
     * properties, instantiate prefabs and so on. Each scene node stores additional
     * meta information, which is used only for editing. Internally, all the data
     * for a runtime is stored in a Scene.
     *
     * When the game is started, scene tree emits a scene object, which is a high-performance
     * and low overhead container with entities for a runtime. Meta information, nodes,
     * hierarchy, etc. is not presented in a final game for a speed and memory reasons.
     *
     * @see Scene
     * @see SceneNode
     * @see SceneTreePacked
     */
    class SceneTree : public Object {
    public:
        WG_OBJECT(SceneTree, Object)

        /**
         * @brief Creates new editable scene
         * 
         * Constructor creates an empty tree with a hidden root inside.
         * Also a runtime representation of a tree is allocated for 
         * tree interactive editing and dispaly at editor. 
         * 
         * @param name Scene name to unique identify it at editor and runtime
         */
        SceneTree(const StringId& name);

        void                          sync();
        void                          each(const std::function<void(const Ref<SceneNode>&)>& visitor);
        bool                          contains(const Ref<SceneNode>& node) const;
        std::optional<Ref<SceneNode>> find_node(const std::string& path);
        std::vector<Ref<SceneNode>>   get_nodes();
        std::vector<Ref<SceneNode>>   filter_nodes(const std::function<bool(const Ref<SceneNode>&)>& predicate);
        Status                        build(const SceneTreeData& data);
        Status                        dump(SceneTreeData& data);

        [[nodiscard]] const StringId&       get_name() const { return m_name; }
        [[nodiscard]] const Ref<SceneNode>& get_root() const { return m_root; }
        [[nodiscard]] const Ref<Scene>&     get_scene() const { return m_scene; }

    private:
        StringId       m_name;
        Ref<SceneNode> m_root;
        Ref<Scene>     m_scene;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TREE_HPP
