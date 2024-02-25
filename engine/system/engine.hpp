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

#include "core/status.hpp"

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
        Engine() = default;

        Status setup(class Application* application);
        Status init();
        Status iteration();
        Status shutdown();

        void request_close();
        bool close_requested() const;

        class Application*      application();
        class ClassDB*          class_db();
        class Time*             time();
        class LayerStack*       layer_stack();
        class HookList*         hook_list();
        class CmdLine*          cmd_line();
        class ConfigFile*       config();
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
        class TextureManager*   texture_manager();
        class AuxDrawManager*   aux_draw_manager();
        class SceneManager*     scene_manager();
        class ActionManager*    action_manager();
        class GameTokenManager* game_token_manager();
        class Profiler*         profiler();
        class Console*          console();
        class Canvas*           canvas_debug();
        class ScriptSystem*     script_system();
        class AudioEngine*      audio_engine();
        class RenderEngine*     render_engine();
        class EcsRegistry*      ecs_registry();

        static Engine* instance();

    private:
        std::atomic_bool m_close_requested{false};

        class Application*      m_application        = nullptr;
        class ClassDB*          m_class_db           = nullptr;
        class Time*             m_time               = nullptr;
        class LayerStack*       m_layer_stack        = nullptr;
        class HookList*         m_hook_list          = nullptr;
        class CmdLine*          m_cmd_line           = nullptr;
        class ConfigFile*       m_config             = nullptr;
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
        class TextureManager*   m_texture_manager    = nullptr;
        class AuxDrawManager*   m_aux_draw_manager   = nullptr;
        class SceneManager*     m_scene_manager      = nullptr;
        class ActionManager*    m_action_manager     = nullptr;
        class GameTokenManager* m_game_token_manager = nullptr;
        class Profiler*         m_profiler           = nullptr;
        class Console*          m_console            = nullptr;
        class Canvas*           m_canvas_debug       = nullptr;
        class ScriptSystem*     m_script_system      = nullptr;
        class AudioEngine*      m_audio_engine       = nullptr;
        class RenderEngine*     m_render_engine      = nullptr;
        class EcsRegistry*      m_ecs_registry       = nullptr;
    };

}// namespace wmoge