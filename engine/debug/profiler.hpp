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

#ifndef WMOGE_PROFILER_HPP
#define WMOGE_PROFILER_HPP

#include "core/string_id.hpp"
#include "core/timer.hpp"

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
        ProfilerMark(StringId in_function, StringId in_function_sig,
                     StringId in_file, StringId in_category,
                     std::size_t in_line);

        StringId        function;
        StringId        function_sig;
        StringId        file;
        StringId        category;
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
        void set_name(StringId name);
        void set_file(std::string file);
        void add_entry(ProfilerEntry&& entry);
        void save_to_json();

        const StringId                   get_name() const { return m_name; }
        const std::vector<ProfilerEntry> get_entries() const { return m_entries; }

    private:
        StringId                   m_name;
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
        void add_tid(std::thread::id id, StringId name);

        bool                                          is_enabled();
        bool                                          is_collecting();
        std::unordered_map<std::thread::id, StringId> get_tid_names() const;

    private:
        std::atomic_bool                              m_is_enabled{false};
        std::atomic_bool                              m_is_collecting{false};
        std::shared_ptr<ProfilerCapture>              m_capture;
        std::unordered_map<std::thread::id, StringId> m_tid_names;
        std::mutex                                    m_mutex;
    };

}// namespace wmoge

#ifndef _WIN32
    #define SIGNATURE __PRETTY_FUNCTION__
#else
    #define SIGNATURE __FUNCSIG__
#endif

#define WG_PROFILE_MARK(name, system) \
    static ProfilerMark name(SID(__FUNCTION__), SID(SIGNATURE), SID(__FILE__), SID(#system), std::size_t{__LINE__})

#define WG_PROFILE_DESC(system, desc)        \
    WG_PROFILE_MARK(__wg_auto_mark, system); \
    ProfilerTimeEvent __wg_auto_scope(&__wg_auto_mark, desc)

#define WG_AUTO_PROFILE_DESC(system, desc) WG_PROFILE_DESC(system, desc)
#define WG_AUTO_PROFILE(system)            WG_AUTO_PROFILE_DESC(system, "wmoge auto profile of " #system)

#define WG_AUTO_PROFILE_TASK(name)  WG_AUTO_PROFILE_DESC(core, name)
#define WG_AUTO_PROFILE_CORE()      WG_AUTO_PROFILE(core)
#define WG_AUTO_PROFILE_PLATFORM()  WG_AUTO_PROFILE(platform)
#define WG_AUTO_PROFILE_GLFW()      WG_AUTO_PROFILE(glfw)
#define WG_AUTO_PROFILE_GFX()       WG_AUTO_PROFILE(gfx)
#define WG_AUTO_PROFILE_VULKAN()    WG_AUTO_PROFILE(vulkan)
#define WG_AUTO_PROFILE_IO()        WG_AUTO_PROFILE(io)
#define WG_AUTO_PROFILE_RESOURCE()  WG_AUTO_PROFILE(resource)
#define WG_AUTO_PROFILE_RENDER()    WG_AUTO_PROFILE(render)
#define WG_AUTO_PROFILE_GAMEPLAY()  WG_AUTO_PROFILE(gameplay)
#define WG_AUTO_PROFILE_DEBUG()     WG_AUTO_PROFILE(debug)
#define WG_AUTO_PROFILE_SCENE()     WG_AUTO_PROFILE(scene)
#define WG_AUTO_PROFILE_SCRIPTING() WG_AUTO_PROFILE(scripting)
#define WG_AUTO_PROFILE_LUA()       WG_AUTO_PROFILE(lua)
#define WG_AUTO_PROFILE_PFX()       WG_AUTO_PROFILE(pfx)
#define WG_AUTO_PROFILE_AUDIO()     WG_AUTO_PROFILE(audio)
#define WG_AUTO_PROFILE_OPENAL()    WG_AUTO_PROFILE(openal)

#define WG_PROFILE_CAPTURE_START(session, file_path)                                            \
    std::shared_ptr<ProfilerCapture> __capture_##session = std::make_shared<ProfilerCapture>(); \
    __capture_##session->set_name(SID(#session));                                               \
    __capture_##session->set_file(file_path);                                                   \
    Engine::instance()->profiler()->start_capture(__capture_##session);

#define WG_PROFILE_CAPTURE_END(session)            \
    Engine::instance()->profiler()->end_capture(); \
    __capture_##session->save_to_json();           \
    __capture_##session.reset();

#endif//WMOGE_PROFILER_HPP
