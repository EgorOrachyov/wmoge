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

#include "engine_app.hpp"

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
#include "grc/shader_compiler.hpp"
#include "grc/shader_library.hpp"
#include "grc/shader_manager.hpp"
#include "grc/texture_manager.hpp"
#include "io/async_file_system.hpp"
#include "io/config.hpp"
#include "io/enum.hpp"
#include "mesh/mesh_manager.hpp"
#include "platform/dll_manager.hpp"
#include "platform/file_system.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "platform/input.hpp"
#include "platform/time.hpp"
#include "platform/window_manager.hpp"
#include "profiler/profiler_capture.hpp"
#include "profiler/profiler_cpu.hpp"
#include "render/render_engine.hpp"
#include "render/shader_table.hpp"
#include "render/view_manager.hpp"
#include "rtti/type_storage.hpp"
#include "scene/scene_manager.hpp"
#include "system/engine.hpp"
#include "system/engine_config.hpp"
#include "system/engine_signals.hpp"
#include "system/plugin_manager.hpp"
#include "ui/ui_manager.hpp"

#include "asset/_rtti.hpp"
#include "audio/_rtti.hpp"
#include "game/_rtti.hpp"
#include "glsl/_rtti.hpp"
#include "grc/_rtti.hpp"
#include "material/_rtti.hpp"
#include "mesh/_rtti.hpp"
#include "pfx/_rtti.hpp"
#include "render/_rtti.hpp"
#include "rtti/_rtti.hpp"
#include "scene/_rtti.hpp"
#include "system/_rtti.hpp"
#include "ui/_rtti.hpp"

namespace wmoge {

    static void bind_globals(IocContainer* ioc) {
        ioc->bind_by_pointer<Log>(Log::instance());
        ioc->bind_by_pointer<RttiTypeStorage>(RttiTypeStorage::instance());
        ioc->bind<PluginManager>();
        ioc->bind<Time>();
        ioc->bind<FileSystem>();
        ioc->bind<CallbackQueue>();
        ioc->bind<EcsRegistry>();
        ioc->bind<ViewManager>();
        ioc->bind<ShaderTable>();
        ioc->bind<EngineConfig>();
        ioc->bind_by_ioc<DllManager>();
        ioc->bind_by_ioc<ProfilerCapture>();
        ioc->bind_by_ioc<Config>();
        ioc->bind_by_ioc<GlslShaderCompiler>();
        ioc->bind_by_ioc<ShaderLibrary>();
        ioc->bind_by_ioc<ShaderManager>();
        ioc->bind_by_ioc<PsoCache>();
        ioc->bind_by_ioc<TextureManager>();
        ioc->bind_by_ioc<MeshManager>();
        ioc->bind_by_ioc<AssetManager>();
        ioc->bind_by_ioc<RenderEngine>();
        bind_by_ioc_scene_manager(ioc);
        bind_by_ioc_game_manager(ioc);
        ioc->bind_by_ioc<Engine>();

        ioc->bind_by_factory<IoAsyncFileSystem>([ioc]() {
            const int num_workers = 4;
            return std::make_shared<IoAsyncFileSystem>(ioc, num_workers);
        });

        ioc->bind_by_factory<TaskManager>([ioc]() {
            const int num_workers = 4;
            return std::make_shared<TaskManager>(num_workers);
        });

        ioc->bind_by_factory<ShaderTaskManager>([ioc]() {
            const int num_workers = 4;
            return std::make_shared<ShaderTaskManager>(num_workers);
        });

        ioc->bind_by_factory<GlfwInput>([ioc]() {
            GlfwWindowManager* window_manager = ioc->resolve_value<GlfwWindowManager>();
            return window_manager->input();
        });

        ioc->bind_by_factory<GlfwWindowManager>([ioc]() {
            const bool vsync      = true;
            const bool client_api = false;
            return std::make_shared<GlfwWindowManager>(vsync, client_api);
        });

        ioc->bind_by_factory<WindowManager>([ioc]() {
            return std::shared_ptr<WindowManager>(ioc->resolve_value<GlfwWindowManager>(), [](auto p) {});
        });

        ioc->bind_by_factory<VKDriver>([ioc]() {
            GlfwWindowManager* window_manager = ioc->resolve_value<GlfwWindowManager>();
            Ref<Window>        window         = window_manager->get_primary_window();

            VKInitInfo init_info;
            init_info.ioc          = ioc;
            init_info.window       = window;
            init_info.app_name     = window->title();
            init_info.engine_name  = "wmoge";
            init_info.required_ext = window_manager->extensions();
            init_info.factory      = window_manager->factory();

            return std::make_shared<VKDriver>(init_info);
        });

        ioc->bind_by_factory<GfxDriver>([ioc]() {
            return std::shared_ptr<GfxDriver>(ioc->resolve_value<VKDriver>(), [](auto p) {});
        });
    }

    static Status unbind_globals(IocContainer* ioc) {
        ioc->unbind<GameManager>();
        ioc->unbind<UiManager>();
        ioc->unbind<ViewManager>();
        ioc->unbind<SceneManager>();
        ioc->unbind<PsoCache>();
        ioc->unbind<ShaderTable>();
        ioc->unbind<ShaderManager>();
        ioc->unbind<ShaderLibrary>();
        ioc->unbind<ShaderTaskManager>();
        ioc->unbind<GlslShaderCompiler>();
        ioc->unbind<TextureManager>();
        ioc->unbind<MeshManager>();
        ioc->unbind<RenderEngine>();
        ioc->unbind<TaskManager>();
        ioc->unbind<VKDriver>();
        ioc->unbind<GlfwInput>();
        ioc->unbind<GlfwWindowManager>();
        ioc->unbind<AssetManager>();
        ioc->unbind<IoAsyncFileSystem>();
        ioc->unbind<PluginManager>();
        ioc->unbind<DllManager>();
        return WG_OK;
    }

    static void bind_rtti(IocContainer* ioc) {
        ioc->resolve_value<RttiTypeStorage>();
        rtti_rtti();
        rtti_asset();
        rtti_audio();
        rtti_grc();
        rtti_glsl();
        rtti_material();
        rtti_mesh();
        rtti_pfx();
        rtti_render();
        rtti_scene();
        rtti_game();
        rtti_system();
        rtti_ui();
    }

    EngineApplication::EngineApplication(EngineApplicationConfig& config) : Application(*config.app_config), m_engine_config(config) {
        m_config.ioc->bind_by_pointer<EngineSignals>(m_engine_config.signals);
    }

    Status EngineApplication::on_register() {
        bind_globals(m_config.ioc);
        bind_rtti(m_config.ioc);

        m_engine = m_config.ioc->resolve_value<Engine>();

        PluginManager* plugin_manager = m_config.ioc->resolve_value<PluginManager>();
        plugin_manager->add(m_engine_config.plugins);

        return m_engine->setup();
    }

    Status EngineApplication::on_init() {
        return m_engine->init();
    }

    Status EngineApplication::on_iteration() {
        return m_engine->iteration();
    }

    Status EngineApplication::on_shutdown() {
        WG_CHECKED(m_engine->shutdown());
        WG_CHECKED(unbind_globals(m_config.ioc));
        return WG_OK;
    }

    bool EngineApplication::should_close() {
        return m_engine->close_requested();
    }

}// namespace wmoge