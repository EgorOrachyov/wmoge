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
#include "core/class.hpp"
#include "core/cmd_line.hpp"
#include "core/log.hpp"
#include "core/task_manager.hpp"
#include "ecs/ecs_registry.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "grc/pso_cache.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "hooks/hook_config.hpp"
#include "hooks/hook_logs.hpp"
#include "hooks/hook_profiler.hpp"
#include "hooks/hook_root_remap.hpp"
#include "hooks/hook_uuid_gen.hpp"
#include "io/enum.hpp"
#include "mesh/mesh_manager.hpp"
#include "platform/application.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler.hpp"
#include "render/render_engine.hpp"
#include "render/view_manager.hpp"
#include "rtti/type_storage.hpp"
#include "scene/scene_manager.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/script_system.hpp"
#include "system/config.hpp"
#include "system/console.hpp"
#include "system/engine.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"
#include "system/plugin_manager.hpp"

#include <cassert>

namespace wmoge {

    Engine* Engine::g_engine = nullptr;

    Status Engine::setup(Application* application) {
        m_application = application;

        IocContainer* ioc = IocContainer::instance();

        m_class_db       = ClassDB::instance();
        m_type_storage   = ioc->resolve_value<RttiTypeStorage>();
        m_time           = ioc->resolve_value<Time>();
        m_cmd_line       = ioc->resolve_value<CmdLine>();
        m_hook_list      = ioc->resolve_value<HookList>();
        m_file_system    = ioc->resolve_value<FileSystem>();
        m_config         = ioc->resolve_value<Config>();
        m_console        = ioc->resolve_value<Console>();
        m_profiler       = ioc->resolve_value<Profiler>();
        m_dll_manager    = ioc->resolve_value<DllManager>();
        m_plugin_manager = ioc->resolve_value<PluginManager>();

        m_plugin_manager->setup();

        return WG_OK;
    }

    Status Engine::init() {
        WG_AUTO_PROFILE_SYSTEM("Engine::init");

        IocContainer* ioc = IocContainer::instance();

        m_task_manager  = ioc->resolve_value<TaskManager>();
        m_asset_manager = ioc->resolve_value<AssetManager>();

        m_window_manager = ioc->resolve_value<GlfwWindowManager>();
        m_input          = ioc->resolve_value<GlfwInput>();

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

        m_gfx_driver = ioc->resolve_value<GfxDriver>();

        m_shader_manager = ioc->resolve_value<ShaderManager>();
        m_shader_manager->load_compilers();

        m_asset_manager->load_loaders();
        m_asset_manager->add_library(std::make_shared<AssetLibraryFileSystem>("", ioc));

        m_shader_library  = ioc->resolve_value<ShaderLibrary>();
        m_pso_cache       = ioc->resolve_value<PsoCache>();
        m_texture_manager = ioc->resolve_value<TextureManager>();
        m_mesh_manager    = ioc->resolve_value<MeshManager>();
        m_render_engine   = ioc->resolve_value<RenderEngine>();
        m_ecs_registry    = ioc->resolve_value<EcsRegistry>();
        m_scene_manager   = ioc->resolve_value<SceneManager>();
        m_view_manager    = ioc->resolve_value<ViewManager>();

        m_console->init();

        m_config->get_bool(SID("engine.window.exit"), m_exit_on_close);

        m_plugin_manager->init();

        return WG_OK;
    }

    Status Engine::iteration() {
        WG_AUTO_PROFILE_SYSTEM("Engine::iteration");

        m_time->tick();
        m_frame_id = m_time->get_iteration();

        auto windows = m_window_manager->get_windows();

        for (const WindowEvent& event : m_window_manager->get_window_events()) {
            if (m_exit_on_close &&
                event.window == m_window_manager->get_primary_window() &&
                event.notification == WindowNotification::CloseRequested) {
                request_close();
            }
        }

        m_gfx_driver->begin_frame(m_frame_id, windows);

        if (m_texture_manager) {
            m_texture_manager->update();
        }

        if (m_scene_manager) {
            m_scene_manager->update();
        }

        m_window_manager->poll_events();

        m_gfx_driver->end_frame(true);

        return WG_OK;
    }

    Status Engine::shutdown() {
        WG_AUTO_PROFILE_SYSTEM("Engine::shutdown");

        m_plugin_manager->shutdown();
        m_task_manager->shutdown();
        m_console->shutdown();
        m_scene_manager->clear();

        return WG_OK;
    }

    void Engine::request_close() {
        m_close_requested.store(true);
    }
    bool Engine::close_requested() const {
        return m_close_requested.load();
    }

    Application*     Engine::application() { return m_application; }
    RttiTypeStorage* Engine::type_storage() { return m_type_storage; }
    ClassDB*         Engine::class_db() { return m_class_db; }
    Time*            Engine::time() { return m_time; }
    HookList*        Engine::hook_list() { return m_hook_list; }
    CmdLine*         Engine::cmd_line() { return m_cmd_line; }
    DllManager*      Engine::dll_manager() { return m_dll_manager; }
    PluginManager*   Engine::plugin_manager() { return m_plugin_manager; }
    Config*          Engine::config() { return m_config; }
    CallbackQueue*   Engine::main_queue() { return m_main_queue; }
    FileSystem*      Engine::file_system() { return m_file_system; }
    TaskManager*     Engine::task_manager() { return m_task_manager; }
    AssetManager*    Engine::asset_manager() { return m_asset_manager; }
    WindowManager*   Engine::window_manager() { return m_window_manager; }
    Input*           Engine::input() { return m_input; }
    GfxDriver*       Engine::gfx_driver() { return m_gfx_driver; }
    ShaderManager*   Engine::shader_manager() { return m_shader_manager; }
    ShaderLibrary*   Engine::shader_library() { return m_shader_library; }
    PsoCache*        Engine::pso_cache() { return m_pso_cache; }
    TextureManager*  Engine::texture_manager() { return m_texture_manager; }
    MeshManager*     Engine::mesh_manager() { return m_mesh_manager; }
    SceneManager*    Engine::scene_manager() { return m_scene_manager; }
    Profiler*        Engine::profiler() { return m_profiler; }
    Console*         Engine::console() { return m_console; }
    ScriptSystem*    Engine::script_system() { return m_script_system; }
    AudioEngine*     Engine::audio_engine() { return m_audio_engine; }
    RenderEngine*    Engine::render_engine() { return m_render_engine; }
    ViewManager*     Engine::view_manager() { return m_view_manager; }
    EcsRegistry*     Engine::ecs_registry() { return m_ecs_registry; }

    Engine* Engine::instance() {
        return g_engine;
    }

    void Engine::provide(Engine* engine) {
        g_engine = engine;
    }

}// namespace wmoge