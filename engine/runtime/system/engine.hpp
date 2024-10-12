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
        Engine(class IocContainer* ioc_container);

        Status setup();
        Status init();
        Status iteration();
        Status shutdown();

        void request_close();
        bool close_requested() const;

        class Application*    application();
        class ClassDB*        class_db();
        class Time*           time();
        class DllManager*     dll_manager();
        class PluginManager*  plugin_manager();
        class Config*         config();
        class CallbackQueue*  main_queue();
        class FileSystem*     file_system();
        class TaskManager*    task_manager();
        class AssetManager*   asset_manager();
        class WindowManager*  window_manager();
        class Input*          input();
        class GfxDriver*      gfx_driver();
        class ShaderManager*  shader_manager();
        class ShaderLibrary*  shader_library();
        class PsoCache*       pso_cache();
        class TextureManager* texture_manager();
        class MeshManager*    mesh_manager();
        class SceneManager*   scene_manager();
        class Console*        console();
        class AudioEngine*    audio_engine();
        class RenderEngine*   render_engine();
        class ViewManager*    view_manager();
        class EcsRegistry*    ecs_registry();
        class EngineConfig*   engine_config();

    private:
        std::atomic_bool m_close_requested{false};
        std::size_t      m_frame_id = 0;

        class Application*    m_application     = nullptr;
        class ClassDB*        m_class_db        = nullptr;
        class Time*           m_time            = nullptr;
        class DllManager*     m_dll_manager     = nullptr;
        class PluginManager*  m_plugin_manager  = nullptr;
        class Config*         m_config          = nullptr;
        class CallbackQueue*  m_main_queue      = nullptr;
        class FileSystem*     m_file_system     = nullptr;
        class TaskManager*    m_task_manager    = nullptr;
        class AssetManager*   m_asset_manager   = nullptr;
        class WindowManager*  m_window_manager  = nullptr;
        class Input*          m_input           = nullptr;
        class GfxDriver*      m_gfx_driver      = nullptr;
        class ShaderManager*  m_shader_manager  = nullptr;
        class ShaderLibrary*  m_shader_library  = nullptr;
        class PsoCache*       m_pso_cache       = nullptr;
        class TextureManager* m_texture_manager = nullptr;
        class MeshManager*    m_mesh_manager    = nullptr;
        class SceneManager*   m_scene_manager   = nullptr;
        class Console*        m_console         = nullptr;
        class AudioEngine*    m_audio_engine    = nullptr;
        class RenderEngine*   m_render_engine   = nullptr;
        class ViewManager*    m_view_manager    = nullptr;
        class EcsRegistry*    m_ecs_registry    = nullptr;
        class IocContainer*   m_ioc_container   = nullptr;
        class ProfilerGpu*    m_profiler_gpu    = nullptr;
        class EngineConfig*   m_engine_config   = nullptr;

        bool m_exit_on_close = true;
    };

}// namespace wmoge