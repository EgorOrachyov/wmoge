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

#include "application.hpp"

#include "core/cmd_line.hpp"
#include "core/engine.hpp"
#include "core/hook.hpp"
#include "debug/profiler.hpp"
#include "hooks/hook_config.hpp"
#include "hooks/hook_logs.hpp"
#include "hooks/hook_root_remap.hpp"
#include "hooks/hook_uuid_gen.hpp"
#include "main/main.hpp"

namespace wmoge {

    int Application::run(int argc, const char* const* argv) {
        Main main(this);

        auto* engine    = Engine::instance();
        auto* hook_list = engine->hook_list();
        auto* cmd_line  = engine->cmd_line();

        cmd_line->add_bool("h,help", "display help message", "false");

        hook_list->attach(std::make_shared<HookUuidGen>());
        hook_list->attach(std::make_shared<HookRootRemap>());
        hook_list->attach(std::make_shared<HookConfig>());
        hook_list->attach(std::make_shared<HookLogs>());

        hook_list->each([&](HookList::HookPtr& hook) {
            hook->on_add_cmd_line_options(*cmd_line);
            return false;
        });

        if (!cmd_line->parse(argc, argv)) {
            return 1;
        }
        if (cmd_line->get_bool("help")) {
            std::cout << cmd_line->get_help();
            return 0;
        }

        std::optional<int> ret_code;

        hook_list->each([&](HookList::HookPtr& hook) {
            const Status     status = hook->on_process(*cmd_line, *engine);
            const StatusCode code   = status.code();

            if (code == StatusCode::ExitCode0) {
                ret_code = 0;
                std::cout << "exit code 0" << std::endl;
                return true;
            }
            if (code == StatusCode::ExitCode1) {
                ret_code = 1;
                std::cerr << "exit code 1 after " << hook->get_name() << std::endl;
                return true;
            }
            if (code != StatusCode::Ok) {
                std::cerr << "error " << status << " after " << hook->get_name() << std::endl;
                return true;
            }

            return false;
        });

        if (ret_code.has_value()) {
            return ret_code.value();
        }

        WG_PROFILE_CAPTURE_START(startup, "debug://wmoge_profile_startup.json");

        if (!main.initialize()) {
            return 1;
        }

        WG_PROFILE_CAPTURE_END(startup);

        WG_PROFILE_CAPTURE_START(runtime, "debug://wmoge_profile_runtime.json");

        while (!engine->close_requested()) {
            if (!main.iteration()) {
                return 1;
            }
        }

        WG_PROFILE_CAPTURE_END(runtime);

        WG_PROFILE_CAPTURE_START(shutdown, "debug://wmoge_profile_shutdown.json");

        if (!main.shutdown()) {
            return 1;
        }

        WG_PROFILE_CAPTURE_END(shutdown);

        return 0;
    }

}// namespace wmoge