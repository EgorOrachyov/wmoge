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

#include "engine.hpp"

#include "asset/asset_manager.hpp"
#include "audio/openal/al_engine.hpp"
#include "core/callback_queue.hpp"
#include "core/class.hpp"
#include "core/cmd_line.hpp"
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "debug/console.hpp"
#include "debug/debug_layer.hpp"
#include "ecs/ecs_registry.hpp"
#include "event/event_manager.hpp"
#include "event/event_window.hpp"
#include "gameplay/action_manager.hpp"
#include "gameplay/game_token_manager.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "hooks/hook_config.hpp"
#include "hooks/hook_logs.hpp"
#include "hooks/hook_profiler.hpp"
#include "hooks/hook_root_remap.hpp"
#include "hooks/hook_uuid_gen.hpp"
#include "io/enum.hpp"
#include "platform/application.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/canvas.hpp"
#include "render/render_engine.hpp"
#include "render/view_manager.hpp"
#include "rtti/type_storage.hpp"
#include "scene/scene_manager.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/script_system.hpp"
#include "system/config_file.hpp"
#include "system/engine.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"
#include "system/layer.hpp"
#include "system/plugin_manager.hpp"

#include <cassert>

namespace wmoge {

    Engine* Engine::g_engine = nullptr;

    Status Engine::setup(Application* application) {
        m_application = application;

        IocContainer* ioc = IocContainer::instance();

        m_class_db       = ClassDB::instance();
        m_type_storage   = ioc->resolve_v<RttiTypeStorage>();
        m_time           = ioc->resolve_v<Time>();
        m_layer_stack    = ioc->resolve_v<LayerStack>();
        m_cmd_line       = ioc->resolve_v<CmdLine>();
        m_hook_list      = ioc->resolve_v<HookList>();
        m_file_system    = ioc->resolve_v<FileSystem>();
        m_config         = ioc->resolve_v<ConfigFile>();
        m_console        = ioc->resolve_v<Console>();
        m_profiler       = ioc->resolve_v<Profiler>();
        m_dll_manager    = ioc->resolve_v<DllManager>();
        m_plugin_manager = ioc->resolve_v<PluginManager>();

        m_plugin_manager->setup();

        return StatusCode::Ok;
    }

    Status Engine::init() {
        WG_AUTO_PROFILE_SYSTEM("Engine::init");

        IocContainer* ioc = IocContainer::instance();

        m_event_manager = ioc->resolve_v<EventManager>();
        m_task_manager  = ioc->resolve_v<TaskManager>();

        m_window_manager = ioc->resolve_v<GlfwWindowManager>();
        m_input          = ioc->resolve_v<GlfwInput>();

        m_asset_manager = ioc->resolve_v<AssetManager>();
        m_asset_manager->load_loaders();

        WindowInfo window_info;
        window_info.width    = m_config->get_int(SID("window.width"), 1280);
        window_info.height   = m_config->get_int(SID("window.height"), 720);
        window_info.title    = m_config->get_string(SID("window.title"), "wmoge");
        window_info.icons[0] = make_ref<Image>();
        window_info.icons[0]->load(m_config->get_string(SID("window.icon_default")), 4);
        window_info.icons[1] = make_ref<Image>();
        window_info.icons[1]->load(m_config->get_string(SID("window.icon_small")), 4);

        auto window = m_window_manager->create(window_info);
        WG_LOG_INFO("init window " << window_info.id);

        m_gfx_driver = ioc->resolve_v<GfxDriver>();
        m_gfx_ctx    = ioc->resolve_v<GfxCtx>();

        m_shader_manager = ioc->resolve_v<ShaderManager>();
        m_shader_manager->load_compilers();

        m_pso_cache        = ioc->resolve_v<PsoCache>();
        m_texture_manager  = ioc->resolve_v<TextureManager>();
        m_render_engine    = ioc->resolve_v<RenderEngine>();
        m_ecs_registry     = ioc->resolve_v<EcsRegistry>();
        m_aux_draw_manager = ioc->resolve_v<AuxDrawManager>();
        m_scene_manager    = ioc->resolve_v<SceneManager>();
        m_action_manager   = ioc->resolve_v<ActionManager>();
        m_canvas_debug     = ioc->resolve_v<Canvas>();
        m_view_manager     = ioc->resolve_v<ViewManager>();

        m_console->init();
        m_layer_stack->attach(std::make_shared<DebugLayer>());

        if (m_config->get_bool(SID("window.exit"), true)) {
            event_manager()->subscribe<EventWindow>([](const EventWindow& event) {
                auto engine = Engine::instance();
                if (event.window == engine->window_manager()->primary_window() &&
                    event.notification == WindowNotification::CloseRequested) {
                    engine->request_close();
                }
                return false;
            });
            WG_LOG_INFO("configure exit on primary window close");
        }

        m_plugin_manager->init();

        return StatusCode::Ok;
    }

