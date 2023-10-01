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

#ifndef WMOGE_PREFAB_HPP
#define WMOGE_PREFAB_HPP

#include "core/async.hpp"
#include "io/yaml.hpp"
#include "resource/resource.hpp"
#include "scene/scene_tree.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class Prefab
     * @brief A prefab resource which can be instantiated into a set of objects
     *
     * Prefab stores a sub-tree of scene nodes, which can be instantiated and added
     * to a scene tree at once. Prefab allows to make a complex object composed from
     * nodes and use it instantiate multiple times and keep all instances in sync.
     */
    class Prefab : public Resource {
    public:
        WG_OBJECT(Prefab, Resource)

        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status copy_to(Object& other) const override;

        [[nodiscard]] const SceneTreeData& get_data() const { return m_data; }

    private:
        SceneTreeData m_data;
    };

}// namespace wmoge

#endif//WMOGE_PREFAB_HPP
