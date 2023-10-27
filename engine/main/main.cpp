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

#include "main.hpp"

#include "audio/openal/al_engine.hpp"
#include "core/callback_queue.hpp"
#include "core/class.hpp"
#include "core/cmd_line.hpp"
#include "core/engine.hpp"
#include "core/hook.hpp"
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
#include "io/enum.hpp"
#include "platform/application.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/window_manager.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/canvas.hpp"
#include "render/render_engine.hpp"
#include "render/shader_manager.hpp"
#include "render/texture_manager.hpp"
#include "resource/config_file.hpp"
#include "resource/resource_manager.hpp"
#include "scene/scene_manager.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/script_system.hpp"

#include <magic_enum.hpp>

namespace wmoge {

    Main::Main(Application* application) {
        auto* engine = Engine::instance();

        engine->m_application = application;

        m_layer_stack         = std::make_unique<LayerStack>();
        engine->m_layer_stack = m_layer_stack.get();

        m_hook_list         = std::make_unique<HookList>();
        engine->m_hook_list = m_hook_list.get();

        m_cmd_line         = std::make_unique<CmdLine>();
        engine->m_cmd_line = m_cmd_line.get();

        m_config         = std::make_unique<ConfigFile>();
        engine->m_config = m_config.get();

        m_file_system         = std::make_unique<FileSystem>();
        engine->m_file_system = m_file_system.get();

        m_console         = std::make_unique<Console>();
        engine->m_console = m_console.get();

        m_profiler         = std::make_unique<Profiler>();
        engine->m_profiler = m_profiler.get();

        WG_LOG_INFO("init essential");
    }

    Main::~Main() = default;

