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

#include "core/synchronization.hpp"
#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

namespace wmoge {

    /**
     * @class ProfilerCapture
     * @brief Allows to caprute cpu and gpu time profiling events and dump to a file
     */
    class ProfilerCapture {
    public:
        ProfilerCapture(class IocContainer* ioc);

        void enable(bool enable);
        void begin_capture(const Strid& session_name, const std::string& filepath);
        void end_capture();
        void save_capture();
        bool is_collecting() const;

    private:
        std::atomic_bool              m_is_enabled{false};
        std::atomic_bool              m_is_collecting{false};
        std::vector<ProfilerCpuEvent> m_events_cpu;
        std::vector<ProfilerGpuEvent> m_events_gpu;
        Strid                         m_session_name;
        std::string                   m_session_path;

        class FileSystem* m_file_system;
        class Time*       m_time;

        SpinMutex m_mutex;
    };

}// namespace wmoge