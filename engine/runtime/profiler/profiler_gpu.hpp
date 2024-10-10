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

#include "core/signal.hpp"
#include "core/string_id.hpp"
#include "core/timer.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_query.hpp"

#include <atomic>
#include <chrono>
#include <cinttypes>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace wmoge {

    struct ProfilerGpuMark {
        std::string name;
        Strid       category;
        Strid       function;
        Strid       file;
        std::size_t line;
    };

    struct ProfilerGpuEvent {
        ProfilerGpuMark* mark = nullptr;
        std::string      data;
        std::uint64_t    start_us    = 0;
        std::uint64_t    duration_us = 0;
        int              queue_id    = -1;
    };

    class ProfilerGpu {
    public:
        Signal<const ProfilerGpuEvent&> on_event;

        ProfilerGpu();

        void setup(class GfxDriver* driver);
        void enable(bool enable);
        void calibrate(std::chrono::steady_clock::time_point time);
        void prepare_cmd_list(GfxCmdList* cmd_list);
        void finish_cmd_list(GfxCmdList* cmd_list);
        void begin_event(ProfilerGpuMark* mark, const std::string& data, GfxCmdList* cmd_list);
        void end_event(GfxCmdList* cmd_list);
        void resolve();
        void get_queue_names(std::vector<std::string>& names);

        static ProfilerGpu* instance();

    private:
        struct CmdListEvent {
            ProfilerGpuMark* mark;
            std::string      data;
            int              query_idx_start;
            int              query_idx_end;
        };

        struct CmdListData {
            GfxQueueType              queue_type;
            GfxQueryPoolRef           query_pool;
            std::vector<CmdListEvent> events;
            std::vector<int>          stack;
            int                       next_query_idx = 0;
        };

        std::unordered_map<GfxCmdList*, CmdListData> m_cmd_lists;
        std::vector<CmdListData>                     m_cmd_to_resolve[GfxLimits::FRAMES_IN_FLIGHT];
        std::vector<GfxQueryPoolRef>                 m_query_pools;
        std::vector<std::string>                     m_queue_names;
        std::uint64_t                                m_cpu_time;
        std::uint64_t                                m_gpu_time;
        std::uint64_t                                m_gpu_freq_ns;
        bool                                         m_is_enabled   = false;
        int                                          m_events_limit = 2048;
        int                                          m_pool_id      = 0;

        class GfxDriver* m_driver = nullptr;

        static ProfilerGpu* g_profiler_gpu;
    };

    struct ProfilerGpuScope {
        ProfilerGpuScope(ProfilerGpuMark& mark, const std::string& data, GfxCmdList* cmd_list) : cmd_list(cmd_list) { ProfilerGpu::instance()->begin_event(&mark, data, cmd_list); }
        ~ProfilerGpuScope() { ProfilerGpu::instance()->end_event(cmd_list); }

        GfxCmdList* cmd_list;
    };

}// namespace wmoge

#define WG_PROFILE_GPU_BEGIN(cmd_list) \
    ProfilerGpu::instance()->prepare_cmd_list(cmd_list)

#define WG_PROFILE_GPU_END(cmd_list) \
    ProfilerGpu::instance()->finish_cmd_list(cmd_list)

#define WG_PROFILE_GPU_MARK(var, system, name)                                                      \
    static ProfilerGpuMark var {                                                                    \
        std::string(name), SID(#system), SID(__FUNCTION__), SID(__FILE__), std::size_t { __LINE__ } \
    }

#define WG_PROFILE_GPU_SCOPE_WITH_DESC(cmd_list, system, name, desc) \
    WG_PROFILE_GPU_MARK(__wg_auto_mark_gpu, system, name);           \
    ProfilerGpuScope __wg_auto_scope_gpu(__wg_auto_mark_gpu, desc, cmd_list)

#define WG_PROFILE_GPU_SCOPE(cmd_list, name) \
    WG_PROFILE_GPU_SCOPE_WITH_DESC(cmd_list, gfx, name, "")