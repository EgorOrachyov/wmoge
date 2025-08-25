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

#include "hooks.hpp"

#include "core/log.hpp"
#include "core/uuid.hpp"
#include "io/config.hpp"
#include "io/enum.hpp"
#include "platform/common/mount_volume_physical.hpp"
#include "platform/file_system.hpp"
#include "platform/time.hpp"
#include "profiler/profiler_capture.hpp"
#include "system/app.hpp"
#include "system/engine_config.hpp"

namespace wmoge {

    namespace eng_hooks {

        void uuid_gen(CmdLineOptions& options, CmdLineHookList& list) {
            options.add_int("gen_uuids", "gen desired count of uuids' values and outputs them", "0");

            list.add([](CmdLineParseResult& result) -> Status {
                const int uuid_count = result.get_int("gen_uuids");

                if (uuid_count > 0) {
                    for (int i = 0; i < uuid_count; i++) {
                        std::cout << UUID::generate() << std::endl;
                    }
                    return StatusCode::ExitCode0;
                }

                return WG_OK;
            });
        }

        void root_remap(CmdLineOptions& options, CmdLineHookList& list, IocContainer* ioc) {
            options.add_string("root_remap", "remap path to game root folder", "");

            list.add([ioc](CmdLineParseResult& result) {
                FileSystem* file_system = ioc->resolve_value<FileSystem>();

                const std::string root_remap = result.get_string("root_remap");

                if (!root_remap.empty()) {
                    file_system->root(root_remap);
                    std::cout << "remap game exe root direcory to " << root_remap << std::endl;
                }

                return WG_OK;
            });
        }

        void engine(CmdLineOptions& options, CmdLineHookList& list, IocContainer* ioc) {
            options.add_string("game_config", "path to game config folder", "config/");
            options.add_string("engine_config", "path to engine config folder", "engine/config/");
            options.add_string("engine_remap", "remap for engine directory (for debug mostly)", "engine/");

            list.add([ioc](CmdLineParseResult& result) {
                Config*       config        = ioc->resolve_value<Config>();
                EngineConfig* engine_config = ioc->resolve_value<EngineConfig>();
                FileSystem*   file_system   = ioc->resolve_value<FileSystem>();

                const bool                  mount_front = true;
                const std::filesystem::path root_path   = file_system->root_path();

                Ref<MountVolumePhysical> volume_engine = make_ref<MountVolumePhysical>(root_path / result.get_string("engine_remap"), "engine/");
                file_system->add_mounting({"engine/", std::move(volume_engine)}, mount_front);

                Ref<MountVolumePhysical> volume_local = make_ref<MountVolumePhysical>(root_path / ".wgengine", "local/");
                file_system->add_mounting({"local/", std::move(volume_local)}, mount_front);

                Ref<MountVolumePhysical> volume_logs = make_ref<MountVolumePhysical>(root_path / ".wgengine/logs", "logs/");
                file_system->add_mounting({"logs/", std::move(volume_logs)}, mount_front);

                Ref<MountVolumePhysical> volume_debug = make_ref<MountVolumePhysical>(root_path / ".wgengine/debug", "debug/");
                file_system->add_mounting({"debug/", std::move(volume_debug)}, mount_front);

                Ref<MountVolumePhysical> volume_cache = make_ref<MountVolumePhysical>(root_path / ".wgengine/cache", "cache/");
                file_system->add_mounting({"cache/", std::move(volume_cache)}, mount_front);

                const std::string path_game_config   = result.get_string("game_config");
                const std::string path_engine_config = result.get_string("engine_config");

                if (!config->load(path_engine_config + "/engine.cfg", ConfigStackMode::Overwrite)) {
                    std::cerr << "failed to load engine engine.cfg file, check your configuration file or path" << std::endl;
                }
                if (!config->load(path_engine_config + "/game.cfg", ConfigStackMode::Overwrite)) {
                    std::cerr << "failed to load engine game.cfg file, check your configure file of path" << std::endl;
                }

                if (!config->load(path_game_config + "/engine.cfg", ConfigStackMode::Overwrite)) {
                    std::cerr << "failed to load game engine.cfg file, check your configuration file or path" << std::endl;
                }
                if (!config->load(path_game_config + "/game.cfg", ConfigStackMode::Overwrite)) {
                    std::cerr << "failed to load game game.cfg file, check your configure file of path" << std::endl;
                }

                WG_CHECKED(config_read(config, "engine", *engine_config));

                return WG_OK;
            });
        }

