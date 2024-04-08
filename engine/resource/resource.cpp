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

#include "resource.hpp"

#include "core/class.hpp"
#include "system/engine.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, ResourceId& id) {
        WG_YAML_READ(node, id.m_name);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ResourceId& id) {
        WG_YAML_WRITE(node, id.m_name);
        return StatusCode::Ok;
    }
    Status archive_read(Archive& archive, ResourceId& id) {
        WG_ARCHIVE_READ(archive, id.m_name);
        return StatusCode::Ok;
    }
    Status archive_write(Archive& archive, const ResourceId& id) {
        WG_ARCHIVE_WRITE(archive, id.m_name);
        return StatusCode::Ok;
    }

    ResourceId::ResourceId(const std::string& id) {
        m_name = SID(id);
    }
    ResourceId::ResourceId(const Strid& id) {
        m_name = id;
    }

    Status Resource::copy_to(Object& other) const {
        auto* ptr = dynamic_cast<Resource*>(&other);
        ptr->m_id = SID(m_id.str() + "_copy");
        return StatusCode::Ok;
    }
    Status Resource::read_from_yaml(const YamlConstNodeRef& node) {
        return StatusCode::Ok;
    }
    Status Resource::write_to_yaml(YamlNodeRef node) const {
        return StatusCode::Ok;
    }

    void Resource::register_class() {
        auto cls = Class::register_class<Resource>();
        cls->add_property(ClassProperty(VarType::Strid, SID("name"), SID("get_name")));
        cls->add_method(ClassMethod(VarType::Strid, SID("get_name"), {}), &Resource::get_name, {});
    }

    void ResourceDependencies::set_mode(CollectionMode mode, std::optional<int> num_levels) {
        assert(m_cur_depth == 0);

        if (mode == CollectionMode::OneLevel) {
            m_max_depth = 1;
        }
        if (mode == CollectionMode::MultipleLevels) {
            m_max_depth = num_levels.value_or(1);
        }
        if (mode == CollectionMode::FullDepth) {
            m_max_depth = std::numeric_limits<int>::max();
        }

        m_mode = mode;
    }

    void ResourceDependencies::add(const Ref<Resource>& resource) {
        if (m_cur_depth >= m_max_depth) {
            return;
        }
        if (!resource) {
            return;
        }

        m_cur_depth += 1;

        m_resources.emplace(resource);
        resource->collect_deps(*this);

        m_cur_depth -= 1;
    }

    buffered_vector<Ref<Resource>> ResourceDependencies::to_vector() const {
        buffered_vector<Ref<Resource>> vec(m_resources.size());
        std::copy(m_resources.begin(), m_resources.end(), vec.begin());
        return vec;
    }

}// namespace wmoge