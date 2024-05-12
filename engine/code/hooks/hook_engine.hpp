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
#include "platform/file_system.hpp"
#include "system/config.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    /** 
     * @class HookEngine
     * @brief Engine hook to setup runtime of the game
     */
    class HookEngine : public Hook {
    public:
        ~HookEngine() override = default;

        std::string get_name() const override {
            return "engine";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_string("engine_remap", "remap for engine directory", "");
            cmd_line.add_string("engine_config", "path to engine config", "engine://config/");
        }

        Status on_process(CmdLine& cmd_line) override {
            Config*     config = IocContainer::iresolve_v<Config>();
            FileSystem* fs     = IocContainer::iresolve_v<FileSystem>();

            const std::string engine_remap = cmd_line.get_string("engine_remap");
            if (!engine_remap.empty()) {
                config->set_string(SID("file_system.engine_path"), engine_remap, true);
            }

            fs->setup_mappings();

            const std::string config_path = cmd_line.get_string("engine_config");

            if (!config->load(config_path + "/engine.cfg", ConfigStackMode::Keep)) {
                std::cerr << "failed to load engine engine.cfg file, check your configuration file or path";
            }
            if (!config->load(config_path + "/game.cfg", ConfigStackMode::Keep)) {
                std::cerr << "failed to load engine game.cfg file, check your configure file of path";
            }
            if (!config->load(config_path + "/cvars.cfg", ConfigStackMode::Keep)) {
                std::cerr << "failed to load engine cvars.cfg file, check your configure file of path";
            }

            return WG_OK;
        }
    };

}// namespace wmoge