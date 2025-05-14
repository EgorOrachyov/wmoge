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

#include <engine.hpp>

// Game plugin include
#include <game.hpp>

// Egnine plug-ins section
#include "assimp_plugin.hpp"
#include "freetype_plugin.hpp"
#include "imgui_plugin.hpp"
#include "importers_plugin.hpp"
#include "runtime_plugin.hpp"
#include "stbimage_plugin.hpp"

// Editor plug-ins section

#ifdef WMOGE_BUILD_TYPE_EDITOR
    #include <editor.hpp>
#endif

int main(int argc, const char* const* argv) {
#ifdef WMOGE_BUILD_TYPE_GAME
    const std::string app_name = "template";
    const std::string app_desc = "wmoge template game app for testing";
#endif

#ifdef WMOGE_BUILD_TYPE_EDITOR
    const std::string app_name = "template";
    const std::string app_desc = "wmoge template game editor app for testing";
#endif

    IocContainer    ioc_containter;
    CmdLineHookList hooks;
    CmdLineOptions  options(app_name, app_desc);

    ApplicationSignals app_signlas;

    eng_hooks::uuid_gen(options, hooks);
    eng_hooks::root_remap(options, hooks, &ioc_containter);
    eng_hooks::engine(options, hooks, &ioc_containter);
#ifdef WMOGE_BUILD_TYPE_EDITOR
    edt_hooks::editor(options, hooks, &ioc_containter);
#endif
    eng_hooks::logs(options, hooks, &ioc_containter);
    eng_hooks::profiler(options, hooks, &ioc_containter, &app_signlas);

    ApplicationCmdLine cmd_line;
    cmd_line.options = &options;
    cmd_line.hooks   = &hooks;
    cmd_line.line    = CmdLineUtil::to_string(argc, argv);
    cmd_line.args    = CmdLineUtil::to_vector(argc, argv);

    ApplicationConfig app_config;
    app_config.name     = app_name;
    app_config.ioc      = &ioc_containter;
    app_config.signals  = &app_signlas;
    app_config.cmd_line = &cmd_line;

    EngineSignals engine_signals;

    EngineApplicationConfig engine_app_config;
    engine_app_config.app_config = &app_config;
    engine_app_config.signals    = &engine_signals;
    engine_app_config.plugins    = {
            std::make_shared<RuntimePlugin>(),
            std::make_shared<ImportersPlugin>(),
            std::make_shared<AssimpPlugin>(),
            std::make_shared<FreetypePlugin>(),
            std::make_shared<StbimagePlugin>(),
            std::make_shared<ImguiPlugin>()};

#ifdef WMOGE_BUILD_TYPE_GAME
    GameApplicationConfig game_app_config;
    game_app_config.app_config  = &engine_app_config;
    game_app_config.game_info   = "template game application";
    game_app_config.game_plugin = std::make_shared<TemplateGame>();

    GameApplication app(game_app_config);
#endif

#ifdef WMOGE_BUILD_TYPE_EDITOR
    EdtApplicationConfig edt_app_config;
    edt_app_config.app_config  = &engine_app_config;
    edt_app_config.plugins     = {};
    edt_app_config.game_plugin = std::make_shared<TemplateGame>();

    EdtApplication app(edt_app_config);
#endif

    return app.run();
}

#if !defined(WMOGE_BUILD_TYPE_GAME) && !defined(WMOGE_BUILD_TYPE_EDITOR)
    #error "Invalid executable build type"
#endif