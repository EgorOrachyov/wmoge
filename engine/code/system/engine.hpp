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
    class Application;
    class Time;
    class DllManager;
    class PluginManager;
    class Config;
    class CfgManager;
    class CallbackQueue;
    class FileSystem;
    class TaskManager;
    class AssetDb;
    class AssetResolver;
    class AssetArtifactCache;
    class AssetManager;
    class WindowManager;
    class Input;
    class GfxDriver;
    class ShaderTable;
    class ShaderManager;
    class ShaderLibrary;
    class PsoCache;
    class TextureManager;
    class MeshManager;
    class SceneManager;
    class AudioEngine;
    class RenderEngine;
    class ViewManager;
    class UiManager;
    class EcsRegistry;
    class ProfilerGpu;
    class GameManager;
    class EngineConfig;
    class EngineSignals;
    class IocContainer;
}// namespace wmoge

namespace wmoge {

    /**
     * @class Engine
     * @brief Global engine state giving access for other systems
     *
     * Engine provides a single point for accessing global systems, allows to
     * bind custom signal handles, provides mechanism to query engine state,
     * simulation state, allows to add custome engine runtime plugins.
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

        Application*        application();
        Time*               time();
        DllManager*         dll_manager();
        PluginManager*      plugin_manager();
        Config*             config();
        CallbackQueue*      main_queue();
        FileSystem*         file_system();
        TaskManager*        task_manager();
        AssetDb*            asset_db();
        AssetArtifactCache* asset_artifact_cache();
        AssetManager*       asset_manager();
        WindowManager*      window_manager();
        Input*              input();
        GfxDriver*          gfx_driver();
        ShaderTable*        shader_table();
        ShaderManager*      shader_manager();
        ShaderLibrary*      shader_library();
        PsoCache*           pso_cache();
        TextureManager*     texture_manager();
        MeshManager*        mesh_manager();
        SceneManager*       scene_manager();
        CfgManager*         cfg_manager();
        AudioEngine*        audio_engine();
        RenderEngine*       render_engine();
        ViewManager*        view_manager();
        UiManager*          ui_manager();
        EcsRegistry*        ecs_registry();
        GameManager*        game_manager();
        EngineConfig*       engine_config();
        EngineSignals*      engine_signals();

    private:
        std::atomic_bool m_close_requested{false};
        std::size_t      m_frame_id = 0;

        Application*        m_application          = nullptr;
        Time*               m_time                 = nullptr;
        DllManager*         m_dll_manager          = nullptr;
        PluginManager*      m_plugin_manager       = nullptr;
        Config*             m_config               = nullptr;
        CallbackQueue*      m_main_queue           = nullptr;
        FileSystem*         m_file_system          = nullptr;
        TaskManager*        m_task_manager         = nullptr;
        AssetDb*            m_asset_db             = nullptr;
        AssetResolver*      m_asset_resolver       = nullptr;
        AssetArtifactCache* m_asset_artifact_cache = nullptr;
        AssetManager*       m_asset_manager        = nullptr;
        WindowManager*      m_window_manager       = nullptr;
        Input*              m_input                = nullptr;
        GfxDriver*          m_gfx_driver           = nullptr;
        ShaderTable*        m_shader_table         = nullptr;
        ShaderManager*      m_shader_manager       = nullptr;
        ShaderLibrary*      m_shader_library       = nullptr;
        PsoCache*           m_pso_cache            = nullptr;
        TextureManager*     m_texture_manager      = nullptr;
        MeshManager*        m_mesh_manager         = nullptr;
        SceneManager*       m_scene_manager        = nullptr;
        CfgManager*         m_cfg_manager          = nullptr;
        AudioEngine*        m_audio_engine         = nullptr;
        RenderEngine*       m_render_engine        = nullptr;
        ViewManager*        m_view_manager         = nullptr;
        UiManager*          m_ui_manager           = nullptr;
        EcsRegistry*        m_ecs_registry         = nullptr;
        ProfilerGpu*        m_profiler_gpu         = nullptr;
        GameManager*        m_game_manager         = nullptr;
        EngineConfig*       m_engine_config        = nullptr;
        EngineSignals*      m_engine_signals       = nullptr;
        IocContainer*       m_ioc_container        = nullptr;

        bool m_exit_on_close = true;
    };

}// namespace wmoge