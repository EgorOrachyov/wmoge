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
#include "io/config.hpp"

namespace wmoge {

    /**
     * @class EngineConfig
     * @brief Engine config params storage for loafing from .ini config storage
     */
    class EngineConfig {
    public:
        struct Log {
            bool     to_out           = true;
            LogLevel to_out_level     = LogLevel::Info;
            bool     to_file          = true;
            LogLevel to_file_level    = LogLevel::Info;
            bool     to_console       = true;
            LogLevel to_console_level = LogLevel::Info;

            friend Status config_read(Config* cfg, const std::string& section, Log& out) {
                WG_CFG_READ(cfg, section, out, to_out);
                WG_CFG_READ(cfg, section, out, to_out_level);
                WG_CFG_READ(cfg, section, out, to_file);
                WG_CFG_READ(cfg, section, out, to_file_level);
                WG_CFG_READ(cfg, section, out, to_console);
                WG_CFG_READ(cfg, section, out, to_console_level);
                return WG_OK;
            }
        };

        struct Window {
            std::string title        = "Wmoge Game";
            int         width        = 1280;
            int         height       = 720;
            bool        vsync        = true;
            bool        exit         = true;
            std::string icon_default = "engine/icons/wmoge-128.png";
            std::string icon_small   = "engine/icons/wmoge-64.png";

            friend Status config_read(Config* cfg, const std::string& section, Window& out) {
                WG_CFG_READ(cfg, section, out, title);
                WG_CFG_READ(cfg, section, out, width);
                WG_CFG_READ(cfg, section, out, height);
                WG_CFG_READ(cfg, section, out, vsync);
                WG_CFG_READ(cfg, section, out, exit);
                WG_CFG_READ(cfg, section, out, icon_default);
                WG_CFG_READ(cfg, section, out, icon_small);
                return WG_OK;
            }
        };

        struct Ecs {
            int chunk_size  = 16;
            int expand_size = 2;

            friend Status config_read(Config* cfg, const std::string& section, Ecs& out) {
                WG_CFG_READ(cfg, section, out, chunk_size);
                WG_CFG_READ(cfg, section, out, expand_size);
                return WG_OK;
            }
        };

        struct Profiler {
            bool enable     = true;
            bool enable_cpu = true;
            bool enable_gpu = true;

            friend Status config_read(Config* cfg, const std::string& section, Profiler& out) {
                WG_CFG_READ(cfg, section, out, enable);
                WG_CFG_READ(cfg, section, out, enable_cpu);
                WG_CFG_READ(cfg, section, out, enable_gpu);
                return WG_OK;
            }
        };

        Log      log;
        Window   window;
        Ecs      ecs;
        Profiler profiler;

        friend Status config_read(Config* cfg, const std::string& section, EngineConfig& out) {
            WG_CFG_READ(cfg, section, out, log);
            WG_CFG_READ(cfg, section, out, window);
            WG_CFG_READ(cfg, section, out, ecs);
            WG_CFG_READ(cfg, section, out, profiler);
            return WG_OK;
        }
    };

}// namespace wmoge