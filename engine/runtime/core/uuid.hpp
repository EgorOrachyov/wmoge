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

#include "io/stream.hpp"
#include "io/tree.hpp"

#include <cinttypes>
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @class UUID
     * @brief Unique 8-byte long identifier for in-game objects and assets
     */
    class UUID {
    public:
        UUID() = default;
        UUID(std::uint64_t value);
        UUID(const std::string& value, int base = 10);

        bool operator==(const UUID& other) const { return m_value == other.m_value; }
        bool operator!=(const UUID& other) const { return m_value != other.m_value; }

        operator bool() const { return is_not_null(); }

        [[nodiscard]] bool        is_null() const { return m_value == 0; }
        [[nodiscard]] bool        is_not_null() const { return m_value != 0; }
        [[nodiscard]] std::string to_str() const;
        [[nodiscard]] std::size_t hash() const { return std::hash<std::uint64_t>()(m_value); }

        [[nodiscard]] std::uint64_t&       value() { return m_value; }
        [[nodiscard]] const std::uint64_t& value() const { return m_value; }

        static UUID generate();

        friend Status tree_read(IoContext& context, IoTree& tree, UUID& id);
        friend Status tree_write(IoContext& context, IoTree& tree, const UUID& id);

        friend Status stream_read(IoContext& context, IoStream& stream, UUID& id);
        friend Status stream_write(IoContext& context, IoStream& stream, const UUID& id);

    private:
        std::uint64_t m_value = 0;
    };

    static_assert(std::is_trivially_destructible_v<UUID>, "uuid must be trivial as ptr on int");

    inline std::ostream& operator<<(std::ostream& stream, const UUID& uuid) {
        stream << '\'' << uuid.to_str() << '\'';
        return stream;
    }

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::UUID> {
        std::size_t operator()(const wmoge::UUID& uuid) const {
            return uuid.hash();
        }
    };

}// namespace std