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

#include "app.hpp"

#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Application::Application(ApplicationConfig& config) : m_config(config) {
    }

    int Application::run() {
        IocContainer* ioc = m_config.ioc;

        ioc->bind_by_pointer<Application>(this);

        if (!on_register()) {
            return 1;
        }

        CmdLineOptions*  cmd_line_options = m_config.cmd_line->options;
        CmdLineHookList* cmd_line_hooks   = m_config.cmd_line->hooks;

        cmd_line_options->add_bool("h,help", "display help message", "false");

        auto cmd_parse_result = cmd_line_options->parse(m_config.cmd_line->args);
        if (!cmd_parse_result) {
            return 1;
        }

        if (cmd_parse_result->get_bool("help")) {
            std::cout << cmd_line_options->get_help();
            return 0;
        }

        const Status status = cmd_line_hooks->process(*cmd_parse_result);

        if (status.code() == StatusCode::ExitCode0) {
            std::cout << "exit code 0" << std::endl;
            return true;
        }
        if (status.code() == StatusCode::ExitCode1) {
            std::cerr << "exit code 1" << std::endl;
            return 1;
        }
        if (status.code() != StatusCode::Ok) {
            std::cerr << "error " << status << std::endl;
            return 2;
        }

        ApplicationSignals* signals = m_config.signals;

        signals->before_init.emit();
        {
            WG_PROFILE_CPU_PLATFORM("Application::initialize");

            if (!on_init()) {
                return 1;
            }
        }
        signals->after_init.emit();

        signals->before_loop.emit();
        {
            while (!should_close()) {
                WG_PROFILE_CPU_PLATFORM("Application::iteration");

                if (!on_iteration()) {
                    return 1;
                }
            }
        }
        signals->after_loop.emit();

        signals->before_shutdown.emit();
        {
            WG_PROFILE_CPU_PLATFORM("Application::shutdown");

            if (!on_shutdown()) {
                return 1;
            }
        }
        signals->after_shutdown.emit();

        return 0;
    }

}// namespace wmoge