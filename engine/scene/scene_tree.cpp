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

#include "scene_tree.hpp"

#include "core/class.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, SceneNodeData& data) {
        WG_YAML_READ_AS_OPT(node, "name", data.name);
        WG_YAML_READ_AS_OPT(node, "uuid", data.uuid);
        WG_YAML_READ_AS_OPT(node, "type", data.type);
        WG_YAML_READ_AS_OPT(node, "transform", data.transform);
        WG_YAML_READ_AS_OPT(node, "properties", data.properties);
        WG_YAML_READ_AS_OPT(node, "parent", data.parent);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const SceneNodeData& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "name", data.name);
        WG_YAML_WRITE_AS(node, "uuid", data.uuid);
        WG_YAML_WRITE_AS(node, "type", data.type);
        WG_YAML_WRITE_AS(node, "transform", data.transform);
        WG_YAML_WRITE_AS(node, "properties", data.properties);
        WG_YAML_WRITE_AS_OPT(node, "parent", data.parent.has_value(), data.parent);

        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, SceneTreeData& data) {
        WG_YAML_READ_AS(node, "nodes", data.nodes);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const SceneTreeData& data) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "nodes", data.nodes);

        return StatusCode::Ok;
    }

    SceneTree::SceneTree(const StringId& name) {
        m_root = make_ref<SceneNode>(SID("<root>"), SceneNodeType::Object);
    }

    void SceneTree::visit(const std::function<void(const Ref<SceneNode>& node)>& visitor) {
        m_root->visit(visitor);
    }
    std::optional<Ref<SceneNode>> SceneTree::find_node(const std::string& path) {
        return m_root->find_child_recursive(path);
    }
    std::vector<Ref<SceneNode>> SceneTree::get_nodes() {
        std::vector<Ref<SceneNode>> nodes;
        m_root->visit([&](const Ref<SceneNode>& node) {
            nodes.push_back(node);
        });
        return nodes;
    }
    std::vector<Ref<SceneNode>> SceneTree::filter_nodes(const std::function<bool(const Ref<SceneNode>& node)>& predicate) {
        std::vector<Ref<SceneNode>> nodes;
        m_root->visit([&](const Ref<SceneNode>& node) {
            if (predicate(node)) {
                nodes.push_back(node);
            }
        });
        return nodes;
    }

    Status SceneTree::build(const SceneTreeData& data) {
        const std::vector<SceneNodeData>& nodes_data = data.nodes;
        std::vector<Ref<SceneNode>>       nodes;

        nodes.reserve(nodes_data.size());

        for (const SceneNodeData& node_data : nodes_data) {
            Ref<SceneNode> node = make_ref<SceneNode>(node_data.name, node_data.type);
            node->set_uuid(node_data.uuid);
            node->set_transform(node_data.transform);
            node->set_properties(copy_objects(node_data.properties));
            nodes.push_back(node);

            Ref<SceneNode>& parent = node_data.parent ? nodes[node_data.parent.value()] : m_root;
            parent->add_child(node);
        }

        return StatusCode::Ok;
    }
    Status SceneTree::dump(SceneTreeData& data) {
        int                                 node_id = 0;
        std::unordered_map<SceneNode*, int> node_to_id;
        std::vector<SceneNodeData>&         nodes_data = data.nodes;

        visit([&](const Ref<SceneNode>& node) {
            node_to_id[node.get()] = node_id++;

            SceneNodeData& node_data = nodes_data.emplace_back();
            node_data.name           = node->get_name();
            node_data.type           = node->get_type();
            node_data.uuid           = node->get_uuid();
            node_data.transform      = node->get_transform();
            node_data.properties     = node->copy_properties();

            if (node->has_parent() && node->get_parent() != m_root.get()) {
                node_data.parent = node_to_id[node->get_parent()];
            }
        });

        return StatusCode::Ok;
    }

    void SceneTree::register_class() {
        auto* cls = Class::register_class<SceneTree>();
    }

}// namespace wmoge
