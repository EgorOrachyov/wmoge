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
#include "scene/scene_camera.hpp"
#include "scene/scene_node.hpp"
#include "scene/scene_transform.hpp"

namespace wmoge {

    /**
     * @class SceneNodeFolder
     * @brief Scene node which exists only in editable scene to group objects together
     */
    class SceneNodeFolder : public SceneNode {
    public:
        WG_OBJECT(SceneNodeFolder, SceneNode);

        bool visit(class SceneTreeVisitor& visitor) override;
    };

    /**
     * @class SceneNodeTransform
     * @brief Scene node representing runtime transform node in transform hierarchy
     */
    class SceneNodeTransform : public SceneNode {
    public:
        WG_OBJECT(SceneNodeTransform, SceneNode);

        bool visit(class SceneTreeVisitor& visitor) override;
    };

    /**
     * @class SceneNodePrefab
     * @brief Scene node for an instanced sub-tree from a prefab resource
     */
    class SceneNodePrefab : public SceneNode {
    public:
        WG_OBJECT(SceneNodePrefab, SceneNode);

        bool visit(class SceneTreeVisitor& visitor) override;
    };

    /**
     * @class SceneNodeEntity
     * @brief Scene node base for any entity object in a runtime scene
     */
    class SceneNodeEntity : public SceneNode {
    public:
        WG_OBJECT(SceneNodeEntity, SceneNode);

        bool visit(class SceneTreeVisitor& visitor) override;
    };

    /**
     * @class SceneNodeComponent
     * @brief Scene node base for any component attached to a parent entity object in a runtime scene
     */
    class SceneNodeComponent : public SceneNode {
    public:
        WG_OBJECT(SceneNodeComponent, SceneNode);

        bool visit(class SceneTreeVisitor& visitor) override;
    };

    /**
     * @class SceneNodeCamera
     * @brief Scene node representing game camera for rendering
     */
    class SceneNodeCamera : public SceneNodeComponent {
    public:
        WG_OBJECT(SceneNodeCamera, SceneNodeComponent);

        bool visit(class SceneTreeVisitor& visitor) override;

        Color4f          color      = Color::BLACK4f;
        Vec4f            viewport   = Vec4f(0, 0, 1, 1);
        float            fov        = Math::deg_to_rad(45.0f);
        float            near       = 0.1f;
        float            far        = 10000.0f;
        StringId         target     = SID("primary");
        CameraProjection projection = CameraProjection::Perspective;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_NODES_HPP
