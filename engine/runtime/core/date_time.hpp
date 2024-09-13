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

#include "date_time.hpp"
#include "io/serialization.hpp"

#include <chrono>
#include <cstddef>
#include <ctime>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @brief DateTimeTm
     * @class Decomposed time struct
    */
    struct DateTimeTm {
        int year   = 1900;
        int month  = 1;
        int day    = 1;
        int hour   = 0;
        int minute = 0;
        int second = 0;
    };

    /**
     * @class DateTime
     * @brief Represents a date and time value.
    */
    class DateTime {
    public:
        using Clock     = std::chrono::system_clock;
        using TimePoint = Clock::time_point;

        DateTime() = default;
        DateTime(const DateTimeTm& tm);
        DateTime(const std::string& source);

        [[nodiscard]] DateTimeTm  to_tm() const;
        [[nodiscard]] std::time_t to_time_t() const;
        [[nodiscard]] std::tm     to_tm_t() const;
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::string to_formatted(const std::string& format) const;
        [[nodiscard]] std::string to_pretty_string() const;

        [[nodiscard]] TimePoint get_time_point() const { return m_value; }

        static DateTime now();

        friend Status yaml_read(IoContext& context, YamlConstNodeRef node, DateTime& value);
        friend Status yaml_write(IoContext& context, YamlNodeRef node, const DateTime& value);
        friend Status stream_read(IoContext& context, IoStream& stream, DateTime& value);
        friend Status stream_write(IoContext& context, IoStream& stream, const DateTime& value);

    private:
        TimePoint m_value{};
    };

    inline std::ostream& operator<<(std::ostream& stream, const DateTime& dt) {
        stream << dt.to_pretty_string();
        return stream;
    }

}// namespace wmoge