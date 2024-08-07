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
#include "system/hook.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    /** 
     * @class HookRootRemap
     * @brief Engine hook to remap root folder to other (for debug moslty)
     */
    class HookRootRemap : public Hook {
    public:
        ~HookRootRemap() override = default;

        std::string get_name() const override {
            return "root_remap";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_string("root_remap", "remap path to game root folder", "");
        }

        Status on_process(CmdLine& cmd_line) override {
            FileSystem* fs = IocContainer::iresolve_v<FileSystem>();

            const std::string root_remap = cmd_line.get_string("root_remap");
            if (!root_remap.empty()) {
                fs->root(root_remap);
                std::cout << "remap game exe root direcory to " << root_remap << std::endl;
            }

            return WG_OK;
        }
    };

}// namespace wmoge