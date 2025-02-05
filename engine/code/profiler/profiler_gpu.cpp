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

#include "profiler_gpu.hpp"

#include "gfx/gfx_driver.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cassert>

namespace wmoge {

    ProfilerGpu::ProfilerGpu() {
        m_queue_names.push_back("queue-graphichs");
        m_queue_names.push_back("queue-compute");
        m_queue_names.push_back("queue-copy");
    }

    void ProfilerGpu::setup(GfxDriver* driver) {
        m_driver = driver;
    }

    void ProfilerGpu::enable(bool enable) {
        m_is_enabled = enable;
    }

    void ProfilerGpu::calibrate(std::chrono::steady_clock::time_point time) {
        WG_PROFILE_CPU_GPU("ProfilerGpu::calibrate");
        std::chrono::steady_clock::time_point cpu_now = std::chrono::steady_clock::now();
        m_driver->query_callibration(m_gpu_time, m_gpu_freq_ns);
        m_cpu_time = std::chrono::duration_cast<std::chrono::microseconds>(cpu_now - time).count();
    }

    void ProfilerGpu::prepare_cmd_list(const GfxCmdListRef& cmd_list) {
        WG_PROFILE_CPU_GPU("ProfilerGpu::prepare_cmd_list");
        if (!m_is_enabled) { return; }

        assert(cmd_list);

        if (m_query_pools.empty()) {
            GfxQueryPoolDesc desc;
            desc.size = m_events_limit;
            desc.type = GfxQueryPoolType::Timestamp;
            m_query_pools.emplace_back(m_driver->make_query_pool(desc, SID("gpu-profiler-pool-" + std::to_string(m_pool_id++))));
        }

        GfxQueryPoolRef pool = m_query_pools.back();
        m_query_pools.pop_back();

        CmdListData& data = m_cmd_lists[cmd_list.get()];
        data.queue_type   = cmd_list->get_queue_type();
        data.query_pool   = pool;

        cmd_list->reset_pool(data.query_pool, m_events_limit);
    }

    void ProfilerGpu::finish_cmd_list(const GfxCmdListRef& cmd_list) {
        WG_PROFILE_CPU_GPU("ProfilerGpu::finish_cmd_list");
        if (!m_is_enabled) { return; }

        assert(cmd_list);

        std::size_t idx = m_driver->frame_number() % GfxLimits::FRAMES_IN_FLIGHT;
        m_cmd_to_resolve[idx].emplace_back(std::move(m_cmd_lists[cmd_list.get()]));
        m_cmd_lists.erase(cmd_list.get());
    }

    void ProfilerGpu::begin_event(ProfilerGpuMark* mark, const std::string& data, GfxCmdList* cmd_list) {
        if (!m_is_enabled) { return; }

        CmdListData& list_data = m_cmd_lists[cmd_list];
        list_data.stack.push_back(int(list_data.events.size()));

        CmdListEvent& event   = list_data.events.emplace_back();
        event.mark            = mark;
        event.data            = data;
        event.query_idx_start = list_data.next_query_idx++;
        event.query_idx_end   = list_data.next_query_idx++;

        cmd_list->write_timestamp(list_data.query_pool, event.query_idx_start, GfxQueryFlag::PipelineTop);

        assert(list_data.next_query_idx < m_events_limit);
    }

    void ProfilerGpu::end_event(GfxCmdList* cmd_list) {
        if (!m_is_enabled) { return; }

        CmdListData&  list_data = m_cmd_lists[cmd_list];
        CmdListEvent& event     = list_data.events[list_data.stack.back()];

        cmd_list->write_timestamp(list_data.query_pool, event.query_idx_end, GfxQueryFlag::PipelineBottom);

        list_data.stack.pop_back();
    }

    void ProfilerGpu::resolve() {
        WG_PROFILE_CPU_GPU("ProfilerGpu::resolve");

        auto convert_timestamp = [&](std::uint64_t t) {
            assert(t >= m_gpu_time);
            return m_cpu_time + ((t - m_gpu_time) * m_gpu_freq_ns) / 1000;
        };

        const std::size_t idx = (m_driver->frame_number() + 1) % GfxLimits::FRAMES_IN_FLIGHT;

        for (CmdListData& data : m_cmd_to_resolve[idx]) {
            std::vector<std::uint64_t> timestamps(data.next_query_idx);
            m_driver->query_results(data.query_pool, timestamps);

            for (CmdListEvent& event : data.events) {
                ProfilerGpuEvent out_event;
                out_event.mark        = event.mark;
                out_event.data        = std::move(event.data);
                out_event.queue_id    = static_cast<int>(data.queue_type);
                out_event.start_us    = convert_timestamp(timestamps[event.query_idx_start]);
                out_event.duration_us = convert_timestamp(timestamps[event.query_idx_end]) - out_event.start_us;

                on_event.emit(out_event);
            }

            m_query_pools.emplace_back(std::move(data.query_pool));
        }

        m_cmd_to_resolve[idx].clear();
    }

    void ProfilerGpu::get_queue_names(std::vector<std::string>& names) {
        names = m_queue_names;
    }

    void ProfilerGpu::clear() {
        m_query_pools.clear();

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            m_cmd_to_resolve[i].clear();
        }
    }

    ProfilerGpu  g_profiler_gpu_default;
    ProfilerGpu* ProfilerGpu::g_profiler_gpu = &g_profiler_gpu_default;

    ProfilerGpu* ProfilerGpu::instance() {
        return g_profiler_gpu;
    }

}// namespace wmoge