    bool Main::initialize() {
        WG_AUTO_PROFILE_PLATFORM("Main::initialize");

        auto* engine = Engine::instance();
        auto* config = engine->config();

        Class::register_types();
        WG_LOG_INFO("init core classes reflection");

        m_event_manager         = std::make_unique<EventManager>();
        engine->m_event_manager = m_event_manager.get();
        WG_LOG_INFO("init event manager");

        m_task_manager         = std::make_unique<TaskManager>(config->get_int(SID("task_manager.workers"), 4));
        engine->m_task_manager = m_task_manager.get();
        WG_LOG_INFO("init task manager");

        m_main_queue         = std::make_unique<CallbackQueue>();
        engine->m_main_queue = m_main_queue.get();
        WG_LOG_INFO("init main queue");

        engine->application()->on_register();

        WindowInfo window_info;
        window_info.width    = config->get_int(SID("window.width"), 1280);
        window_info.height   = config->get_int(SID("window.height"), 720);
        window_info.title    = config->get_string(SID("window.title"), "wmoge");
        window_info.icons[0] = make_ref<Image>();
        window_info.icons[0]->load(config->get_string(SID("window.icon_default")), 4);
        window_info.icons[1] = make_ref<Image>();
        window_info.icons[1]->load(config->get_string(SID("window.icon_small")), 4);

        bool vsync = config->get_bool(SID("window.vsync"), true);
        bool exit  = config->get_bool(SID("window.exit"), true);

        m_glfw_window_manager    = std::make_unique<GlfwWindowManager>(vsync, false);
        engine->m_window_manager = m_glfw_window_manager.get();
        engine->m_input          = m_glfw_window_manager->input().get();
        WG_LOG_INFO("init window system");

        auto window = m_glfw_window_manager->create(window_info);
        WG_LOG_INFO("init window " << window_info.id);

        VKInitInfo init_info;
        init_info.window       = window;
        init_info.app_name     = window_info.title;
        init_info.engine_name  = "wmoge";
        init_info.required_ext = m_glfw_window_manager->extensions();
        init_info.factory      = m_glfw_window_manager->factory();

        m_vk_driver          = std::make_unique<VKDriver>(std::move(init_info));
        engine->m_gfx_driver = m_vk_driver->driver_wrapper();
        engine->m_gfx_ctx    = m_vk_driver->ctx_immediate_wrapper();

        m_al_engine            = std::make_unique<ALAudioEngine>();
        engine->m_audio_engine = m_al_engine.get();

        m_lua_script_system     = std::make_unique<LuaScriptSystem>();
        engine->m_script_system = m_lua_script_system.get();

        m_shader_manager         = std::make_unique<ShaderManager>();
        engine->m_shader_manager = m_shader_manager.get();

        m_texture_manager         = std::make_unique<TextureManager>();
        engine->m_texture_manager = m_texture_manager.get();

        m_render_engine         = std::make_unique<RenderEngine>();
        engine->m_render_engine = m_render_engine.get();

        m_resource_manager         = std::make_unique<ResourceManager>();
        engine->m_resource_manager = m_resource_manager.get();

        m_ecs_registry         = std::make_unique<EcsRegistry>();
        engine->m_ecs_registry = m_ecs_registry.get();

        m_aux_draw_manager         = std::make_unique<AuxDrawManager>();
        engine->m_aux_draw_manager = m_aux_draw_manager.get();

        m_scene_manager         = std::make_unique<SceneManager>();
        engine->m_scene_manager = m_scene_manager.get();

        m_action_manager         = std::make_unique<ActionManager>();
        engine->m_action_manager = m_action_manager.get();

        m_game_token_manager         = std::make_unique<GameTokenManager>();
        engine->m_game_token_manager = m_game_token_manager.get();

        m_canvas_debug         = std::make_unique<Canvas>();
        engine->m_canvas_debug = m_canvas_debug.get();

        m_console->init();
        WG_LOG_INFO("init high level systems");

        m_dbg_layer = std::make_shared<DebugLayer>();
        m_layer_stack->attach(m_dbg_layer);
        WG_LOG_INFO("init debug layer");

        engine->application()->on_init();

        if (exit) {
            engine->event_manager()->subscribe<EventWindow>([](const EventWindow& event) {
                auto engine = Engine::instance();
                if (event.window == engine->window_manager()->primary_window() &&
                    event.notification == WindowNotification::CloseRequested)
                    engine->request_close();
                return false;
            });

            WG_LOG_INFO("configure exit on primary window close");
        }

        m_time_point = clock::now();

        return true;
    }
    bool Main::iteration() {
        WG_AUTO_PROFILE_PLATFORM("Main::iteration");

        m_num_iterations += 1;

        auto* engine     = Engine::instance();
        auto* gfx_driver = engine->gfx_driver();

        auto new_point = clock::now();
        auto t         = float(double(std::chrono::duration_cast<ns>(new_point - m_runtime_time).count()) * 1e-9);
        auto dt        = float(double(std::chrono::duration_cast<ns>(new_point - m_time_point).count()) * 1e-9);
        m_time_point   = new_point;

        engine->m_iteration          = m_num_iterations;
        engine->m_time               = t;
        engine->m_current_delta      = dt;
        engine->m_current_delta_game = Math::min(dt, 1.0f / 20.0f);

        m_layer_stack->each_up([](LayerStack::LayerPtr& layer) {
            layer->on_start_frame();
        });

        gfx_driver->begin_frame();
        gfx_driver->prepare_window(m_glfw_window_manager->primary_window());

        m_glfw_window_manager->poll_events();
        m_main_queue->flush();
        m_event_manager->flush();
        m_action_manager->update();
        m_scene_manager->update();

        m_layer_stack->each_up([](LayerStack::LayerPtr& layer) {
            layer->on_debug_draw();
        });

        gfx_driver->end_frame();

        m_layer_stack->each_down([](LayerStack::LayerPtr& layer) {
            layer->on_end_frame();
        });

        gfx_driver->swap_buffers(m_glfw_window_manager->primary_window());

        return true;
    }
    bool Main::shutdown() {
        WG_AUTO_PROFILE_PLATFORM("Main::shutdown");

        auto* engine = Engine::instance();

        engine->application()->on_shutdown();

        m_layer_stack->clear();
        m_resource_manager->clear();
        m_task_manager->shutdown();
        m_main_queue->flush();
        m_console->shutdown();
        m_scene_manager->clear();
        m_event_manager->flush();

        m_lua_script_system.reset();
        m_scene_manager.reset();
        m_game_token_manager.reset();
        m_action_manager.reset();
        m_aux_draw_manager.reset();
        m_canvas_debug.reset();
        m_ecs_registry.reset();
        m_render_engine.reset();
        m_texture_manager.reset();
        m_shader_manager.reset();
        m_al_engine.reset();
        m_vk_driver.reset();
        m_glfw_window_manager.reset();
        m_main_queue.reset();
        m_task_manager.reset();
        m_event_manager.reset();
        m_resource_manager.reset();
        m_hook_list.reset();

        WG_LOG_INFO("shutdown engine systems");

        return true;
    }

}// namespace wmoge