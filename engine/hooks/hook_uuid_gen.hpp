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
#include "core/hook.hpp"
#include "core/uuid.hpp"

namespace wmoge {

    /** 
     * @class HookUuidGen
     * @brief Engine hook to generate uuids
     */
    class HookUuidGen : public Hook {
    public:
        ~HookUuidGen() override = default;

        std::string get_name() const override {
            return "uuid_gen";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_int("gen_uuids", "gen desired count of uuids' values and outputs them", "0");
        }

        Status on_process(CmdLine& cmd_line) override {
            const int uuid_count = cmd_line.get_int("gen_uuids");

            if (uuid_count > 0) {
                for (int i = 0; i < uuid_count; i++) {
                    std::cout << UUID::generate() << std::endl;
                }
                return StatusCode::ExitCode0;
            }

            return StatusCode::Ok;
        }
    };

}// namespace wmoge