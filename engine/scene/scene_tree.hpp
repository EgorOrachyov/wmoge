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
#include "io/yaml.hpp"
#include "scene/scene_node.hpp"
#include "scene/scene_tree_visitor.hpp"

#include <string>

namespace wmoge {

    /**
     * @class SceneTree
     * @brief Editable tree representing static version of all scene game objects
     */
    class SceneTree final {
    public:
        SceneTree();

        // add node
        // find node

        /**
         * @brief Visit tree with specified visitor
         *
         * @param visitor Visitor to pass to tree nodes
         *
         * @return True on success
         */
        bool visit(SceneTreeVisitor& visitor);

        [[nodiscard]] const Ref<SceneNode>& get_root() const { return m_root; }

        friend bool yaml_read(const YamlConstNodeRef& node, SceneTree& tree);
        friend bool yaml_write(YamlNodeRef node, const SceneTree& tree);

    private:
        Ref<SceneNode> m_root;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TREE_HPP