    Status Engine::iteration() {
        WG_AUTO_PROFILE_SYSTEM("Engine::iteration");

        m_time->tick();

        auto windows = m_window_manager->windows();

        m_layer_stack->each_up([](LayerStack::LayerPtr& layer) {
            layer->on_start_frame();
        });

        m_gfx_driver->begin_frame();

        for (auto& w : windows) {
            m_gfx_driver->prepare_window(w);
        }

        if (m_scene_manager) {
            m_scene_manager->update();
        }

        m_layer_stack->each_up([](LayerStack::LayerPtr& layer) {
            layer->on_iter();
        });

        m_layer_stack->each_up([](LayerStack::LayerPtr& layer) {
            layer->on_debug_draw();
        });

        m_gfx_driver->end_frame();

        m_layer_stack->each_down([](LayerStack::LayerPtr& layer) {
            layer->on_end_frame();
        });

        m_window_manager->poll_events();

        for (auto& w : windows) {
            m_gfx_driver->swap_buffers(w);
        }

        return StatusCode::Ok;
    }

    Status Engine::shutdown() {
        WG_AUTO_PROFILE_SYSTEM("Engine::shutdown");

        m_plugin_manager->shutdown();
        m_layer_stack->clear();
        m_task_manager->shutdown();
        m_console->shutdown();
        m_scene_manager->clear();
        m_event_manager->flush();

        return StatusCode::Ok;
    }

    void Engine::request_close() {
        m_close_requested.store(true);
    }
    bool Engine::close_requested() const {
        return m_close_requested.load();
    }

    Application*      Engine::application() { return m_application; }
    RttiTypeStorage*  Engine::type_storage() { return m_type_storage; }
    ClassDB*          Engine::class_db() { return m_class_db; }
    Time*             Engine::time() { return m_time; }
    LayerStack*       Engine::layer_stack() { return m_layer_stack; }
    HookList*         Engine::hook_list() { return m_hook_list; }
    CmdLine*          Engine::cmd_line() { return m_cmd_line; }
    DllManager*       Engine::dll_manager() { return m_dll_manager; }
    PluginManager*    Engine::plugin_manager() { return m_plugin_manager; }
    ConfigFile*       Engine::config() { return m_config; }
    CallbackQueue*    Engine::main_queue() { return m_main_queue; }
    FileSystem*       Engine::file_system() { return m_file_system; }
    TaskManager*      Engine::task_manager() { return m_task_manager; }
    EventManager*     Engine::event_manager() { return m_event_manager; }
    AssetManager*     Engine::asset_manager() { return m_asset_manager; }
    WindowManager*    Engine::window_manager() { return m_window_manager; }
    Input*            Engine::input() { return m_input; }
    GfxDriver*        Engine::gfx_driver() { return m_gfx_driver; }
    GfxCtx*           Engine::gfx_ctx() { return m_gfx_ctx; }
    ShaderManager*    Engine::shader_manager() { return m_shader_manager; }
    PsoCache*         Engine::pso_cache() { return m_pso_cache; }
    TextureManager*   Engine::texture_manager() { return m_texture_manager; }
    AuxDrawManager*   Engine::aux_draw_manager() { return m_aux_draw_manager; }
    SceneManager*     Engine::scene_manager() { return m_scene_manager; }
    ActionManager*    Engine::action_manager() { return m_action_manager; }
    GameTokenManager* Engine::game_token_manager() { return m_game_token_manager; }
    Profiler*         Engine::profiler() { return m_profiler; }
    Console*          Engine::console() { return m_console; }
    Canvas*           Engine::canvas_debug() { return m_canvas_debug; }
    ScriptSystem*     Engine::script_system() { return m_script_system; }
    AudioEngine*      Engine::audio_engine() { return m_audio_engine; }
    RenderEngine*     Engine::render_engine() { return m_render_engine; }
    ViewManager*      Engine::view_manager() { return m_view_manager; }
    EcsRegistry*      Engine::ecs_registry() { return m_ecs_registry; }

    Engine* Engine::instance() {
        return g_engine;
    }

    void Engine::provide(Engine* engine) {
        g_engine = engine;
    }

}// namespace wmoge