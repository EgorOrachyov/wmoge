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

#include "application.hpp"

#include "asset/asset_manager.hpp"
#include "audio/openal/al_engine.hpp"
#include "core/callback_queue.hpp"
#include "core/class.hpp"
#include "core/cmd_line.hpp"
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "ecs/ecs_registry.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "glsl/glsl_shader_compiler.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "hooks/hook_config.hpp"
#include "hooks/hook_engine.hpp"
#include "hooks/hook_logs.hpp"
#include "hooks/hook_profiler.hpp"
#include "hooks/hook_root_remap.hpp"
#include "hooks/hook_uuid_gen.hpp"
#include "io/enum.hpp"
#include "mesh/mesh_manager.hpp"
#include "platform/application.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler.hpp"
#include "render/render_engine.hpp"
#include "render/view_manager.hpp"
#include "rtti/type_storage.hpp"
#include "scene/scene_manager.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/script_system.hpp"
#include "system/config.hpp"
#include "system/console.hpp"
#include "system/engine.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"
#include "system/plugin_manager.hpp"

#include "asset/_rtti.hpp"
#include "audio/_rtti.hpp"
#include "glsl/_rtti.hpp"
#include "grc/_rtti.hpp"
#include "material/_rtti.hpp"
#include "mesh/_rtti.hpp"
#include "pfx/_rtti.hpp"
#include "render/_rtti.hpp"
#include "rtti/_rtti.hpp"
#include "scene/_rtti.hpp"
#include "scripting/_rtti.hpp"
#include "system/_rtti.hpp"

namespace wmoge {

    static void bind_globals(IocContainer* ioc) {
        ioc->bind<HookList>();
        ioc->bind<CmdLine>();
        ioc->bind<DllManager>();
        ioc->bind<PluginManager>();
        ioc->bind<Time>();
        ioc->bind<Config>();
        ioc->bind<FileSystem>();
        ioc->bind<Console>();
        ioc->bind<CallbackQueue>();
        ioc->bind<GlslShaderCompiler>();
        ioc->bind<ShaderLibrary>();
        ioc->bind<ShaderManager>();
        ioc->bind<PsoCache>();
        ioc->bind<TextureManager>();
        ioc->bind<MeshManager>();
        ioc->bind<RenderEngine>();
        ioc->bind<AssetManager>();
        ioc->bind<EcsRegistry>();
        ioc->bind<SceneManager>();
        ioc->bind<ViewManager>();

        ioc->bind_f<TaskManager, TaskManager>([ioc]() {
            Config*   config      = ioc->resolve_v<Config>();
            const int num_workers = config->get_int_or_default(SID("task_manager.workers"), 4);
            return std::make_shared<TaskManager>(num_workers);
        });

        ioc->bind_f<ShaderCompilerTaskManager, ShaderCompilerTaskManager>([ioc]() {
            Config*   config      = ioc->resolve_v<Config>();
            const int num_workers = config->get_int_or_default(SID("grc.shader.compiler.workers"), 4);
            return std::make_shared<ShaderCompilerTaskManager>(num_workers);
        });

        ioc->bind_f<GlfwWindowManager, GlfwWindowManager>([ioc]() {
            Config*    config     = ioc->resolve_v<Config>();
            const bool vsync      = config->get_bool_or_default(SID("gfx.vsync"), true);
            const bool client_api = false;
            return std::make_shared<GlfwWindowManager>(vsync, client_api);
        });

        ioc->bind_f<GlfwInput, GlfwInput>([ioc]() {
            GlfwWindowManager* window_manager = ioc->resolve_v<GlfwWindowManager>();
            return window_manager->input();
        });

        ioc->bind_f<VKDriver, VKDriver>([ioc]() {
            GlfwWindowManager* window_manager = ioc->resolve_v<GlfwWindowManager>();
            Ref<Window>        window         = window_manager->get_primary_window();

            VKInitInfo init_info;
            init_info.window       = window;
            init_info.app_name     = window->title();
            init_info.engine_name  = "wmoge";
            init_info.required_ext = window_manager->extensions();
            init_info.factory      = window_manager->factory();

            return std::make_shared<VKDriver>(std::move(init_info));
        });

        ioc->bind_f<GfxDriver, GfxDriver>([ioc]() {
            return std::shared_ptr<GfxDriver>(ioc->resolve_v<VKDriver>(), [](auto p) {});
        });

        ioc->bind_f<RttiTypeStorage, RttiTypeStorage>([]() {
            auto type_storage = std::make_shared<RttiTypeStorage>();
            RttiTypeStorage::provide(type_storage.get());
            return type_storage;
        });

        ioc->bind_f<Profiler, Profiler>([]() {
            auto profiler = std::make_shared<Profiler>();
            Profiler::provide(profiler.get());
            return profiler;
        });

        ioc->bind_f<Log, Log>([]() {
            auto log = std::make_shared<Log>();
            Log::provide(log.get());
            return log;
        });

        ioc->bind_f<Engine, Engine>([]() {
            auto engine = std::make_shared<Engine>();
            Engine::provide(engine.get());
            return engine;
        });
    }

