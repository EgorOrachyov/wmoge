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

#include "asset/asset_library_fs.hpp"
#include "asset/asset_manager.hpp"
#include "audio/openal/al_engine.hpp"
#include "core/callback_queue.hpp"
#include "core/cmd_line.hpp"
#include "core/ioc_container.hpp"
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "ecs/ecs_registry.hpp"
#include "game/game_manager.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "glsl/glsl_shader_compiler.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "io/config.hpp"
#include "io/enum.hpp"
#include "mesh/mesh_manager.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"
#include "render/render_engine.hpp"
#include "render/shader_table.hpp"
#include "render/view_manager.hpp"
#include "rtti/type_storage.hpp"
#include "scene/scene_manager.hpp"
#include "system/app.hpp"
#include "system/engine.hpp"
#include "system/engine_config.hpp"
#include "system/engine_signals.hpp"
#include "system/plugin_manager.hpp"
#include "ui/ui_manager.hpp"

#include <cassert>

namespace wmoge {

    Engine::Engine(IocContainer* ioc_container) {
        m_ioc_container = ioc_container;
    }

    Status Engine::setup() {
        m_application = m_ioc_container->resolve_value<Application>();
        m_time        = m_ioc_container->resolve_value<Time>();
        m_file_system = m_ioc_container->resolve_value<FileSystem>();
        m_config      = m_ioc_container->resolve_value<Config>();
        // m_console        = m_ioc_container->resolve_value<Console>();
        m_dll_manager    = m_ioc_container->resolve_value<DllManager>();
        m_plugin_manager = m_ioc_container->resolve_value<PluginManager>();
        m_engine_config  = m_ioc_container->resolve_value<EngineConfig>();
        m_engine_signals = m_ioc_container->resolve_value<EngineSignals>();

        m_engine_signals->setup.emit();
        m_plugin_manager->setup(m_ioc_container);

        return WG_OK;
    }

    Status Engine::init() {
        WG_PROFILE_CPU_SYSTEM("Engine::init");

        m_task_manager  = m_ioc_container->resolve_value<TaskManager>();
        m_asset_manager = m_ioc_container->resolve_value<AssetManager>();

        m_window_manager = m_ioc_container->resolve_value<GlfwWindowManager>();
        m_input          = m_ioc_container->resolve_value<GlfwInput>();

        WindowInfo window_info;
        window_info.width    = m_config->get_int_or_default(SID("engine.window.width"), 1280);
        window_info.height   = m_config->get_int_or_default(SID("engine.window.height"), 720);
        window_info.title    = m_config->get_string_or_default(SID("engine.window.title"), "wmoge");
        window_info.icons[0] = make_ref<Image>();
        window_info.icons[0]->load(m_file_system, m_config->get_string_or_default(SID("engine.window.icon_default")), 4);
        window_info.icons[1] = make_ref<Image>();
        window_info.icons[1]->load(m_file_system, m_config->get_string_or_default(SID("engine.window.icon_small")), 4);

        auto window = m_window_manager->create_window(window_info);
        WG_LOG_INFO("init window " << window_info.id);

        m_gfx_driver = m_ioc_container->resolve_value<GfxDriver>();

        m_profiler_gpu = ProfilerGpu::instance();
        m_profiler_gpu->setup(m_gfx_driver);
        m_profiler_gpu->enable(true);
        m_profiler_gpu->calibrate(m_time->get_start());

        m_asset_manager->load_loaders();
        m_asset_manager->add_library(std::make_shared<AssetLibraryFileSystem>("", m_ioc_container));

        m_shader_library  = m_ioc_container->resolve_value<ShaderLibrary>();
        m_pso_cache       = m_ioc_container->resolve_value<PsoCache>();
        m_texture_manager = m_ioc_container->resolve_value<TextureManager>();
        m_mesh_manager    = m_ioc_container->resolve_value<MeshManager>();
        m_render_engine   = m_ioc_container->resolve_value<RenderEngine>();
        m_ecs_registry    = m_ioc_container->resolve_value<EcsRegistry>();
        m_scene_manager   = m_ioc_container->resolve_value<SceneManager>();
        m_view_manager    = m_ioc_container->resolve_value<ViewManager>();
        m_ui_manager      = m_ioc_container->resolve_value<UiManager>();
        m_game_manager    = m_ioc_container->resolve_value<GameManager>();

        m_shader_manager = m_ioc_container->resolve_value<ShaderManager>();
        m_shader_manager->add_compiler(make_ref<GlslShaderCompilerVulkanMacOS>(m_ioc_container));
        m_shader_manager->add_compiler(make_ref<GlslShaderCompilerVulkanLinux>(m_ioc_container));
        m_shader_manager->add_compiler(make_ref<GlslShaderCompilerVulkanWindows>(m_ioc_container));

        m_shader_table = m_ioc_container->resolve_value<ShaderTable>();
        WG_CHECKED(m_shader_table->reflect_types(m_shader_manager));
        WG_CHECKED(m_shader_table->load_shaders(m_asset_manager));

        // m_console->init(m_asset_manager);

        m_config->get_bool(SID("engine.window.exit"), m_exit_on_close);

        m_engine_signals->init.emit();
        m_plugin_manager->init();

        return WG_OK;
    }

