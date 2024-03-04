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

#include "audio/openal/al_engine.hpp"
#include "core/callback_queue.hpp"
#include "core/class.hpp"
#include "core/cmd_line.hpp"
#include "core/hook.hpp"
#include "core/ioc_container.hpp"
#include "core/layer.hpp"
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "debug/console.hpp"
#include "debug/debug_layer.hpp"
#include "debug/profiler.hpp"
#include "ecs/ecs_registry.hpp"
#include "event/event_manager.hpp"
#include "gameplay/action_manager.hpp"
#include "gameplay/game_token_manager.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "hooks/hook_config.hpp"
#include "hooks/hook_logs.hpp"
#include "hooks/hook_profiler.hpp"
#include "hooks/hook_root_remap.hpp"
#include "hooks/hook_uuid_gen.hpp"
#include "io/enum.hpp"
#include "platform/application.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/canvas.hpp"
#include "render/render_engine.hpp"
#include "render/shader_manager.hpp"
#include "render/texture_manager.hpp"
#include "render/view_manager.hpp"
#include "resource/config_file.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene_manager.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/script_system.hpp"
#include "system/engine.hpp"

#include "event/register_classes_event.hpp"
#include "pfx/register_classes_pfx.hpp"
#include "resource/register_classes_resource.hpp"
#include "scene/register_classes_scene.hpp"

namespace wmoge {

    Status Application::on_register() {
        IocContainer* ioc = IocContainer::instance();

        ioc->bind<HookList>();
        ioc->bind<CmdLine>();

        return StatusCode::Ok;
    }

    int Application::run(int argc, const char* const* argv) {
        IocContainer* ioc = IocContainer::instance();

        on_register();

        CmdLine* cmd_line = ioc->resolve<CmdLine>().value();
        cmd_line->add_bool("h,help", "display help message", "false");

        on_hook();

        HookList* hook_list = ioc->resolve<HookList>().value();
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

        while (!should_close()) {
            WG_AUTO_PROFILE_PLATFORM("Application::iteration");

            if (!on_loop()) {
                return 1;
            }
        }

        signal_after_loop.emit();

        signal_before_shutdown.emit();
        {
            WG_AUTO_PROFILE_PLATFORM("Application::shutdown");

            if (!on_shutdown()) {
                return 1;
            }
        }
        signal_after_shutdown.emit();

        ioc->clear();

        return 0;
    }

    Status BaseApplication::on_register() {
        Application::on_register();

        Class::register_types();
        register_classes_event();
        register_classes_resource();
        register_classes_pfx();
        register_classes_scene();

        IocContainer* ioc = IocContainer::instance();

        ioc->bind<Time>();
        ioc->bind<LayerStack>();
        ioc->bind<ConfigFile>();
        ioc->bind<FileSystem>();
        ioc->bind<Profiler>();
        ioc->bind<Console>();
        ioc->bind<EventManager>();
        ioc->bind<CallbackQueue>();
        ioc->bind<ShaderManager>();
        ioc->bind<TextureManager>();
        ioc->bind<RenderEngine>();
        ioc->bind<ResourceManager>();
        ioc->bind<EcsRegistry>();
        ioc->bind<AuxDrawManager>();
        ioc->bind<SceneManager>();
        ioc->bind<ActionManager>();
        ioc->bind<GameTokenManager>();
        ioc->bind<Canvas>();
        ioc->bind<ViewManager>();

        ioc->bind_f<TaskManager, TaskManager>([]() {
            ConfigFile* config = Engine::instance()->config();

            return std::make_shared<TaskManager>(config->get_int(SID("task_manager.workers"), 4));
        });

        ioc->bind_f<GlfwWindowManager, GlfwWindowManager>([]() {
            ConfigFile* config = Engine::instance()->config();

            const bool vsync      = config->get_bool(SID("gfx.vsync"), true);
            const bool client_api = false;

            return std::make_shared<GlfwWindowManager>(vsync, client_api);
        });

        ioc->bind_f<VKDriver, VKDriver>([]() {
            GlfwWindowManager* window_manager = IocContainer::instance()->resolve<GlfwWindowManager>().value();
            Ref<Window>        window         = window_manager->primary_window();

            VKInitInfo init_info;
            init_info.window       = window;
            init_info.app_name     = window->title();
            init_info.engine_name  = "wmoge";
            init_info.required_ext = window_manager->extensions();
            init_info.factory      = window_manager->factory();

            return std::make_shared<VKDriver>(std::move(init_info));
        });

        Engine* engine = Engine::instance();
        return engine->setup(this);
    }

    Status BaseApplication::on_hook() {
        Application::on_hook();

        IocContainer* ioc = IocContainer::instance();

        HookList* hook_list = ioc->resolve<HookList>().value();

        hook_list->attach(std::make_shared<HookUuidGen>());
        hook_list->attach(std::make_shared<HookRootRemap>());
        hook_list->attach(std::make_shared<HookConfig>());
        hook_list->attach(std::make_shared<HookLogs>());
        hook_list->attach(std::make_shared<HookProfiler>());

        return StatusCode::Ok;
    }

    Status GameApplication::on_init() {
        BaseApplication::on_init();

        Engine* engine = Engine::instance();
        return engine->init();
    }

    Status GameApplication::on_loop() {
        BaseApplication::on_loop();

        Engine* engine = Engine::instance();
        return engine->iteration();
    }

    Status GameApplication::on_shutdown() {
        Engine* engine = Engine::instance();
        engine->shutdown();

        IocContainer* ioc = IocContainer::instance();

        ioc->unbind<ViewManager>();
        ioc->unbind<ActionManager>();
        ioc->unbind<SceneManager>();
        ioc->unbind<ResourceManager>();
        ioc->unbind<ShaderManager>();
        ioc->unbind<TextureManager>();
        ioc->unbind<RenderEngine>();
        ioc->unbind<AuxDrawManager>();
        ioc->unbind<Canvas>();
        ioc->unbind<VKDriver>();
        ioc->unbind<GlfwWindowManager>();
        ioc->unbind<TaskManager>();
        ioc->unbind<EventManager>();

        BaseApplication::on_shutdown();

        return StatusCode::Ok;
    }

    bool GameApplication::should_close() {
        return Engine::instance()->close_requested();
    }

}// namespace wmoge