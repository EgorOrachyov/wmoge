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
#include "system/config.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"

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
            cmd_line.add_string("root_config", "path to exe config", "root://config/");
        }

        Status on_process(CmdLine& cmd_line) override {
            Config*     config = IocContainer::iresolve_v<Config>();
            FileSystem* fs     = IocContainer::iresolve_v<FileSystem>();

            const std::string path = cmd_line.get_string("root_config");

            if (!config->load(path + "/engine.cfg")) {
                std::cerr << "failed to load exe engine.cfg file, check your configuration file or path";
            }
            if (!config->load(path + "/game.cfg")) {
                std::cerr << "failed to load exe game.cfg file, check your configure file of path";
            }
            if (!config->load(path + "/cvars.cfg")) {
                std::cerr << "failed to load exe cvars.cfg file, check your configure file of path";
            }

            fs->setup_mappings();

            return WG_OK;
        }
    };

}// namespace wmoge