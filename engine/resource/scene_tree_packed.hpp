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

#ifndef WMOGE_SCENE_TREE_PACKED_HPP
#define WMOGE_SCENE_TREE_PACKED_HPP

#include "core/async.hpp"
#include "io/yaml.hpp"
#include "resource/resource.hpp"
#include "scene/scene_tree.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class SceneTreePacked
     * @brief Represents packed scene trre resource which can be used to load editable scene
     *
     * Packed scene tree stores serialized scene tree description. The description is a list
     * of scene nodes data and information about their hirerachy. This description can be
     * used to instantiate a scene tree, which can be used for scene editing or for
     * emitting of a runtime scene version. Scene tree used only for editing, 
     * runtime scene can be loaded used ScenePacked resource. 
     * 
     * @see SceneTree
     */
    class SceneTreePacked final : public Resource {
    public:
        WG_OBJECT(SceneTreePacked, Resource)

        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status copy_to(Object& other) const override;

        AsyncResult<Ref<SceneTree>> instantiate_async();
        Ref<SceneTree>              instantiate();

    private:
        SceneTreeData m_data;
    };

}// namespace wmoge

#endif//WMOGE_SCENE_TREE_PACKED_HPP
