#include "time.hpp"
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

#include "time.hpp"

#include "math/math_utils.hpp"

#include <cassert>

namespace wmoge {

    void Time::tick() {
        auto new_point = clock::now();

        auto t  = float(double(std::chrono::duration_cast<ns>(new_point - m_start).count()) * 1e-9);
        auto dt = float(double(std::chrono::duration_cast<ns>(new_point - m_time_point).count()) * 1e-9);

        m_iteration += 1;
        m_time_point         = new_point;
        m_time               = t;
        m_current_delta      = dt;
        m_current_delta_game = Math::min(dt, 1.0f / 20.0f);
    }

    Time::time_point wmoge::Time::get_start() {
        return m_start;
    }

    std::time_t Time::get_time() {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }

    std::tm Time::get_tm(std::time_t time) {
        std::tm tm;
#if defined(TARGET_WINDOWS)
        localtime_s(&tm, &time);
#else
        localtime_r(&time, &tm);
#endif
        return tm;
    }

    std::string Time::get_time_formatted(const char* format, std::time_t time) {
        char    buffer[128];
        std::tm tm = get_tm(time);
        strftime(buffer, sizeof(buffer), format, &tm);
        return buffer;
    }

    std::size_t Time::get_iteration() {
        return m_iteration;
    }

    float Time::get_time() const {
        return m_time;
    }

    float Time::get_delta_time() const {
        return m_current_delta;
    }

    float Time::get_delta_time_game() const {
        return m_current_delta_game;
    }

}// namespace wmoge
