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

#include "core/string_id.hpp"
#include "core/synchronization.hpp"
#include "core/timer.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace wmoge {

    /**
     * @class ProfilerMark
     * @brief Structure holding information about profiled function
     */
    struct ProfilerMark {
    public:
        ProfilerMark(Strid label, Strid in_function, Strid in_function_sig,
                     Strid in_file, Strid in_category,
                     std::size_t in_line);

        Strid           label;
        Strid           function;
        Strid           function_sig;
        Strid           file;
        Strid           category;
        std::size_t     line;
        std::string     pretty_name;
        class Profiler* profiler;
    };

    /**
     * @class ProfilerTimeEvent
     * @brief Auto profiling scope to measure duration of function
     */
    class ProfilerTimeEvent {
    public:
        ProfilerTimeEvent(ProfilerMark* mark, std::string desc);
        ~ProfilerTimeEvent();

    private:
        ProfilerMark* m_mark;
        std::string   m_desc;
        Timer         m_timer;
    };

    /**
     * @class ProfilerEntry
     * @brief Capture single entry holding information about function execution
     */
    struct ProfilerEntry {
        std::thread::id                       tid{};
        std::chrono::steady_clock::time_point start{};
        std::chrono::steady_clock::time_point stop{};
        std::string                           desc;
        ProfilerMark*                         mark = nullptr;
    };

    /**
     * @class ProfilerCapture
     * @brief Capture storing all events recorded during capture
     */
    class ProfilerCapture {
    public:
        ProfilerCapture() = default;

        void set_name(Strid name);
        void set_file(std::string file);
        void add_entry(ProfilerEntry&& entry);
        void save_to_json();

        [[nodiscard]] const Strid&                      get_name() const { return m_name; }
        [[nodiscard]] const std::vector<ProfilerEntry>& get_entries() const { return m_entries; }

    private:
        Strid                      m_name;
        std::string                m_file;
        std::vector<ProfilerEntry> m_entries;
    };

    /**
     * @class Profiler
     * @brief Engine profiler to capture trace of engine execution
     */
    class Profiler {
    public:
        Profiler();

        void set_enabled(bool value);
        void start_capture(std::shared_ptr<ProfilerCapture> capture);
        void end_capture();
        void add_entry(ProfilerEntry&& entry);
        void add_tid(std::thread::id id, Strid name);

        bool                                       is_enabled();
        bool                                       is_collecting();
        std::unordered_map<std::thread::id, Strid> get_tid_names() const;

        static Profiler* instance();
        static void      provide(Profiler* profiler);

    private:
        std::atomic_bool                           m_is_enabled{false};
        std::atomic_bool                           m_is_collecting{false};
        std::shared_ptr<ProfilerCapture>           m_capture;
        std::unordered_map<std::thread::id, Strid> m_tid_names;
        SpinMutex                                  m_mutex;

        static Profiler* g_profiler;
    };

}// namespace wmoge

#ifndef _WIN32
    #define SIGNATURE __PRETTY_FUNCTION__
#else
    #define SIGNATURE __FUNCSIG__
#endif

#define WG_PROFILE_MARK(name, system, label) \
    static ProfilerMark name(SID(label), SID(__FUNCTION__), SID(SIGNATURE), SID(__FILE__), SID(#system), std::size_t{__LINE__})

#define WG_PROFILE_DESC(system, label, desc)        \
    WG_PROFILE_MARK(__wg_auto_mark, system, label); \
    ProfilerTimeEvent __wg_auto_scope(&__wg_auto_mark, desc)

#define WG_AUTO_PROFILE_DESC(system, label, desc) WG_PROFILE_DESC(system, label, desc)
#define WG_AUTO_PROFILE(system, label)            WG_AUTO_PROFILE_DESC(system, label, "")

#define WG_AUTO_PROFILE_TASK(label, name)     WG_AUTO_PROFILE_DESC(core, label, name)
#define WG_AUTO_PROFILE_CORE(label)           WG_AUTO_PROFILE(core, label)
#define WG_AUTO_PROFILE_PLATFORM(label)       WG_AUTO_PROFILE(platform, label)
#define WG_AUTO_PROFILE_GLFW(label)           WG_AUTO_PROFILE(glfw, label)
#define WG_AUTO_PROFILE_GFX(label)            WG_AUTO_PROFILE(gfx, label)
#define WG_AUTO_PROFILE_GRC(label)            WG_AUTO_PROFILE(grc, label)
#define WG_AUTO_PROFILE_HGFX(label)           WG_AUTO_PROFILE(hgfx, label)
#define WG_AUTO_PROFILE_VULKAN(label)         WG_AUTO_PROFILE(vulkan, label)
#define WG_AUTO_PROFILE_IO(label)             WG_AUTO_PROFILE(io, label)
#define WG_AUTO_PROFILE_RESOURCE(label)       WG_AUTO_PROFILE(resource, label)
#define WG_AUTO_PROFILE_MESH(label)           WG_AUTO_PROFILE(mesh, label)
#define WG_AUTO_PROFILE_RENDER(label)         WG_AUTO_PROFILE(render, label)
#define WG_AUTO_PROFILE_GAMEPLAY(label)       WG_AUTO_PROFILE(gameplay, label)
#define WG_AUTO_PROFILE_DEBUG(label)          WG_AUTO_PROFILE(debug, label)
#define WG_AUTO_PROFILE_ECS(label)            WG_AUTO_PROFILE(ecs, label)
#define WG_AUTO_PROFILE_ECS_DECS(label, desc) WG_AUTO_PROFILE_DESC(ecs, label, desc)
#define WG_AUTO_PROFILE_SCENE(label)          WG_AUTO_PROFILE(scene, label)
#define WG_AUTO_PROFILE_SCRIPTING(label)      WG_AUTO_PROFILE(scripting, label)
#define WG_AUTO_PROFILE_LUA(label)            WG_AUTO_PROFILE(lua, label)
#define WG_AUTO_PROFILE_PFX(label)            WG_AUTO_PROFILE(pfx, label)
#define WG_AUTO_PROFILE_AUDIO(label)          WG_AUTO_PROFILE(audio, label)
#define WG_AUTO_PROFILE_OPENAL(label)         WG_AUTO_PROFILE(openal, label)
#define WG_AUTO_PROFILE_SYSTEM(label)         WG_AUTO_PROFILE(system, label)

#define WG_PROFILE_CAPTURE_START(capture, session, file_path) \
    capture = std::make_shared<ProfilerCapture>();            \
    capture->set_name(SID(#session));                         \
    capture->set_file(file_path);                             \
    Profiler::instance()->start_capture(capture);

#define WG_PROFILE_CAPTURE_END(capture)  \
    Profiler::instance()->end_capture(); \
    capture->save_to_json();             \
    capture.reset();
