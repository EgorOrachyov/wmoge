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

#include "prefab.hpp"

#include "core/class.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"

namespace wmoge {

    Status Prefab::read_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_RESOURCE("Prefab::read_from_yaml");

        SceneTree& tree = m_scene_tree.emplace();
        WG_YAML_READ(node, tree);

        return StatusCode::Ok;
    }

    Status Prefab::copy_to(Object& copy) const {
        Resource::copy_to(copy);
        auto* prefab         = dynamic_cast<Prefab*>(&copy);
        prefab->m_scene_tree = m_scene_tree;
        return StatusCode::Ok;
    }

    Status Prefab::instantiate(SceneNode& parent) {
        WG_AUTO_PROFILE_RESOURCE("Prefab::instantiate");

        assert(m_scene_tree.has_value());

        if (!m_scene_tree.has_value()) {
            WG_LOG_ERROR("no tree to instantiate prefab " << get_name());
            return StatusCode::InvalidState;
        }

        m_scene_tree.value().add_as_subtree(parent);

        return StatusCode::Ok;
    }

    void Prefab::register_class() {
        auto* cls = Class::register_class<Prefab>();
    }

}// namespace wmoge