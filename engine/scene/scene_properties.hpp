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

#ifndef WMOGE_SCENE_PROPERTIES_HPP
#define WMOGE_SCENE_PROPERTIES_HPP

#include "scene/scene.hpp"
#include "scene/scene_camera.hpp"
#include "scene/scene_components.hpp"
#include "scene/scene_node.hpp"
#include "scene/scene_property.hpp"

namespace wmoge {

    /**
     * @class ScenePropCamera
     * @brief Node property
     */
    class ScenePropCamera : public SceneProperty {
    public:
        WG_OBJECT(ScenePropCamera, SceneProperty);

        void collect_arch(EcsArch& arch, class SceneNode& owner) override;
        void on_make_entity(EcsEntity entity, class SceneNode& owner) override;
        void on_delete_entity(EcsEntity entity, class SceneNode& owner) override;

        Status copy_to(Object& other) const override;
        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status write_to_yaml(YamlNodeRef node) const override;

        SceneDataCamera settings;
    };

    /**
     * @class ScenePropMeshStatic
     * @brief Node property
     */
    class ScenePropMeshStatic : public SceneProperty {
    public:
        WG_OBJECT(ScenePropMeshStatic, SceneProperty);

        void collect_arch(EcsArch& arch, class SceneNode& owner) override;
        void on_make_entity(EcsEntity entity, class SceneNode& owner) override;
        void on_delete_entity(EcsEntity entity, class SceneNode& owner) override;

        Status copy_to(Object& other) const override;
        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status write_to_yaml(YamlNodeRef node) const override;

        SceneDataMeshStatic settings;
    };

    /**
     * @class ScenePropAudioSource
     * @brief Node property
     */
    class ScenePropAudioSource : public SceneProperty {
    public:
        WG_OBJECT(ScenePropAudioSource, SceneProperty);
    };

    /**
     * @class ScenePropAudioListener
     * @brief Node property
     */
    class ScenePropAudioListener : public SceneProperty {
    public:
        WG_OBJECT(ScenePropAudioListener, SceneProperty);
    };

    /**
     * @class ScenePropLuaScript
     * @brief Node property
     */
    class ScenePropLuaScript : public SceneProperty {
    public:
        WG_OBJECT(ScenePropLuaScript, SceneProperty);
    };

}// namespace wmoge

#endif//WMOGE_SCENE_PROPERTIES_HPP