    static void unbind_globals(IocContainer* ioc) {
        ioc->unbind<ViewManager>();
        ioc->unbind<SceneManager>();
        ioc->unbind<PsoCache>();
        ioc->unbind<ShaderManager>();
        ioc->unbind<ShaderLibrary>();
        ioc->unbind<ShaderCompilerTaskManager>();
        ioc->unbind<GlslShaderCompiler>();
        ioc->unbind<TextureManager>();
        ioc->unbind<MeshManager>();
        ioc->unbind<RenderEngine>();
        ioc->unbind<TaskManager>();
        ioc->unbind<VKDriver>();
        ioc->unbind<GlfwInput>();
        ioc->unbind<GlfwWindowManager>();
        ioc->unbind<AssetManager>();
        ioc->unbind<PluginManager>();
        ioc->unbind<DllManager>();
    }

    static void bind_rtti(IocContainer* ioc) {
        ioc->resolve_v<RttiTypeStorage>();
        rtti_rtti();
        rtti_asset();
        rtti_audio();
        rtti_grc();
        rtti_glsl();
        rtti_material();
        rtti_mesh();
        rtti_pfx();
        rtti_render();
        rtti_scene();
        rtti_scripting();
        rtti_system();
    }

    int Application::run(int argc, const char* const* argv) {
        IocContainer ioc;
        IocContainer::provide(&ioc);

        bind_globals(&ioc);
        bind_rtti(&ioc);

        Log* log = ioc.resolve<Log>().value();

        ioc.bind_i<Application>(std::shared_ptr<Application>(this, [](auto p) {}));

        on_register();

        CmdLine* cmd_line = ioc.resolve<CmdLine>().value();
        cmd_line->add_bool("h,help", "display help message", "false");

        signal_hook.emit();

        HookList* hook_list = ioc.resolve<HookList>().value();
        hook_list->each([&](HookList::HookPtr& hook) {
            hook->on_add_cmd_line_options(*cmd_line);
            return false;
        });

        if (!cmd_line->parse(argc, argv)) {
            return 1;
        }
        if (cmd_line->get_bool("help")) {
            std::cout << cmd_line->get_help();
            return 0;
        }

        std::optional<int> ret_code;

        hook_list->each([&](HookList::HookPtr& hook) {
            const Status     status = hook->on_process(*cmd_line);
            const StatusCode code   = status.code();

            if (code == StatusCode::ExitCode0) {
                ret_code = 0;
                std::cout << "exit code 0" << std::endl;
                return true;
            }
            if (code == StatusCode::ExitCode1) {
                ret_code = 1;
                std::cerr << "exit code 1 after " << hook->get_name() << std::endl;
                return true;
            }
            if (code != StatusCode::Ok) {
                std::cerr << "error " << status << " after " << hook->get_name() << std::endl;
                return true;
            }

            return false;
        });

        if (ret_code.has_value()) {
            return ret_code.value();
        }

        signal_before_init.emit();
        {
            WG_AUTO_PROFILE_PLATFORM("Application::initialize");

            if (!on_init()) {
                return 1;
            }
        }
        signal_after_init.emit();

        signal_before_loop.emit();
        {
            while (!should_close()) {
                WG_AUTO_PROFILE_PLATFORM("Application::iteration");

                if (!on_loop()) {
                    return 1;
                }
            }
        }
        signal_after_loop.emit();

        signal_before_shutdown.emit();
        {
            WG_AUTO_PROFILE_PLATFORM("Application::shutdown");

            if (!on_shutdown()) {
                return 1;
            }

            unbind_globals(&ioc);
        }
        signal_after_shutdown.emit();

        return 0;
    }

    Status GameApplication::on_register() {
        signal_hook.bind([]() {
            HookList* hook_list = IocContainer::iresolve_v<HookList>();

            hook_list->attach(std::make_shared<HookUuidGen>());
            hook_list->attach(std::make_shared<HookRootRemap>());
            hook_list->attach(std::make_shared<HookEngine>());
            hook_list->attach(std::make_shared<HookLogs>());
            hook_list->attach(std::make_shared<HookProfiler>());
        });

        return IocContainer::iresolve_v<Engine>()->setup(this);
    }

    Status GameApplication::on_init() {
        return Engine::instance()->init();
    }

    Status GameApplication::on_loop() {
        return Engine::instance()->iteration();
    }

    Status GameApplication::on_shutdown() {
        return Engine::instance()->shutdown();
    }

    bool GameApplication::should_close() {
        return Engine::instance()->close_requested();
    }

}// namespace wmoge