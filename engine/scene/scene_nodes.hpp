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

#ifndef WMOGE_SCENE_NODES_HPP
#define WMOGE_SCENE_NODES_HPP

#include "core/class.hpp"
#include "scene/scene_node.hpp"

namespace wmoge {

    /**
     * @class SceneNodeFolder
     * @brief Scene node which exists only in editable scene to group objects together
     */
    class SceneNodeFolder : public SceneNode {
    public:
        WG_OBJECT(SceneNodeFolder, SceneNode);
    };

    /**
     * @class SceneNodeTransform
     * @brief Scene node representing runtime transform node in transform hierarchy
     */
    class SceneNodeTransform : public SceneNode {
    public:
        WG_OBJECT(SceneNodeTransform, SceneNode);
    };

    /**
     * @class SceneNodePrefab
     * @brief Scene node for an instanced sub-tree from a prefab resource
     */
    class SceneNodePrefab : public SceneNode {
    public:
        WG_OBJECT(SceneNodePrefab, SceneNode);
    };

    /**
     * @class SceneNodeEntity
     * @brief Scene node base for any entity object in a runtime scene
     */
    class SceneNodeEntity : public SceneNode {
    public:
        WG_OBJECT(SceneNodeEntity, SceneNode);
    };

    /**
     * @class SceneNodeCamera
     * @brief Scene node representing game camera for rendering
     */
    class SceneNodeCamera : public SceneNodeEntity {
    public:
        WG_OBJECT(SceneNodeCamera, SceneNodeEntity);
    };

}// namespace wmoge

#endif//WMOGE_SCENE_NODES_HPP
