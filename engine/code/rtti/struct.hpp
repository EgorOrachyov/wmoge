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

#pragma once

#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "io/context.hpp"
#include "rtti/meta_data.hpp"
#include "rtti/type.hpp"

#include <cinttypes>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class RttiMember
     * @brief Base class for any composable type (struct or class)
    */
    class RttiMember {
    public:
        RttiMember(Strid name) {
            m_name = name;
        }

        void set_metadata(RttiMetaData meta_data) { m_meta_data = std::move(meta_data); }

        [[nodiscard]] const Strid&        get_name() const { return m_name; }
        [[nodiscard]] const RttiMetaData& get_meta_data() const { return m_meta_data; }

    protected:
        Strid        m_name;
        RttiMetaData m_meta_data;
    };

    /**
     * @class RttiField
     * @brief Field info of a struct or a class
    */
    class RttiField : public RttiMember {
    public:
        RttiField(Strid name, RttiType* type, std::size_t byte_size, std::size_t byte_offset) : RttiMember(name) {
            m_type        = type;
            m_byte_size   = byte_size;
            m_byte_offset = byte_offset;
        }

        [[nodiscard]] RttiType*   get_type() const { return m_type; }
        [[nodiscard]] std::size_t get_byte_size() const { return m_byte_size; }
        [[nodiscard]] std::size_t get_byte_offset() const { return m_byte_offset; }

    private:
        RttiType*   m_type;
        std::size_t m_byte_size   = 0;
        std::size_t m_byte_offset = 0;
    };

    /**
     * @class RttiStruct
     * @brief Runtime struct info as a collecion of fields without logic
    */
    class RttiStruct : public RttiType {
    public:
        RttiStruct(Strid name, std::size_t byte_size, RttiStruct* parent, RttiArchetype archetype = RttiArchetype::Struct);
        ~RttiStruct() override = default;

        std::optional<const RttiField*> find_field(const Strid& name) const;
        void                            add_field(RttiField field);
        void                            set_extension(std::string extension);
        bool                            has_field(const Strid& name) const;
        bool                            has_member(const Strid& name) const;
        bool                            has_parent() const;
        bool                            is_subtype_of(const Strid& name) const;
        bool                            is_subtype_of(const RttiStruct* type) const;

        [[nodiscard]] const flat_set<Strid>&               get_inherits() const { return m_inherits; }
        [[nodiscard]] const flat_set<Strid>&               get_members() const { return m_members; }
        [[nodiscard]] const flat_map<Strid, std::int16_t>& get_fields_map() const { return m_fields_map; }
        [[nodiscard]] const std::vector<RttiField>&        get_fields() const { return m_fields; }
        [[nodiscard]] const std::string&                   get_extension() const { return m_extension; }
        [[nodiscard]] RttiStruct*                          get_parent() const { return m_parent; }

        Status copy(void* dst, const void* src) const override;
        Status clone(void* dst, const void* src) const override;
        Status read_from_tree(void* dst, IoTree& tree, IoContext& context) const override;
        Status write_to_tree(const void* src, IoTree& tree, IoContext& context) const override;
        Status read_from_stream(void* dst, IoStream& stream, IoContext& context) const override;
        Status write_to_stream(const void* src, IoStream& stream, IoContext& context) const override;
        Status to_string(const void* src, std::stringstream& s) const override;

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Struct; }

    protected:
        flat_set<Strid>               m_inherits;
        flat_set<Strid>               m_members;
        flat_map<Strid, std::int16_t> m_fields_map;
        std::vector<RttiField>        m_fields;
        std::string                   m_extension;
        RttiStruct*                   m_parent = nullptr;
    };

}// namespace wmoge