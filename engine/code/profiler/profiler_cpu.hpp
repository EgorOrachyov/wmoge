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

#include <atomic>
#include <chrono>
#include <cinttypes>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace wmoge {

    /** @brief Cpu profiling mark */
    struct ProfilerCpuMark {
        std::string name;
        Strid       category;
        Strid       function;
        Strid       file;
        std::size_t line;
    };

    /** @brief Cpu profiling time event */
    struct ProfilerCpuEvent {
        ProfilerCpuMark* mark = nullptr;
        std::string      data;
        std::uint64_t    start_us    = 0;
        std::uint64_t    duration_us = 0;
        int              thread_id   = -1;
    };

    /**
     * @class ProfilerCpu
     * @brief Collects cpu time events for application from different threads
     */
    class ProfilerCpu {
    public:
        Signal<const ProfilerCpuEvent&> on_event;

        ProfilerCpu();

        void calibrate(std::chrono::steady_clock::time_point time);
        void begin_event(ProfilerCpuMark* mark, const std::string& data);
        void end_event();
        void add_thread(const std::string& name, std::thread::id id);
        void get_thread_names(std::vector<std::string>& names);
        int  get_thread_id(std::thread::id id);

        static ProfilerCpu* instance();

    private:
        std::chrono::steady_clock::time_point    m_cpu_time;
        std::unordered_map<std::thread::id, int> m_thread_map;
        std::vector<std::string>                 m_thread_id;
        std::mutex                               m_mutex;

        static ProfilerCpu* g_profiler_cpu;
    };

    /** @brief Cpu profiling scope for single event */
    struct ProfilerCpuScope {
        ProfilerCpuScope(ProfilerCpuMark& mark, const std::string& data) { ProfilerCpu::instance()->begin_event(&mark, data); }
        ~ProfilerCpuScope() { ProfilerCpu::instance()->end_event(); }
    };

}// namespace wmoge

#define WG_PROFILE_CPU_MARK(var, system, name)                                                      \
    static ProfilerCpuMark var {                                                                    \
        std::string(name), SID(#system), SID(__FUNCTION__), SID(__FILE__), std::size_t { __LINE__ } \
    }

#define WG_PROFILE_CPU_SCOPE_WITH_DESC(system, name, desc) \
    WG_PROFILE_CPU_MARK(__wg_auto_mark, system, name);     \
    ProfilerCpuScope __wg_auto_scope(__wg_auto_mark, desc)

#define WG_PROFILE_CPU_SCOPE(system, name) \
    WG_PROFILE_CPU_SCOPE_WITH_DESC(system, name, "")

#define WG_PROFILE_CPU_TASK(label, desc)     WG_PROFILE_CPU_SCOPE_WITH_DESC(core, label, desc)
#define WG_PROFILE_CPU_CORE(label)           WG_PROFILE_CPU_SCOPE(core, label)
#define WG_PROFILE_CPU_PLATFORM(label)       WG_PROFILE_CPU_SCOPE(platform, label)
#define WG_PROFILE_CPU_GLFW(label)           WG_PROFILE_CPU_SCOPE(glfw, label)
#define WG_PROFILE_CPU_GFX(label)            WG_PROFILE_CPU_SCOPE(gfx, label)
#define WG_PROFILE_CPU_GRC(label)            WG_PROFILE_CPU_SCOPE(grc, label)
#define WG_PROFILE_CPU_RDG(label)            WG_PROFILE_CPU_SCOPE(rdg, label)
#define WG_PROFILE_CPU_VULKAN(label)         WG_PROFILE_CPU_SCOPE(vulkan, label)
#define WG_PROFILE_CPU_IO(label)             WG_PROFILE_CPU_SCOPE(io, label)
#define WG_PROFILE_CPU_RTTI(label)           WG_PROFILE_CPU_SCOPE(rtti, label)
#define WG_PROFILE_CPU_ASSET(label)          WG_PROFILE_CPU_SCOPE(asset, label)
#define WG_PROFILE_CPU_MESH(label)           WG_PROFILE_CPU_SCOPE(mesh, label)
#define WG_PROFILE_CPU_RENDER(label)         WG_PROFILE_CPU_SCOPE(render, label)
#define WG_PROFILE_CPU_GAME(label)           WG_PROFILE_CPU_SCOPE(game, label)
#define WG_PROFILE_CPU_DEBUG(label)          WG_PROFILE_CPU_SCOPE(debug, label)
#define WG_PROFILE_CPU_ECS(label)            WG_PROFILE_CPU_SCOPE(ecs, label)
#define WG_PROFILE_CPU_ECS_DECS(label, desc) WG_PROFILE_CPU_SCOPE_WITH_DESC(ecs, label, desc)
#define WG_PROFILE_CPU_SCENE(label)          WG_PROFILE_CPU_SCOPE(scene, label)
#define WG_PROFILE_CPU_SCRIPTING(label)      WG_PROFILE_CPU_SCOPE(scripting, label)
#define WG_PROFILE_CPU_UI(label)             WG_PROFILE_CPU_SCOPE(ui, label)
#define WG_PROFILE_CPU_FLOWGRAPH(label)      WG_PROFILE_CPU_SCOPE(flowgraph, label)
#define WG_PROFILE_CPU_LUA(label)            WG_PROFILE_CPU_SCOPE(lua, label)
#define WG_PROFILE_CPU_PFX(label)            WG_PROFILE_CPU_SCOPE(pfx, label)
#define WG_PROFILE_CPU_AUDIO(label)          WG_PROFILE_CPU_SCOPE(audio, label)
#define WG_PROFILE_CPU_OPENAL(label)         WG_PROFILE_CPU_SCOPE(openal, label)
#define WG_PROFILE_CPU_SYSTEM(label)         WG_PROFILE_CPU_SCOPE(system, label)
#define WG_PROFILE_CPU_PLUGIN(label)         WG_PROFILE_CPU_SCOPE(plugin, label)
#define WG_PROFILE_CPU_GPU(label)            WG_PROFILE_CPU_SCOPE(gpu, label)