    Status Engine::iteration() {
        WG_PROFILE_CPU_SYSTEM("Engine::iteration");

        m_time->tick();
        m_frame_id = m_time->get_iteration();

        auto windows = m_window_manager->get_windows();

        if (m_exit_on_close) {
            for (const WindowEvent& event : m_window_manager->get_window_events()) {
                if (event.window == m_window_manager->get_primary_window() &&
                    event.notification == WindowNotification::CloseRequested) {
                    request_close();
                }
            }
        }

        m_gfx_driver->begin_frame(m_frame_id, windows);
        m_engine_signals->begin_frame.emit();

        m_texture_manager->flust_textures_upload();
        m_mesh_manager->flush_meshes_upload();

        m_engine_signals->debug_draw.emit();

        // auto cmd_list = m_gfx_driver->acquire_cmd_list();
        // m_ui_manager->update(m_frame_id);
        // m_ui_manager->render(cmd_list);
        // m_gfx_driver->submit_cmd_list(cmd_list);
        // cmd_list.reset();

        m_profiler_gpu->resolve();

        m_window_manager->poll_events();

        m_engine_signals->end_frame.emit();
        m_gfx_driver->end_frame(true);

        return WG_OK;
    }

    Status Engine::shutdown() {
        WG_PROFILE_CPU_SYSTEM("Engine::shutdown");

        m_gfx_driver->wait_idle();

        m_engine_signals->shutdown.emit();
        m_plugin_manager->shutdown();
        m_task_manager->shutdown();
        // m_console->shutdown();
        m_scene_manager->clear();
        m_profiler_gpu->clear();

        return WG_OK;
    }

    void Engine::request_close() {
        m_close_requested.store(true);
    }
    bool Engine::close_requested() const {
        return m_close_requested.load();
    }

    Application*    Engine::application() { return m_application; }
    Time*           Engine::time() { return m_time; }
    DllManager*     Engine::dll_manager() { return m_dll_manager; }
    PluginManager*  Engine::plugin_manager() { return m_plugin_manager; }
    Config*         Engine::config() { return m_config; }
    CallbackQueue*  Engine::main_queue() { return m_main_queue; }
    FileSystem*     Engine::file_system() { return m_file_system; }
    TaskManager*    Engine::task_manager() { return m_task_manager; }
    AssetManager*   Engine::asset_manager() { return m_asset_manager; }
    WindowManager*  Engine::window_manager() { return m_window_manager; }
    Input*          Engine::input() { return m_input; }
    GfxDriver*      Engine::gfx_driver() { return m_gfx_driver; }
    ShaderTable*    Engine::shader_table() { return m_shader_table; }
    ShaderManager*  Engine::shader_manager() { return m_shader_manager; }
    ShaderLibrary*  Engine::shader_library() { return m_shader_library; }
    PsoCache*       Engine::pso_cache() { return m_pso_cache; }
    TextureManager* Engine::texture_manager() { return m_texture_manager; }
    MeshManager*    Engine::mesh_manager() { return m_mesh_manager; }
    SceneManager*   Engine::scene_manager() { return m_scene_manager; }
    Console*        Engine::console() { return m_console; }
    AudioEngine*    Engine::audio_engine() { return m_audio_engine; }
    RenderEngine*   Engine::render_engine() { return m_render_engine; }
    ViewManager*    Engine::view_manager() { return m_view_manager; }
    UiManager*      Engine::ui_manager() { return m_ui_manager; }
    EcsRegistry*    Engine::ecs_registry() { return m_ecs_registry; }
    GameManager*    Engine::game_manager() { return m_game_manager; }
    EngineConfig*   Engine::engine_config() { return m_engine_config; }
    EngineSignals*  Engine::engine_signals() { return m_engine_signals; }

}// namespace wmoge