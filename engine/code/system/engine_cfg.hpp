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

#include "core/log.hpp"
#include "io/cfg_val.hpp"

namespace wmoge {

    class CfgValManager;

    /**
     * @brief Config variables collection for engine-wide common sub-systems 
     */
    class EngineCfg {
    public:
        struct Log {
            CfgValBool           to_out{"engine.log.to_out", true};
            CfgValEnum<LogLevel> to_out_level{"engine.log.to_out_level", LogLevel::Info};
            CfgValBool           to_file{"engine.log.to_file", true};
            CfgValEnum<LogLevel> to_file_level{"engine.log.to_file_level", LogLevel::Info};
            CfgValBool           to_console{"engine.log.to_console", true};
            CfgValEnum<LogLevel> to_console_level{"engine.log.to_console_level", LogLevel::Info};

            void bind(CfgValManager* cfg_manager);
        };

        struct Profiler {
            CfgValBool enable{"engine.profiler.enable", false};
            CfgValBool trace_cpu{"engine.profiler.trace_cpu", true};
            CfgValBool trace_gpu{"engine.profiler.trace_gpu", true};

            void bind(CfgValManager* cfg_manager);
        };

        Log      log;
        Profiler profiler;

        void bind(CfgValManager* cfg_manager);
    };

}// namespace wmoge