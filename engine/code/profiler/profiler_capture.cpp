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

#include "profiler_capture.hpp"

#include "core/ioc_container.hpp"
#include "core/log.hpp"
#include "platform/file_system.hpp"
#include "platform/time.hpp"

namespace wmoge {

    ProfilerCapture::ProfilerCapture(IocContainer* ioc) {
        m_file_system = ioc->resolve_value<FileSystem>();
        m_time        = ioc->resolve_value<Time>();

        ProfilerCpu::instance()->on_event.bind([this](const ProfilerCpuEvent& e) {
            if (is_collecting()) {
                std::lock_guard guard(m_mutex);
                m_events_cpu.push_back(e);
            }
        });

        ProfilerGpu::instance()->on_event.bind([this](const ProfilerGpuEvent& e) {
            if (is_collecting()) {
                std::lock_guard guard(m_mutex);
                m_events_gpu.push_back(e);
            }
        });
    }

    void ProfilerCapture::enable(bool enable) {
        std::lock_guard guard(m_mutex);

        m_is_enabled.store(enable);
    }

    void ProfilerCapture::begin_capture(const Strid& session_name, const std::string& filepath) {
        std::lock_guard guard(m_mutex);

        if (!m_is_enabled.load()) { return; }

        m_session_name = session_name;
        m_session_path = filepath;
        m_events_cpu.clear();
        m_events_gpu.clear();

        m_is_collecting.store(true);
    }

    void ProfilerCapture::end_capture() {
        std::lock_guard guard(m_mutex);

        if (!m_is_enabled.load()) { return; }

        m_is_collecting.store(false);
    }

    void ProfilerCapture::save_capture() {
        std::lock_guard guard(m_mutex);

        std::fstream file_stream;
        if (!m_file_system->open_file_physical(m_session_path, file_stream, std::ios_base::out | std::ios::binary)) {
            WG_LOG_ERROR("failed to open capture file " << m_session_path);
            return;
        }

        std::vector<std::string> tid_names;
        ProfilerCpu::instance()->get_thread_names(tid_names);

        file_stream << R"({"otherData":{}, "traceEvents":[)";

        bool first = true;

        for (const ProfilerCpuEvent& entry : m_events_cpu) {
            auto entry_start = entry.start_us;
            auto entry_dur   = entry.duration_us;

            if (!first) file_stream << ",";

            file_stream << "{";
            file_stream << "\"pid\":0,";
            file_stream << R"("tid":")" << tid_names[entry.thread_id] << "\",";
            file_stream << R"("cat":")" << entry.mark->category.str() << "\",";
            file_stream << R"("name":")" << entry.mark->name << "\",";
            file_stream << R"("ph":"X",)";
            file_stream << "\"ts\":" << entry_start << ",";
            file_stream << "\"dur\":" << entry_dur << "";

            if (!entry.data.empty()) {
                file_stream << ",\"args\":";
                file_stream << "{";
                file_stream << R"("description":")" << entry.data << "\"";
                file_stream << "}";
            }

            file_stream << "}";
            first = false;
        }

        std::vector<std::string> queue_names;
        ProfilerGpu::instance()->get_queue_names(queue_names);

        for (const ProfilerGpuEvent& entry : m_events_gpu) {
            auto entry_start = entry.start_us;
            auto entry_dur   = entry.duration_us;

            if (!first) file_stream << ",";

            file_stream << "{";
            file_stream << "\"pid\":0,";
            file_stream << R"("tid":")" << queue_names[entry.queue_id] << "\",";
            file_stream << R"("cat":")" << entry.mark->category.str() << "\",";
            file_stream << R"("name":")" << entry.mark->name << "\",";
            file_stream << R"("ph":"X",)";
            file_stream << "\"ts\":" << entry_start << ",";
            file_stream << "\"dur\":" << entry_dur << "";

            if (!entry.data.empty()) {
                file_stream << ",\"args\":";
                file_stream << "{";
                file_stream << R"("description":")" << entry.data << "\"";
                file_stream << "}";
            }

            file_stream << "}";
            first = false;
        }

        file_stream << "]}";
        file_stream.close();

        WG_LOG_INFO("saved capture to " << m_session_path);
    }

    bool ProfilerCapture::is_collecting() const {
        return m_is_collecting.load();
    }

}// namespace wmoge