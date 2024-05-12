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

#include "core/signal.hpp"
#include "core/status.hpp"

namespace wmoge {

    /**
     * @class Application
     * @brief Base class for any application which is an entry point to run
     */
    class Application {
    public:
        virtual ~Application() = default;

        Signal<> signal_hook;
        Signal<> signal_before_init;
        Signal<> signal_after_init;
        Signal<> signal_before_loop;
        Signal<> signal_after_loop;
        Signal<> signal_before_shutdown;
        Signal<> signal_after_shutdown;

        virtual Status on_register() { return WG_OK; }
        virtual Status on_init() { return WG_OK; }
        virtual Status on_loop() { return WG_OK; }
        virtual Status on_shutdown() { return WG_OK; }
        virtual bool   should_close() { return true; }
        virtual void   requiest_close() {}

        [[nodiscard]] int run(int argc, const char* const* argv);
    };

    /**
     * @class GameApplication
     * @brief Base class for application to run stand-alone game
    */
    class GameApplication : public Application {
    public:
        ~GameApplication() override = default;

        Status on_register() override;
        Status on_init() override;
        Status on_loop() override;
        Status on_shutdown() override;
        bool   should_close() override;
    };

    /**
     * @class ToolApplication
     * @brief Base class for application to run command-line based tool
    */
    class ToolApplication : public Application {
    public:
        ~ToolApplication() override = default;
    };

}// namespace wmoge