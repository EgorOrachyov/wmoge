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

#include "core/cmd_line.hpp"
#include "core/log.hpp"
#include "platform/application.hpp"
#include "profiler/profiler.hpp"
#include "system/config.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    /** 
     * @class HookProfiler
     * @brief Engine hook to setup profiler of game application
     */
    class HookProfiler : public Hook {
    public:
        ~HookProfiler() override = default;

        std::string get_name() const override {
            return "profiler";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_bool("profiler", "enable cpu profiler hook", "false");
        }

        Status on_process(CmdLine& cmd_line) override {
            Config*      config      = IocContainer::iresolve_v<Config>();
            Profiler*    profiler    = IocContainer::iresolve_v<Profiler>();
            Application* application = IocContainer::iresolve_v<Application>();

            enable_profiler = enable_profiler || cmd_line.get_bool("profiler");
            enable_profiler = enable_profiler || config->get_bool_or_default(SID("profiler.enable"), false);

            profiler->set_enabled(enable_profiler);

            if (enable_profiler) {
                WG_LOG_INFO("attach cpu performance profiler");

                application->signal_before_init.bind([this]() {
                    WG_PROFILE_CAPTURE_START(m_capture_startup, startup, "debug://profile_startup.json");
                });
                application->signal_after_init.bind([this]() {
                    WG_PROFILE_CAPTURE_END(m_capture_startup);
                });
                application->signal_before_loop.bind([this]() {
                    WG_PROFILE_CAPTURE_START(m_capture_runtime, runtime, "debug://profile_runtime.json");
                });
                application->signal_after_loop.bind([this]() {
                    WG_PROFILE_CAPTURE_END(m_capture_runtime);
                });
                application->signal_before_shutdown.bind([this]() {
                    WG_PROFILE_CAPTURE_START(m_capture_shutdown, shutdown, "debug://profile_shutdown.json");
                });
                application->signal_after_shutdown.bind([this]() {
                    WG_PROFILE_CAPTURE_END(m_capture_shutdown);
                });
            }

            return WG_OK;
        }

    private:
        std::shared_ptr<ProfilerCapture> m_capture_startup;
        std::shared_ptr<ProfilerCapture> m_capture_runtime;
        std::shared_ptr<ProfilerCapture> m_capture_shutdown;

        bool enable_profiler = false;
    };

}// namespace wmoge