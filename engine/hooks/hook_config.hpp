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

#ifndef WMOGE_HOOK_CONFIG_HPP
#define WMOGE_HOOK_CONFIG_HPP

#include "core/cmd_line.hpp"
#include "core/engine.hpp"
#include "core/hook.hpp"
#include "debug/profiler.hpp"
#include "resource/config_file.hpp"

namespace wmoge {

    /** 
     * @class HookConfig
     * @brief Engine hook to setup common config workflow
     */
    class HookConfig : public Hook {
    public:
        ~HookConfig() override = default;

        std::string get_name() const override {
            return "config";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_string("config_engine", "path to engine config", "root://config/engine.cfg");
            cmd_line.add_string("config_game", "path to game config", "root://config/game.cfg");
        }

        Status on_process(CmdLine& cmd_line, class Engine& engine) override {
            ConfigFile* config   = engine.config();
            Profiler*   profiler = engine.profiler();

            if (!config->load_and_stack(cmd_line.get_string("config_engine"))) {
                std::cerr << "failed to load config engine file, check your configure";
            }
            if (!config->load_and_stack(cmd_line.get_string("config_game"))) {
                std::cerr << "failed to load config game file, check your configure";
            }

            profiler->set_enabled(config->get_bool(SID("debug.profiler"), false));

            return StatusCode::Ok;
        }
    };

}// namespace wmoge

#endif//WMOGE_HOOK_CONFIG_HPP