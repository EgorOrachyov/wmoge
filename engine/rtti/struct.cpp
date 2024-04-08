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

#include <cassert>

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

}// namespace wmoge