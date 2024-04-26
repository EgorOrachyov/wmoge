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
#include "profiler/profiler.hpp"
#include "scene/scene_manager.hpp"
#include "scene/scene_prefab.hpp"
#include "system/engine.hpp"

namespace wmoge {

    WG_IO_BEGIN(SceneTreeData)
    WG_IO_PROFILE()
    WG_IO_FIELD_OPT(nodes)
    WG_IO_FIELD_OPT(pipeline)
    WG_IO_END(SceneTreeData)

    SceneTree::SceneTree(const Strid& name) {
        m_name  = name;
        m_scene = Engine::instance()->scene_manager()->make_scene(SID(name.str() + ".runtime"));
        m_root  = make_ref<SceneNode>(SID("<root>"), SceneNodeType::Object);
        m_root->enter_tree(this);
    }

    void SceneTree::each(const std::function<void(const Ref<SceneNode>& node)>& visitor) {
        m_root->each(visitor);
    }
    bool SceneTree::contains(const Ref<SceneNode>& node) const {
        return m_root->contains(node);
    }
    std::optional<Ref<SceneNode>> SceneTree::find_node(const std::string& path) {
        return m_root->find_child_recursive(path);
    }
    std::vector<Ref<SceneNode>> SceneTree::get_nodes() {
        return std::move(m_root->get_nodes());
    }
    std::vector<Ref<SceneNode>> SceneTree::filter_nodes(const std::function<bool(const Ref<SceneNode>& node)>& predicate) {
        std::vector<Ref<SceneNode>> nodes;
        m_root->each([&](const Ref<SceneNode>& node) {
            if (predicate(node)) {
                nodes.push_back(node);
            }
        });
        return nodes;
    }

    Status SceneTree::build(const SceneTreeData& data) {
        WG_AUTO_PROFILE_SCENE("SceneTree::build");

        if (!m_root->build(data.nodes)) {
            return StatusCode::Error;
        }

        return StatusCode::Ok;
    }
    Status SceneTree::dump(SceneTreeData& data) {
        WG_AUTO_PROFILE_SCENE("SceneTree::dump");

        if (!m_root->dump(data.nodes)) {
            return StatusCode::Error;
        }

        return StatusCode::Ok;
    }

    void SceneTree::register_class() {
        auto* cls = Class::register_class<SceneTree>();
    }

}// namespace wmoge
