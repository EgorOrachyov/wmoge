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

#include "date_time.hpp"

#include <ctime>
#include <sstream>

namespace wmoge {

    DateTime::DateTime(TimePoint tp) : m_value(tp) {
    }

    DateTime::DateTime(const DateTimeTm& tm) {
        std::tm s_tm;
        s_tm.tm_year = tm.year;
        s_tm.tm_mon  = tm.month;
        s_tm.tm_mday = tm.day;
        s_tm.tm_hour = tm.hour;
        s_tm.tm_min  = tm.minute;
        s_tm.tm_sec  = tm.second;

        std::time_t s_time_t = std::mktime(&s_tm);
        m_value              = Clock::from_time_t(s_time_t);
    }

    DateTime::DateTime(const std::string& source) {
        std::stringstream stream(source);
        std::size_t       count;
        stream >> count;

        m_value = TimePoint(Clock::duration(static_cast<Clock::rep>(count)));
    }

    bool DateTime::operator<(const DateTime& other) const {
        return m_value < other.m_value;
    }

    bool DateTime::operator>(const DateTime& other) const {
        return m_value > other.m_value;
    }

    bool DateTime::operator==(const DateTime& other) const {
        return m_value == other.m_value;
    }

    bool DateTime::operator!=(const DateTime& other) const {
        return m_value != other.m_value;
    }

    DateTimeTm DateTime::to_tm() const {
        std::tm s_tm = to_tm_t();

        DateTimeTm tm{};
        tm.year   = s_tm.tm_year;
        tm.month  = s_tm.tm_mon;
        tm.day    = s_tm.tm_mday;
        tm.hour   = s_tm.tm_hour;
        tm.minute = s_tm.tm_min;
        tm.second = s_tm.tm_sec;
        return tm;
    }

    std::time_t DateTime::to_time_t() const {
        return Clock::to_time_t(m_value);
    }

    std::tm DateTime::to_tm_t() const {
        std::tm     s_tm;
        std::time_t s_time_t = to_time_t();

#if defined(TARGET_WINDOWS)
        localtime_s(&s_tm, &s_time_t);
#else
        localtime_r(&s_time_t, &s_tm);
#endif

        return s_tm;
    }

    std::string DateTime::to_string() const {
        std::size_t count = static_cast<std::size_t>(m_value.time_since_epoch().count());
        return std::to_string(count);
    }

    std::string DateTime::to_formatted(const std::string& format) const {
        char    buffer[128];
        std::tm tm = to_tm_t();
        strftime(buffer, sizeof(buffer), format.c_str(), &tm);
        return buffer;
    }

    std::string DateTime::to_pretty_string() const {
        static const std::string format = "%Y.%m.%d %H:%M:%S";
        return to_formatted(format);
    }

    DateTime DateTime::now() {
        DateTime t;
        t.m_value = Clock::now();
        return t;
    }

    Status tree_read(IoContext& context, IoTree& tree, DateTime& value) {
        std::string s;
        WG_TREE_READ(context, tree, s);
        value = DateTime(s);
        return WG_OK;
    }
    Status tree_write(IoContext& context, IoTree& tree, const DateTime& value) {
        const std::string s = value.to_string();
        WG_TREE_WRITE(context, tree, s);
        return WG_OK;
    }
    Status stream_read(IoContext& context, IoStream& stream, DateTime& value) {
        return stream.nread(sizeof(value.m_value), &value.m_value);
    }
    Status stream_write(IoContext& context, IoStream& stream, const DateTime& value) {
        return stream.nwrite(sizeof(value.m_value), &value.m_value);
    }

}// namespace wmoge