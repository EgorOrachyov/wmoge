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
#include "debug/profiler.hpp"
#include "main/main.hpp"
#include "resource/config_file.hpp"

namespace wmoge {

    int Application::run(int argc, const char* const* argv) {
        Main main(this);

        auto* engine   = Engine::instance();
        auto* cmd_line = engine->cmd_line();
        auto* config   = engine->config();

        cmd_line->add_bool("h,help", "display help message", "false");
        cmd_line->add_string("config_engine", "path to engine config", "root://config/engine.cfg");
        cmd_line->add_string("config_game", "path to game config", "root://config/game.cfg");

        if (!cmd_line->parse(argc, argv)) {
            return 1;
        }
        if (cmd_line->get_bool("help")) {
            std::cout << cmd_line->get_help();
            return 0;
        }

        ConfigFile config_engine;
        if (!config_engine.load(cmd_line->get_string("config_engine"))) {
            std::cerr << "failed to load config engine file, check your configure";
        }

        config->stack(config_engine);
        config_engine.clear();

        ConfigFile config_game;
        if (!config_game.load(cmd_line->get_string("config_game"))) {
            std::cerr << "failed to load config game file, check your configure";
        }

        config->stack(config_game);
        config_game.clear();

        if (!main.prepare()) {
            return 1;
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