        void logs(CmdLineOptions& options, CmdLineHookList& list, IocContainer* ioc) {
            options.add_bool("disable_logs", "disable all logs entirely (overrides config)", "false");

            list.add([ioc](CmdLineParseResult& result) {
                EngineConfig* config      = ioc->resolve_value<EngineConfig>();
                FileSystem*   file_system = ioc->resolve_value<FileSystem>();
                // Console*      console     = ioc->resolve_value<Console>();
                Time* time = ioc->resolve_value<Time>();

                const bool no_logs = result.get_bool("disable_logs");
                if (no_logs) {
                    return WG_OK;
                }

                const bool log_to_out     = config->log.to_out;
                const bool log_to_file    = config->log.to_file;
                const bool log_to_console = config->log.to_console;

                const auto log_to_out_level     = config->log.to_out_level;
                const auto log_to_file_level    = config->log.to_file_level;
                const auto log_to_console_level = config->log.to_console_level;

                if (log_to_file) {
                    auto log_name            = "file";
                    auto file                = LogListenerStream::open_file(file_system, LogListenerStream::make_file_name(time, log_name));
                    auto log_listener_stream = std::make_shared<LogListenerStream>(std::move(file), log_name, log_to_file_level);
                    Log::instance()->listen(log_listener_stream);
                    WG_LOG_INFO("attach file log listener");
                }
                if (log_to_out) {
                    auto log_listener_stdout = std::make_shared<LogListenerStdout>("out", log_to_out_level);
                    Log::instance()->listen(log_listener_stdout);
                    WG_LOG_INFO("attach stdout log listener");
                }
                // if (log_to_console) {
                //     auto log_listener_console = std::make_shared<LogListenerConsole>(console, log_to_console_level);
                //     Log::instance()->listen(log_listener_console);
                //     WG_LOG_INFO("attach console log listener");
                // }

                return WG_OK;
            });
        }

        void profiler(CmdLineOptions& options, CmdLineHookList& list, IocContainer* ioc, ApplicationSignals* app_signals) {
            options.add_bool("profiler", "enable cpu profiler hook", "false");

            list.add([ioc, app_signals](CmdLineParseResult& result) {
                EngineConfig*    config   = ioc->resolve_value<EngineConfig>();
                Time*            time     = ioc->resolve_value<Time>();
                ProfilerCapture* profiler = ioc->resolve_value<ProfilerCapture>();

                bool enable_profiler = false;

                enable_profiler = enable_profiler || result.get_bool("profiler");
                enable_profiler = enable_profiler || config->profiler.enable;

                profiler->enable(enable_profiler);

                if (enable_profiler) {
                    WG_LOG_INFO("attach cpu performance profiler");

                    ProfilerCpu::instance()->calibrate(time->get_start());

                    app_signals->before_init.bind([=]() {
                        profiler->begin_capture(SID("startup"), "debug/profile_startup.json");
                    });
                    app_signals->after_init.bind([=]() {
                        profiler->end_capture();
                        profiler->save_capture();
                    });
                    app_signals->before_loop.bind([=]() {
                        profiler->begin_capture(SID("runtime"), "debug/profile_runtime.json");
                    });
                    app_signals->after_loop.bind([=]() {
                        profiler->end_capture();
                        profiler->save_capture();
                    });
                    app_signals->before_shutdown.bind([=]() {
                        profiler->begin_capture(SID("shutdown"), "debug/profile_shutdown.json");
                    });
                    app_signals->after_shutdown.bind([=]() {
                        profiler->end_capture();
                        profiler->save_capture();
                    });
                }

                return WG_OK;
            });
        }
    }// namespace eng_hooks

}// namespace wmoge
