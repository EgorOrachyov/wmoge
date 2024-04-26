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
#include "io/enum.hpp"
#include "system/config_file.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    /** 
     * @class HookLogs
     * @brief Engine hook to configure and and init engine logs
     */
    class HookLogs : public Hook {
    public:
        ~HookLogs() override = default;

        std::string get_name() const override {
            return "logs";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_bool("disable_logs", "disable all logs entirely (overrides config)", "false");
        }

        Status on_process(CmdLine& cmd_line) override {
            ConfigFile* config = IocContainer::instance()->resolve<ConfigFile>().value();

            const bool no_logs = cmd_line.get_bool("disable_logs");
            if (no_logs) {
                return StatusCode::Ok;
            }

            const bool log_to_out     = config->get_bool(SID("engine.log_to_out"), true);
            const bool log_to_file    = config->get_bool(SID("engine.log_to_file"), true);
            const bool log_to_console = config->get_bool(SID("engine.log_to_console"), true);

            const auto log_to_out_level     = Enum::parse<LogLevel>(config->get_string(SID("engine.log_to_out_level"), "Info"));
            const auto log_to_file_level    = Enum::parse<LogLevel>(config->get_string(SID("engine.log_to_file_level"), "Info"));
            const auto log_to_console_level = Enum::parse<LogLevel>(config->get_string(SID("engine.log_to_console_level"), "Info"));

            if (log_to_file) {
                auto log_listener_stream = std::make_shared<LogListenerStream>("file", log_to_file_level);
                Log::instance()->listen(log_listener_stream);
                WG_LOG_INFO("attach file log listener");
            }
            if (log_to_out) {
                auto log_listener_stdout = std::make_shared<LogListenerStdout>("out", log_to_out_level);
                Log::instance()->listen(log_listener_stdout);
                WG_LOG_INFO("attach stdout log listener");
            }
            if (log_to_console) {
                auto log_listener_console = std::make_shared<LogListenerConsole>(IocContainer::instance()->resolve_v<Console>(), log_to_console_level);
                Log::instance()->listen(log_listener_console);
                WG_LOG_INFO("attach console log listener");
            }

            return StatusCode::Ok;
        }
    };

}// namespace wmoge