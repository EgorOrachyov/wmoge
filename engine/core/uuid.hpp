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

#ifndef WMOGE_UUID_HPP
#define WMOGE_UUID_HPP

#include "io/archive.hpp"
#include "io/yaml.hpp"

#include <cinttypes>
#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @class UUID
     * @brief Unique 8-byte long identifier for in-game objects and resources
     */
    class UUID {
    public:
        UUID() = default;
        UUID(std::uint64_t value);

        [[nodiscard]] bool is_null() const { return m_value == 0; }
        [[nodiscard]] bool is_not_null() const { return m_value != 0; }

        operator bool() const { return is_not_null(); }

        [[nodiscard]] std::string to_str() const;

        [[nodiscard]] std::uint64_t&       value() { return m_value; }
        [[nodiscard]] const std::uint64_t& value() const { return m_value; }

        static UUID generate();

        friend bool yaml_read(const YamlConstNodeRef& node, UUID& id);
        friend bool yaml_write(YamlNodeRef node, const UUID& id);

        friend bool archive_read(Archive& archive, UUID& id);
        friend bool archive_write(Archive& archive, const UUID& id);

    private:
        std::uint64_t m_value = 0;
    };

    static_assert(std::is_trivially_destructible_v<UUID>, "uuid must be trivial as ptr on int");

    inline std::ostream& operator<<(std::ostream& stream, const UUID& uuid) {
        stream << '\'' << uuid.to_str() << '\'';
        return stream;
    }

}// namespace wmoge

#endif//WMOGE_UUID_HPP
