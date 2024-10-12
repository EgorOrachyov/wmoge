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
#include "core/ioc_container.hpp"
#include "core/signal.hpp"
#include "core/status.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class ApplicationCmdLine
     * @brief App command line processing params
     */
    struct ApplicationCmdLine {
        CmdLineOptions*          options;
        CmdLineHookList*         hooks;
        std::string              line;
        std::vector<std::string> args;
    };

    /**
     * @class ApplicationSignals
     * @brief App signals to intercept some events
     */
    struct ApplicationSignals {
        Signal<> before_init;
        Signal<> after_init;
        Signal<> before_loop;
        Signal<> after_loop;
        Signal<> before_shutdown;
        Signal<> after_shutdown;
    };

    /**
     * @class ApplicationConfig
     * @brief Desc containing info to setup application
     */
    struct ApplicationConfig {
        std::string         name;
        IocContainer*       ioc;
        ApplicationSignals* signals;
        ApplicationCmdLine* cmd_line;
    };

    /**
     * @class Application
     * @brief Base class for any application which is an entry point to run
     */
    class Application {
    public:
        Application(ApplicationConfig& config);
        virtual ~Application() = default;

        virtual Status on_register() { return WG_OK; }
        virtual Status on_init() { return WG_OK; }
        virtual Status on_iteration() { return WG_OK; }
        virtual Status on_shutdown() { return WG_OK; }
        virtual bool   should_close() { return true; }
        virtual void   requiest_close() {}

        [[nodiscard]] int run();

    protected:
        ApplicationConfig& m_config;
    };

}// namespace wmoge