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

#ifndef WMOGE_SCENE_TREE_VISITOR_HPP
#define WMOGE_SCENE_TREE_VISITOR_HPP

#include "scene/scene_node.hpp"
#include "scene/scene_nodes.hpp"

namespace wmoge {

    /**
     * @class SceneTreeVisitor
     * @brief Interface to a visitor class to process scene tree recursively in depth-first order
     */
    class SceneTreeVisitor {
    public:
        virtual ~SceneTreeVisitor() = default;

        virtual bool visit(SceneNode& node)          = 0;
        virtual bool visit(SceneNodeFolder& node)    = 0;
        virtual bool visit(SceneNodeTransform& node) = 0;
        virtual bool visit(SceneNodePrefab& node)    = 0;
        virtual bool visit(SceneNodeEntity& node)    = 0;
        virtual bool visit(SceneNodeComponent& node) = 0;
        virtual bool visit(SceneNodeCamera& node)    = 0;
    };

    /**
     * @class SceneTreeVisitorSplit
     * @brief Interface to a visitor with a begin and end sequence
     */
    class SceneTreeVisitorSplit : public SceneTreeVisitor {
    public:
        ~SceneTreeVisitorSplit() override = default;

        bool visit_children(SceneNode& node);

        bool visit(SceneNode& node) final;
        bool visit(SceneNodeFolder& node) final;
        bool visit(SceneNodeTransform& node) final;
        bool visit(SceneNodePrefab& node) final;
        bool visit(SceneNodeEntity& node) final;
        bool visit(SceneNodeComponent& node) final;
        bool visit(SceneNodeCamera& node) final;

        virtual bool visit_begin(SceneNode& node)          = 0;
        virtual bool visit_begin(SceneNodeFolder& node)    = 0;
        virtual bool visit_begin(SceneNodeTransform& node) = 0;
        virtual bool visit_begin(SceneNodePrefab& node)    = 0;
        virtual bool visit_begin(SceneNodeEntity& node)    = 0;
        virtual bool visit_begin(SceneNodeComponent& node) = 0;
        virtual bool visit_begin(SceneNodeCamera& node)    = 0;

        virtual bool visit_end(SceneNode& node)          = 0;
        virtual bool visit_end(SceneNodeFolder& node)    = 0;
        virtual bool visit_end(SceneNodeTransform& node) = 0;
        virtual bool visit_end(SceneNodePrefab& node)    = 0;
        virtual bool visit_end(SceneNodeEntity& node)    = 0;
        virtual bool visit_end(SceneNodeComponent& node) = 0;
        virtual bool visit_end(SceneNodeCamera& node)    = 0;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TREE_VISITOR_HPP
