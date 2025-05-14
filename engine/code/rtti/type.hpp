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
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"
#include "rtti/meta_data.hpp"

#include <cinttypes>
#include <memory>
#include <optional>
#include <sstream>

namespace wmoge {

    /** @brief Rtti archetype of reflected type */
    enum class RttiArchetype {
        Fundamental = 0,
        Vec,
        Mask,
        Bitset,
        Ref,
        AssetRef,
        Optional,
        Vector,
        Set,
        Map,
        Pair,
        Function,
        Enum,
        Struct,
        Class
    };

    /**
     * @class RttiType
     * @brief Base class for any rtti system type for a reflection
    */
    class RttiType : public RefCnt {
    public:
        RttiType(Strid name, std::size_t byte_size, RttiArchetype archetype = RttiArchetype::Fundamental)
            : m_name(name), m_byte_size(byte_size), m_archetype(archetype) {
        }

        ~RttiType() override = default;

        void set_metadata(RttiMetaData meta_data) { m_meta_data = std::move(meta_data); }

        virtual Status consturct(void* dst) const { return StatusCode::NotImplemented; }
        virtual Status copy(void* dst, const void* src) const { return StatusCode::NotImplemented; }
        virtual Status clone(void* dst, const void* src) const { return StatusCode::NotImplemented; }
        virtual Status destruct(void* dst) const { return StatusCode::NotImplemented; }
        virtual Status read_from_tree(void* dst, IoTree& tree, IoContext& context) const { return StatusCode::NotImplemented; }
        virtual Status write_to_tree(const void* src, IoTree& tree, IoContext& context) const { return StatusCode::NotImplemented; }
        virtual Status read_from_stream(void* dst, IoStream& stream, IoContext& context) const { return StatusCode::NotImplemented; }
        virtual Status write_to_stream(const void* src, IoStream& stream, IoContext& context) const { return StatusCode::NotImplemented; }
        virtual Status to_string(const void* src, std::stringstream& s) const { return StatusCode::NotImplemented; }
        virtual bool   archetype_is(RttiArchetype ar) const { return ar == m_archetype; }

        [[nodiscard]] const Strid&        get_name() const { return m_name; }
        [[nodiscard]] const std::string&  get_str() const { return m_name.str(); }
        [[nodiscard]] std::size_t         get_byte_size() const { return m_byte_size; }
        [[nodiscard]] const RttiMetaData& get_meta_data() const { return m_meta_data; }
        [[nodiscard]] RttiArchetype       get_archetype() const { return m_archetype; }

        [[nodiscard]] static RttiArchetype static_archetype() { return RttiArchetype::Fundamental; }

    protected:
        Strid         m_name;
        std::size_t   m_byte_size = 0;
        RttiMetaData  m_meta_data;
        RttiArchetype m_archetype;
    };

}// namespace wmoge