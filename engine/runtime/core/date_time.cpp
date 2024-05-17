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
        DateTimeTm        tm;
        std::stringstream str(source);

        str >> tm.year >> tm.month >> tm.day >> tm.hour >> tm.minute >> tm.second;

        *this = DateTime(tm);
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
        std::stringstream str;
        DateTimeTm        tm = to_tm();

        str << tm.year << ' '
            << tm.month << ' '
            << tm.day << ' '
            << tm.hour << ' '
            << tm.minute << ' '
            << tm.second;

        return str.str();
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

    Status yaml_read(IoContext& context, YamlConstNodeRef node, DateTime& value) {
        std::string s;
        WG_YAML_READ(context, node, s);
        value = DateTime(s);
        return WG_OK;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const DateTime& value) {
        const std::string s = value.to_string();
        WG_YAML_WRITE(context, node, s);
        return WG_OK;
    }
    Status archive_read(IoContext& context, Archive& archive, DateTime& value) {
        return archive.nread(sizeof(value.m_value), &value.m_value);
    }
    Status archive_write(IoContext& context, Archive& archive, const DateTime& value) {
        return archive.nwrite(sizeof(value.m_value), &value.m_value);
    }

}// namespace wmoge