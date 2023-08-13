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

#include "debug/profiler.hpp"
#include "scene/scene_tree_visitor.hpp"

namespace wmoge {

    SceneTree::SceneTree() {
        m_root = make_ref<SceneNode>();
    }

    void SceneTree::add_as_subtree(SceneNode& parent) {
        Ref<SceneNode> sub_tree = make_ref<SceneNode>();

        m_root->copy_to(*sub_tree);
        std::vector<Ref<SceneNode>> sub_tree_nodes = m_root->m_children;

        for (auto& child : sub_tree_nodes) {
            m_root->remove_child(child);
            parent.add_child(child);
        }
    }

    void SceneTree::copy_to(SceneTree& other) {
        WG_AUTO_PROFILE_SCENE("SceneTree::copy_to");

        m_root->copy_to(*other.m_root);
    }

    bool SceneTree::visit(SceneTreeVisitor& visitor) {
        WG_AUTO_PROFILE_SCENE("SceneTree::visit");

        assert(m_root);

        return m_root->accept_visitor(visitor);
    }

    Status yaml_read(const YamlConstNodeRef& node, SceneTree& tree) {
        return tree.m_root->read_from_yaml(node);
    }

    Status yaml_write(YamlNodeRef node, const SceneTree& tree) {
        return tree.m_root->write_to_yaml(node);
    }

}// namespace wmoge
