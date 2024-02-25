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

#include <atomic>
#include <chrono>
#include <ctime>
#include <string>

namespace wmoge {

    /**
     * @class Time
     * @brief Manages platform time and engine update time state
    */
    class Time {
    public:
        using clock      = std::chrono::steady_clock;
        using time_point = clock::time_point;
        using ns         = std::chrono::nanoseconds;

        void tick();

        time_point  get_start();
        std::time_t get_time();
        std::tm     get_tm(std::time_t time);
        std::string get_time_formatted(const char* format, std::time_t time);
        std::size_t get_iteration();
        float       get_time() const;
        float       get_delta_time() const;
        float       get_delta_time_game() const;

    private:
        time_point  m_start              = clock::now();
        time_point  m_time_point         = clock::now();
        std::size_t m_iteration          = 0;
        float       m_time               = 0;
        float       m_current_delta      = 0;
        float       m_current_delta_game = 0;
    };

}// namespace wmoge