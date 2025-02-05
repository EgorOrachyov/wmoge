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

#include "edt_system/edt_plugin.hpp"
#include "system/app.hpp"
#include "system/engine_app.hpp"
#include "system/game_plugin.hpp"

namespace wmoge {

    /**
     * @class EngineApplication
     * @brief Base class for application to run stand-alone game
    */
    struct EdtApplicationConfig {
        EngineApplicationConfig*  app_config;
        GamePluginPtr             game_plugin;
        std::vector<EdtPluginPtr> plugins;
    };

    /**
     * @class EngineApplication
     * @brief Base class for application to run stand-alone game
    */
    class EdtApplication : public EngineApplication {
    public:
        EdtApplication(EdtApplicationConfig& config);

    protected:
        EdtApplicationConfig& m_edt_config;
        class EdtEditor*      m_editor = nullptr;
    };

}// namespace wmoge