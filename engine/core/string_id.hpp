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

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @class StringId
     * @brief Interned globally-stored string based ids for fast lookups
     */
    class StringId {
    public:
        StringId();
        explicit StringId(const std::string& string);

        bool operator==(const StringId& other) const;
        bool operator!=(const StringId& other) const;
        bool operator<(const StringId& other) const;

        [[nodiscard]] bool empty() const { return m_string->empty(); }

        [[nodiscard]] std::size_t        id() const;
        [[nodiscard]] std::size_t        hash() const;
        [[nodiscard]] const std::string& str() const;

    private:
        const std::string* m_string;
    };

    static_assert(std::is_trivially_destructible_v<StringId>, "string must be trivial as ptr or int");

    inline std::ostream& operator<<(std::ostream& stream, const StringId& id) {
        stream << '\'' << id.str() << '\'';
        return stream;
    }

#define SID(id) ::wmoge::StringId(id)

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::StringId> {
    public:
        std::size_t operator()(const wmoge::StringId& id) const {
            return id.hash();
        }
    };

}// namespace std