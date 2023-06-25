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

#ifndef WMOGE_ENGINE_HPP
#define WMOGE_ENGINE_HPP

#include <atomic>
#include <ctime>
#include <filesystem>
#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class Engine
     * @brief Global engine state giving access for other systems
     *
     * Engine provides a single point for accessing global systems, allows to
     * push additional user layers to get updated, provides mechanism to query
     * global, update and game time.
     */
    class Engine {
    public:
        void push_layer(class Layer* layer);

        void request_close();
        bool close_requested() const;

        class Application*      application();
        class CmdLine*          cmd_line();
        class ConfigFile*       config_engine();
        class CallbackQueue*    main_queue();
        class FileSystem*       file_system();
        class TaskManager*      task_manager();
        class EventManager*     event_manager();
        class ResourceManager*  resource_manager();
        class WindowManager*    window_manager();
        class Input*            input();
        class GfxDriver*        gfx_driver();
        class GfxCtx*           gfx_ctx();
        class ShaderManager*    shader_manager();
        class AuxDrawManager*   aux_draw_manager();
        class SceneManager*     scene_manager();
        class ActionManager*    action_manager();
        class GameTokenManager* game_token_manager();
        class Profiler*         profiler();
        class Console*          console();
        class AuxDrawCanvas*    canvas_2d_debug();
        class ScriptSystem*     script_system();
        class AudioEngine*      audio_engine();
        class RenderEngine*     render_engine();
        class EcsRegistry*      ecs_registry();

        std::chrono::steady_clock::time_point get_start();
        std::time_t                           get_time();
        std::tm                               get_tm(std::time_t time);
        std::string                           get_time_formatted(const char* format, std::time_t time);
        std::size_t                           get_iteration();
        float                                 get_time() const;
        float                                 get_delta_time() const;
        float                                 get_delta_time_game() const;

        static Engine* instance();

    private:
        friend class Main;

        std::chrono::steady_clock::time_point m_start = std::chrono::steady_clock::now();
        std::atomic_bool                      m_close_requested{false};
        std::size_t                           m_iteration          = 0;
        float                                 m_time               = 0;
        float                                 m_current_delta      = 0;
        float                                 m_current_delta_game = 0;

        class Application*      m_application        = nullptr;
        class CmdLine*          m_cmd_line           = nullptr;
        class ConfigFile*       m_config_engine      = nullptr;
        class CallbackQueue*    m_main_queue         = nullptr;
        class FileSystem*       m_file_system        = nullptr;
        class TaskManager*      m_task_manager       = nullptr;
        class EventManager*     m_event_manager      = nullptr;
        class ResourceManager*  m_resource_manager   = nullptr;
        class WindowManager*    m_window_manager     = nullptr;
        class Input*            m_input              = nullptr;
        class GfxDriver*        m_gfx_driver         = nullptr;
        class GfxCtx*           m_gfx_ctx            = nullptr;
        class ShaderManager*    m_shader_manager     = nullptr;
        class AuxDrawManager*   m_aux_draw_manager   = nullptr;
        class SceneManager*     m_scene_manager      = nullptr;
        class ActionManager*    m_action_manager     = nullptr;
        class GameTokenManager* m_game_token_manager = nullptr;
        class Profiler*         m_profiler           = nullptr;
        class Console*          m_console            = nullptr;
        class AuxDrawCanvas*    m_canvas_2d_debug    = nullptr;
        class ScriptSystem*     m_script_system      = nullptr;
        class AudioEngine*      m_audio_engine       = nullptr;
        class RenderEngine*     m_render_engine      = nullptr;
        class EcsRegistry*      m_ecs_registry       = nullptr;

        std::vector<class Layer*> m_layers;
    };

}// namespace wmoge

#endif//WMOGE_ENGINE_HPP
