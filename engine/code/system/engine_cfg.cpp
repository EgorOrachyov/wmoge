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

#include "engine_cfg.hpp"

#include "io/cfg_val_manager.hpp"

namespace wmoge {

    void EngineCfg::Log::bind(CfgValManager* cfg_manager) {
        WG_CFG_BIND_VAL(cfg_manager, to_out, "");
        WG_CFG_BIND_VAL(cfg_manager, to_out_level, "");
        WG_CFG_BIND_VAL(cfg_manager, to_file, "");
        WG_CFG_BIND_VAL(cfg_manager, to_file_level, "");
        WG_CFG_BIND_VAL(cfg_manager, to_console, "");
        WG_CFG_BIND_VAL(cfg_manager, to_console_level, "");
    }

    void EngineCfg::Profiler::bind(CfgValManager* cfg_manager) {
        WG_CFG_BIND_VAL(cfg_manager, enable, "");
        WG_CFG_BIND_VAL(cfg_manager, trace_cpu, "");
        WG_CFG_BIND_VAL(cfg_manager, trace_gpu, "");
    }

    void EngineCfg::bind(CfgValManager* cfg_manager) {
        log.bind(cfg_manager);
        profiler.bind(cfg_manager);
    }

}// namespace wmoge