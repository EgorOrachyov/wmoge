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

#include <cassert>

namespace wmoge {

    void Engine::push_layer(Layer* layer) {
        assert(layer);
        m_layers.push_back(layer);
    }
    void Engine::request_close() {
        m_close_requested.store(true);
    }
    bool Engine::close_requested() const {
        return m_close_requested.load();
    }

    class Application* Engine::application() {
        return m_application;
    }
    CmdLine* Engine::cmd_line() {
        return m_cmd_line;
    }
    ConfigFile* Engine::config() {
        return m_config;
    }
    CallbackQueue* Engine::main_queue() {
        return m_main_queue;
    }
    FileSystem* Engine::file_system() {
        return m_file_system;
    }
    TaskManager* Engine::task_manager() {
        return m_task_manager;
    }
    EventManager* Engine::event_manager() {
        return m_event_manager;
    }
    ResourceManager* Engine::resource_manager() {
        return m_resource_manager;
    }
    WindowManager* Engine::window_manager() {
        return m_window_manager;
    }
    Input* Engine::input() {
        return m_input;
    }
    GfxDriver* Engine::gfx_driver() {
        return m_gfx_driver;
    }
    GfxCtx* Engine::gfx_ctx() {
        return m_gfx_ctx;
    }
    ShaderManager* Engine::shader_manager() {
        return m_shader_manager;
    }
    AuxDrawManager* Engine::aux_draw_manager() {
        return m_aux_draw_manager;
    }
    SceneManager* Engine::scene_manager() {
        return m_scene_manager;
    }
    ActionManager* Engine::action_manager() {
        return m_action_manager;
    }
    GameTokenManager* Engine::game_token_manager() {
        return m_game_token_manager;
    }
    Profiler* Engine::profiler() {
        return m_profiler;
    }
    Console* Engine::console() {
        return m_console;
    }
    AuxDrawCanvas* Engine::canvas_2d_debug() {
        return m_canvas_2d_debug;
    }
    ScriptSystem* Engine::script_system() {
        return m_script_system;
    }
    AudioEngine* Engine::audio_engine() {
        return m_audio_engine;
    }
    class RenderEngine* Engine::render_engine() {
        return m_render_engine;
    }
    class EcsRegistry* Engine::ecs_registry() {
        return m_ecs_registry;
    }

    Engine* Engine::instance() {
        static auto g_engine = std::make_unique<Engine>();
        return g_engine.get();
    }

    std::chrono::steady_clock::time_point Engine::get_start() {
        return m_start;
    }
    std::time_t Engine::get_time() {
        return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    }
    std::tm Engine::get_tm(std::time_t time) {
        std::tm tm;
#if defined(TARGET_WINDOWS)
        localtime_s(&tm, &time);
#else
        localtime_r(&time, &tm);
#endif
        return tm;
    }
    std::string Engine::get_time_formatted(const char* format, std::time_t time) {
        char    buffer[128];
        std::tm tm = get_tm(time);
        strftime(buffer, 128, format, &tm);
        return buffer;
    }
    std::size_t Engine::get_iteration() {
        return m_iteration;
    }
    float Engine::get_time() const {
        return m_time;
    }
    float Engine::get_delta_time() const {
        return m_current_delta;
    }
    float Engine::get_delta_time_game() const {
        return m_current_delta_game;
    }

}// namespace wmoge