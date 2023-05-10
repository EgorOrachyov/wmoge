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
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "debug/console.hpp"
#include "debug/debug_layer.hpp"
#include "debug/profiler.hpp"
#include "event/event_manager.hpp"
#include "gameplay/action_manager.hpp"
#include "gameplay/game_token_manager.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "platform/application.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/window_manager.hpp"
#include "render/aux_draw_canvas.hpp"
#include "render/aux_draw_manager.hpp"
#include "render/render_engine.hpp"
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

        m_cmd_line         = std::make_unique<CmdLine>();
        engine->m_cmd_line = m_cmd_line.get();

        m_config_engine         = std::make_unique<ConfigFile>();
        engine->m_config_engine = m_config_engine.get();

        m_file_system         = std::make_unique<FileSystem>();
        engine->m_file_system = m_file_system.get();

        m_console         = std::make_unique<Console>();
        engine->m_console = m_console.get();

        m_profiler         = std::make_unique<Profiler>();
        engine->m_profiler = m_profiler.get();

        WG_LOG_INFO("init essential");
    }

    Main::~Main() = default;

    bool Main::load_config(const std::string& config_path) {
        WG_AUTO_PROFILE_PLATFORM("Main::load_config");

        if (!m_config_engine->load(config_path)) {
            std::cerr << "failed to load default engine config file";
            return false;
        }

        bool     log_to_out        = m_config_engine->get_bool(SID("engine.log_to_out"), true);
        bool     log_to_file       = m_config_engine->get_bool(SID("engine.log_to_file"), true);
        LogLevel log_to_out_level  = magic_enum::enum_cast<LogLevel>(m_config_engine->get_string(SID("engine.log_to_out_level"), "Info")).value();
        LogLevel log_to_file_level = magic_enum::enum_cast<LogLevel>(m_config_engine->get_string(SID("engine.log_to_file_level"), "Info")).value();

        if (log_to_file) {
            Log::instance()->listen(std::make_shared<LogListenerStream>("file", log_to_file_level));
            WG_LOG_INFO("attach file log listener");
        }

        if (log_to_out) {
            Log::instance()->listen(std::make_shared<LogListenerStdout>("out", log_to_out_level));
            WG_LOG_INFO("attach stdout log listener");
            m_console->setup_log(log_to_out_level);
            WG_LOG_INFO("attach console log listener");
        }

        m_profiler->set_enabled(m_config_engine->get_bool(SID("debug.profiler"), false));

        WG_LOG_INFO("init config " << config_path);

        return true;
    }

    bool Main::initialize() {
        WG_AUTO_PROFILE_PLATFORM("Main::initialize");

        auto* engine        = Engine::instance();
        auto* config_engine = engine->config_engine();

        Class::register_types();
        WG_LOG_INFO("init core classes reflection");

        m_event_manager         = std::make_unique<EventManager>();
        engine->m_event_manager = m_event_manager.get();

        m_task_manager         = std::make_unique<TaskManager>(config_engine->get_int(SID("task_manager.workers"), 4));
        engine->m_task_manager = m_task_manager.get();

        m_main_queue         = std::make_unique<CallbackQueue>();
        engine->m_main_queue = m_main_queue.get();

        WG_LOG_INFO("init low level systems");

        engine->application()->on_register();

        WindowInfo window_info;
        window_info.width    = config_engine->get_int(SID("window.width"), 1280);
        window_info.height   = config_engine->get_int(SID("window.height"), 720);
        window_info.title    = config_engine->get_string(SID("window.title"), "wmoge");
        window_info.icons[0] = make_ref<Image>();
        window_info.icons[0]->load(config_engine->get_string(SID("window.icon_default")), 4);
        window_info.icons[1] = make_ref<Image>();
        window_info.icons[1]->load(config_engine->get_string(SID("window.icon_small")), 4);

        bool vsync = config_engine->get_bool(SID("window.vsync"), true);
        bool exit  = config_engine->get_bool(SID("window.exit"), true);

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
        WG_LOG_INFO("init video driver");

        m_al_engine            = std::make_unique<ALAudioEngine>();
        engine->m_audio_engine = m_al_engine.get();
        WG_LOG_INFO("init audio engine");

        m_lua_script_system     = std::make_unique<LuaScriptSystem>();
        engine->m_script_system = m_lua_script_system.get();
        WG_LOG_INFO("init script system");

        m_render_engine         = std::make_unique<RenderEngine>();
        engine->m_render_engine = m_render_engine.get();

        m_resource_manager         = std::make_unique<ResourceManager>();
        engine->m_resource_manager = m_resource_manager.get();

        m_aux_draw_manager         = std::make_unique<AuxDrawManager>();
        engine->m_aux_draw_manager = m_aux_draw_manager.get();

        m_scene_manager         = std::make_unique<SceneManager>();
        engine->m_scene_manager = m_scene_manager.get();

        m_action_manager         = std::make_unique<ActionManager>();
        engine->m_action_manager = m_action_manager.get();

        m_game_token_manager         = std::make_unique<GameTokenManager>();
        engine->m_game_token_manager = m_game_token_manager.get();

        m_canvas_2d_debug         = std::make_unique<AuxDrawCanvas>();
        engine->m_canvas_2d_debug = m_canvas_2d_debug.get();

        m_console->init();

        engine->push_layer(m_scene_manager.get());
        WG_LOG_INFO("init high level systems");

        m_dbg_layer = std::make_unique<DebugLayer>();
        engine->push_layer(m_dbg_layer.get());
        WG_LOG_INFO("init debug layer");

        engine->application()->on_init();

        if (exit) {
            engine->event_manager()->subscribe(make_listener<EventWindow>([](const EventWindow& event) {
                auto engine = Engine::instance();
                if (event.window == engine->window_manager()->primary_window() &&
                    event.notification == WindowNotification::CloseRequested)
                    engine->request_close();
                return false;
            }));

            WG_LOG_INFO("setup exit on primary window close");
        }

        m_time_point = clock::now();

        return true;
    }
    bool Main::iteration() {
        WG_AUTO_PROFILE_PLATFORM("Main::iteration");

        m_num_iterations += 1;

        auto* engine = Engine::instance();
        auto& layers = engine->m_layers;

        auto new_point = clock::now();
        auto t         = float(double(std::chrono::duration_cast<ns>(new_point - m_runtime_time).count()) * 1e-9);
        auto dt        = float(double(std::chrono::duration_cast<ns>(new_point - m_time_point).count()) * 1e-9);
        m_time_point   = new_point;

        engine->m_iteration          = m_num_iterations;
        engine->m_time               = t;
        engine->m_current_delta      = dt;
        engine->m_current_delta_game = Math::min(dt, 1.0f / 20.0f);

        // Frame start
        for (auto it = layers.begin(); it != layers.end(); ++it)
            (*it)->on_start_frame();

        // Flush commands to be executed on main
        m_main_queue->flush();

        // Process events, do it twice since action manager can push new
        m_event_manager->update();
        m_action_manager->update();
        m_event_manager->update();

        // Kick off tasks to process active scene
        // todo

        // Begin new GPU frame only here, since it has costly command buffer
        // allocation, acquiring new window image for presentation, etc.
        engine->m_gfx_driver->begin_frame();
        engine->m_gfx_driver->prepare_window(m_glfw_window_manager->primary_window());

        m_scene_manager->on_update();

        // After flush and before swap we have a lot of time, which will be used
        // for main to sleep. It can be used to do some useful work, such as GC
        m_lua_script_system->update();

        // Debug draw, can be used for debug UI, console, stats, overlay, etc.
        for (auto it = layers.begin(); it != layers.end(); ++it)
            (*it)->on_debug_draw();

        // Render debug canvas
        m_canvas_2d_debug->render();

        // Finish frame, submitting commands
        // after this point no rendering on GPU is allowed
        engine->m_gfx_driver->end_frame();

        // Obtain new events from the operating system
        m_glfw_window_manager->poll_events();

        // Wait for vsync and swap buffers, so main sleep the rest of the frame
        engine->m_gfx_driver->swap_buffers(m_glfw_window_manager->primary_window());

        engine->m_gfx_driver->flush();

        // Frame end
        for (auto it = layers.rbegin(); it != layers.rend(); ++it)
            (*it)->on_end_frame();

        return true;
    }
    bool Main::shutdown() {
        WG_AUTO_PROFILE_PLATFORM("Main::shutdown");

        auto* engine = Engine::instance();

        engine->application()->on_shutdown();

        m_resource_manager->clear();
        m_task_manager->shutdown();
        m_main_queue->flush();
        m_console->shutdown();
        m_scene_manager->shutdown();
        m_event_manager->shutdown();

        m_lua_script_system.reset();
        m_scene_manager.reset();
        m_game_token_manager.reset();
        m_action_manager.reset();
        m_aux_draw_manager.reset();
        m_canvas_2d_debug.reset();
        m_render_engine.reset();
        m_al_engine.reset();
        m_vk_driver.reset();
        m_glfw_window_manager.reset();
        m_main_queue.reset();
        m_task_manager.reset();
        m_event_manager.reset();
        m_resource_manager.reset();
        m_console.reset();

        WG_LOG_INFO("shutdown engine systems");

        return true;
    }

}// namespace wmoge