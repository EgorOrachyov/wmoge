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
#include "core/ioc_container.hpp"
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
#include "io/async_file_system.hpp"
#include "io/enum.hpp"
#include "mesh/mesh_manager.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler_capture.hpp"
#include "profiler/profiler_cpu.hpp"
#include "render/render_engine.hpp"
#include "render/view_manager.hpp"
#include "rtti/type_storage.hpp"
#include "scene/scene_manager.hpp"
#include "system/config.hpp"
#include "system/console.hpp"
#include "system/engine.hpp"
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
#include "system/_rtti.hpp"

namespace wmoge {

    static void bind_globals(IocContainer* ioc) {
        ioc->bind_by_pointer<Log>(Log::instance());
        ioc->bind_by_ioc<ProfilerCapture>();
        ioc->bind_by_pointer<RttiTypeStorage>(RttiTypeStorage::instance());
        ioc->bind_by_ioc<DllManager>();
        ioc->bind<PluginManager>();
        ioc->bind<Time>();
        ioc->bind_by_ioc<Config>();
        ioc->bind<FileSystem>();
        ioc->bind<Console>();
        ioc->bind<CallbackQueue>();
        ioc->bind_by_ioc<GlslShaderCompiler>();
        ioc->bind_by_ioc<ShaderLibrary>();
        ioc->bind_by_ioc<ShaderManager>();
        ioc->bind_by_ioc<PsoCache>();
        ioc->bind_by_ioc<TextureManager>();
        ioc->bind_by_ioc<MeshManager>();
        ioc->bind<RenderEngine>();
        ioc->bind_by_ioc<AssetManager>();
        ioc->bind<EcsRegistry>();
        ioc->bind_by_ioc<SceneManager>();
        ioc->bind<ViewManager>();

        ioc->bind_by_factory<IoAsyncFileSystem>([ioc]() {
            Config*   config      = ioc->resolve_value<Config>();
            const int num_workers = config->get_int_or_default(SID("io.num_workers"), 4);
            return std::make_shared<IoAsyncFileSystem>(ioc, num_workers);
        });

        ioc->bind_by_factory<TaskManager>([ioc]() {
            Config*   config      = ioc->resolve_value<Config>();
            const int num_workers = config->get_int_or_default(SID("task_manager.workers"), 4);
            return std::make_shared<TaskManager>(num_workers);
        });

        ioc->bind_by_factory<ShaderTaskManager>([ioc]() {
            Config*   config      = ioc->resolve_value<Config>();
            const int num_workers = config->get_int_or_default(SID("grc.shader.compiler.workers"), 4);
            return std::make_shared<ShaderTaskManager>(num_workers);
        });

        ioc->bind_by_factory<GlfwWindowManager>([ioc]() {
            Config*    config     = ioc->resolve_value<Config>();
            const bool vsync      = config->get_bool_or_default(SID("gfx.vsync"), true);
            const bool client_api = false;
            return std::make_shared<GlfwWindowManager>(vsync, client_api);
        });

        ioc->bind_by_factory<GlfwInput>([ioc]() {
            GlfwWindowManager* window_manager = ioc->resolve_value<GlfwWindowManager>();
            return window_manager->input();
        });

        ioc->bind_by_factory<VKDriver>([ioc]() {
            GlfwWindowManager* window_manager = ioc->resolve_value<GlfwWindowManager>();
            Ref<Window>        window         = window_manager->get_primary_window();

            VKInitInfo init_info;
            init_info.ioc          = ioc;
            init_info.window       = window;
            init_info.app_name     = window->title();
            init_info.engine_name  = "wmoge";
            init_info.required_ext = window_manager->extensions();
            init_info.factory      = window_manager->factory();

            return std::make_shared<VKDriver>(init_info);
        });

        ioc->bind_by_factory<GfxDriver>([ioc]() {
            return std::shared_ptr<GfxDriver>(ioc->resolve_value<VKDriver>(), [](auto p) {});
        });

        ioc->bind_by_ioc<Engine>();
    }

    static void unbind_globals(IocContainer* ioc) {
        ioc->unbind<ViewManager>();
        ioc->unbind<SceneManager>();
        ioc->unbind<PsoCache>();
        ioc->unbind<ShaderManager>();
        ioc->unbind<ShaderLibrary>();
        ioc->unbind<ShaderTaskManager>();
        ioc->unbind<GlslShaderCompiler>();
        ioc->unbind<TextureManager>();
        ioc->unbind<MeshManager>();
        ioc->unbind<RenderEngine>();
        ioc->unbind<TaskManager>();
        ioc->unbind<VKDriver>();
        ioc->unbind<GlfwInput>();
        ioc->unbind<GlfwWindowManager>();
        ioc->unbind<AssetManager>();
        ioc->unbind<IoAsyncFileSystem>();
        ioc->unbind<PluginManager>();
        ioc->unbind<DllManager>();
    }

    static void bind_rtti(IocContainer* ioc) {
        ioc->resolve_value<RttiTypeStorage>();
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
        rtti_system();
    }

    Application::Application(ApplicationConfig& config) : m_config(config) {
    }

    int Application::run() {
        IocContainer* ioc = m_config.ioc;

        bind_globals(ioc);
        bind_rtti(ioc);

        ioc->bind_by_pointer<Application>(this);

        on_register();

        CmdLineOptions*  cmd_line_options = m_config.cmd_line.options;
        CmdLineHookList* cmd_line_hooks   = m_config.cmd_line.hooks;

        cmd_line_options->add_bool("h,help", "display help message", "false");

        auto cmd_parse_result = cmd_line_options->parse(m_config.cmd_line.args);
        if (!cmd_parse_result) {
            return 1;
        }

        if (cmd_parse_result->get_bool("help")) {
            std::cout << cmd_line_options->get_help();
            return 0;
        }

        const Status status = cmd_line_hooks->process(*cmd_parse_result);

        if (status.code() == StatusCode::ExitCode0) {
            std::cout << "exit code 0" << std::endl;
            return true;
        }
        if (status.code() == StatusCode::ExitCode1) {
            std::cerr << "exit code 1" << std::endl;
            return 1;
        }
        if (status.code() != StatusCode::Ok) {
            std::cerr << "error " << status << std::endl;
            return 2;
        }

        ApplicationSignals& signals = m_config.signals;

        signals.before_init.emit();
        {
            WG_PROFILE_CPU_PLATFORM("Application::initialize");

            if (!on_init()) {
                return 1;
            }
        }
        signals.after_init.emit();

        signals.before_loop.emit();
        {
            while (!should_close()) {
                WG_PROFILE_CPU_PLATFORM("Application::iteration");

                if (!on_loop()) {
                    return 1;
                }
            }
        }
        signals.after_loop.emit();

        signals.before_shutdown.emit();
        {
            WG_PROFILE_CPU_PLATFORM("Application::shutdown");

            if (!on_shutdown()) {
                return 1;
            }

            unbind_globals(ioc);
        }
        signals.after_shutdown.emit();

        return 0;
    }

}// namespace wmoge