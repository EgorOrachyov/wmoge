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

#include "profiler_cpu.hpp"

namespace wmoge {

    struct ProfilerCpuStackEvent {
        ProfilerCpuMark*                       mark;
        std::string                            data;
        std::chrono::steady_clock ::time_point start;
    };

    thread_local std::vector<ProfilerCpuStackEvent> g_thread_event_stack;

    ProfilerCpu::ProfilerCpu() {
        add_thread("main", std::this_thread::get_id());
    }

    void ProfilerCpu::calibrate(std::chrono::steady_clock::time_point time) {
        m_cpu_time = time;
    }

    void ProfilerCpu::begin_event(ProfilerCpuMark* mark, const std::string& data) {
        ProfilerCpuStackEvent& event = g_thread_event_stack.emplace_back();
        event.mark                   = mark;
        event.data                   = data;
        event.start                  = std::chrono::steady_clock::now();
    }

    void ProfilerCpu::end_event() {
        thread_local int thread_id = get_thread_id(std::this_thread::get_id());

        ProfilerCpuStackEvent& event = g_thread_event_stack.back();
        ProfilerCpuEvent       out_event;
        out_event.mark        = event.mark;
        out_event.data        = std::move(event.data);
        out_event.thread_id   = thread_id;
        out_event.duration_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - event.start).count();
        out_event.start_us    = std::chrono::duration_cast<std::chrono::microseconds>(event.start - m_cpu_time).count();

        on_event.emit(out_event);

        g_thread_event_stack.pop_back();
    }

    void ProfilerCpu::add_thread(const std::string& name, std::thread::id id) {
        std::lock_guard guard(m_mutex);
        m_thread_map[id] = int(m_thread_id.size());
        m_thread_id.push_back(name);
    }

    void ProfilerCpu::get_thread_names(std::vector<std::string>& names) {
        std::lock_guard guard(m_mutex);
        names = m_thread_id;
    }

    int ProfilerCpu::get_thread_id(std::thread::id id) {
        std::lock_guard guard(m_mutex);
        return m_thread_map[id];
    }

    ProfilerCpu  g_profiler_cpu_default;
    ProfilerCpu* ProfilerCpu::g_profiler_cpu = &g_profiler_cpu_default;

    ProfilerCpu* ProfilerCpu::instance() {
        return g_profiler_cpu;
    }

}// namespace wmoge