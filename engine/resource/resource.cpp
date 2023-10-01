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
#include "core/engine.hpp"

namespace wmoge {

    ResourceId::ResourceId(const std::string& id) {
        m_name = SID(id);
    }
    ResourceId::ResourceId(const StringId& id) {
        m_name = id;
    }
    Status yaml_read(const YamlConstNodeRef& node, ResourceId& id) {
        WG_YAML_READ(node, id.m_name);
        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ResourceId& id) {
        WG_YAML_WRITE(node, id.m_name);
        return StatusCode::Ok;
    }

    Status Resource::copy_to(Object& other) const {
        auto* ptr   = dynamic_cast<Resource*>(&other);
        ptr->m_name = SID(m_name.str() + "_copy");
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
        cls->add_property(ClassProperty(VarType::StringId, SID("name"), SID("get_name")));
        cls->add_method(ClassMethod(VarType::StringId, SID("get_name"), {}), &Resource::get_name, {});
    }

}// namespace wmoge