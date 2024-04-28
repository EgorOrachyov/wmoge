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

#include "struct.hpp"

#include "profiler/profiler.hpp"

#include <cassert>
#include <cinttypes>

namespace wmoge {

    RttiStruct::RttiStruct(Strid name, std::size_t byte_size, RttiStruct* parent) : RttiType(name, byte_size) {
        m_parent = parent;

        if (m_parent) {
            m_inherits   = parent->m_inherits;
            m_fields     = parent->m_fields;
            m_fields_map = parent->m_fields_map;
            m_members    = parent->m_members;
        }

        m_inherits.insert(name);
    }

    std::optional<const RttiField*> RttiStruct::find_field(const Strid& name) const {
        auto query = m_fields_map.find(name);

        if (query != m_fields_map.end()) {
            return &m_fields[query->second];
        }

        return std::nullopt;
    }

    void RttiStruct::add_field(RttiField field) {
        assert(!has_field(field.get_name()));

        const std::int16_t id = std::int16_t(m_fields.size());
        m_fields.push_back(std::move(field));
        m_fields_map[m_fields.back().get_name()] = id;
        m_members.insert(m_fields.back().get_name());
    }

    bool RttiStruct::has_field(const Strid& name) const {
        auto query = m_fields_map.find(name);
        return query != m_fields_map.end();
    }

    bool RttiStruct::has_member(const Strid& name) const {
        auto query = m_members.find(name);
        return query != m_members.end();
    }

    bool RttiStruct::has_parent() const {
        return m_parent;
    }

    bool RttiStruct::is_subtype_of(const Strid& name) const {
        auto query = m_inherits.find(name);
        return query != m_inherits.end();
    }

    bool RttiStruct::is_subtype_of(const RttiStruct* type) const {
        return is_subtype_of(type->get_name());
    }

    Status RttiStruct::copy(void* dst, const void* src) const {
        assert(dst);
        assert(src);
        std::uint8_t*       self  = reinterpret_cast<std::uint8_t*>(dst);
        const std::uint8_t* other = reinterpret_cast<const std::uint8_t*>(src);
        for (const RttiField& field : get_fields()) {
            if (field.get_meta_data().is_no_copy()) {
                continue;
            }
            std::size_t offset = field.get_byte_offset();
            WG_CHECKED(field.get_type()->copy(self + offset, other + offset));
        }
        return WG_OK;
    }

    Status RttiStruct::read_from_yaml(void* dst, YamlConstNodeRef node) const {
        WG_AUTO_PROFILE_RTTI("RttiStruct::read_from_yaml");
        assert(dst);
        std::uint8_t* self = reinterpret_cast<std::uint8_t*>(dst);
        for (const RttiField& field : get_fields()) {
            if (field.get_meta_data().is_no_save_load()) {
                continue;
            }

            const std::string&  field_name = field.get_name().str();
            const ryml::csubstr field_name_str(field_name.data(), field_name.length());

            if (field.get_meta_data().is_optional()) {
                if (node.has_child(field_name_str)) {
                    WG_CHECKED(field.get_type()->read_from_yaml(self + field.get_byte_offset(), node[field_name_str]));
                }
            } else {
                if (!node.has_child(field_name_str)) {
                    WG_LOG_ERROR("failed to read yaml " << field_name);
                    return StatusCode::FailedRead;
                }
                WG_CHECKED(field.get_type()->read_from_yaml(self + field.get_byte_offset(), node[field_name_str]));
            }
        }
        return WG_OK;
    }

    Status RttiStruct::write_to_yaml(const void* src, YamlNodeRef node) const {
        WG_AUTO_PROFILE_RTTI("RttiStruct::write_to_yaml");

        assert(src);
        WG_YAML_MAP(node);
        const std::uint8_t* self = reinterpret_cast<const std::uint8_t*>(src);
        for (const RttiField& field : get_fields()) {
            if (field.get_meta_data().is_no_save_load()) {
                continue;
            }

            const std::string&  field_name = field.get_name().str();
            const ryml::csubstr field_name_str(field_name.data(), field_name.length());

            YamlNodeRef child = node.append_child();
            child << ryml::key(field_name_str);

            WG_CHECKED(field.get_type()->write_to_yaml(self + field.get_byte_offset(), child));
        }
        return WG_OK;
    }

    Status RttiStruct::read_from_archive(void* dst, Archive& archive) const {
        WG_AUTO_PROFILE_RTTI("RttiStruct::read_from_archive");
        assert(dst);

        std::uint8_t* self = reinterpret_cast<std::uint8_t*>(dst);
        for (const RttiField& field : get_fields()) {
            if (field.get_meta_data().is_no_save_load()) {
                continue;
            }
            WG_CHECKED(field.get_type()->read_from_archive(self + field.get_byte_offset(), archive));
        }
        return WG_OK;
    }

    Status RttiStruct::write_to_archive(const void* src, Archive& archive) const {
        WG_AUTO_PROFILE_RTTI("RttiStruct::write_to_archive");

        assert(src);
        const std::uint8_t* self = reinterpret_cast<const std::uint8_t*>(src);
        for (const RttiField& field : get_fields()) {
            if (field.get_meta_data().is_no_save_load()) {
                continue;
            }
            WG_CHECKED(field.get_type()->write_to_archive(self + field.get_byte_offset(), archive));
        }
        return WG_OK;
    }

    Status RttiStruct::to_string(const void* src, std::stringstream& s) const {
        assert(src);
        const std::uint8_t* self = reinterpret_cast<const std::uint8_t*>(src);
        s << "{";
        for (const RttiField& field : get_fields()) {
            s << field.get_name().str() << "=";
            WG_CHECKED(field.get_type()->to_string(self + field.get_byte_offset(), s));
            s << "; ";
        }
        s << "}";
        return WG_OK;
    }

}// namespace wmoge