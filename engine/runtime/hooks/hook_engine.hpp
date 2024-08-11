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

#include "core/cmd_line.hpp"
#include "platform/common/mount_volume_physical.hpp"
#include "platform/file_system.hpp"
#include "system/config.hpp"
#include "system/hook.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    /** 
     * @class HookEngine
     * @brief Engine hook to setup runtime of the game
     */
    class HookEngine : public Hook {
    public:
        ~HookEngine() override = default;

        std::string get_name() const override {
            return "engine";
        }

        void on_add_cmd_line_options(CmdLine& cmd_line) override {
            cmd_line.add_string("game_config", "path to game config folder", "config/");
            cmd_line.add_string("engine_config", "path to engine config folder", "engine/config/");
            cmd_line.add_string("engine_remap", "remap for engine directory (for debug mostly)", "engine/");
        }

        Status on_process(CmdLine& cmd_line) override {
            Config*     config = IocContainer::iresolve_v<Config>();
            FileSystem* fs     = IocContainer::iresolve_v<FileSystem>();

            const bool                  mount_front = true;
            const std::filesystem::path root_path   = fs->root_path();

            Ref<MountVolumePhysical> volume_engine = make_ref<MountVolumePhysical>(root_path / cmd_line.get_string("engine_remap"), "engine/");
            fs->add_mounting({"engine/", std::move(volume_engine)}, mount_front);

            Ref<MountVolumePhysical> volume_local = make_ref<MountVolumePhysical>(root_path / ".wgengine", "local/");
            fs->add_mounting({"local/", std::move(volume_local)}, mount_front);

            Ref<MountVolumePhysical> volume_logs = make_ref<MountVolumePhysical>(root_path / ".wgengine/logs", "logs/");
            fs->add_mounting({"logs/", std::move(volume_logs)}, mount_front);

            Ref<MountVolumePhysical> volume_debug = make_ref<MountVolumePhysical>(root_path / ".wgengine/debug", "debug/");
            fs->add_mounting({"debug/", std::move(volume_debug)}, mount_front);

            Ref<MountVolumePhysical> volume_cache = make_ref<MountVolumePhysical>(root_path / ".wgengine/cache", "cache/");
            fs->add_mounting({"cache/", std::move(volume_cache)}, mount_front);

            const std::string game_config   = cmd_line.get_string("game_config");
            const std::string engine_config = cmd_line.get_string("engine_config");

            if (!config->load(engine_config + "/engine.cfg", ConfigStackMode::Overwrite)) {
                std::cerr << "failed to load engine engine.cfg file, check your configuration file or path" << std::endl;
            }
            if (!config->load(engine_config + "/game.cfg", ConfigStackMode::Overwrite)) {
                std::cerr << "failed to load engine game.cfg file, check your configure file of path" << std::endl;
            }
            if (!config->load(engine_config + "/cvars.cfg", ConfigStackMode::Overwrite)) {
                std::cerr << "failed to load engine cvars.cfg file, check your configure file of path" << std::endl;
            }

            if (!config->load(game_config + "/engine.cfg", ConfigStackMode::Overwrite)) {
                std::cerr << "failed to load game engine.cfg file, check your configuration file or path" << std::endl;
            }
            if (!config->load(game_config + "/game.cfg", ConfigStackMode::Overwrite)) {
                std::cerr << "failed to load game game.cfg file, check your configure file of path" << std::endl;
            }
            if (!config->load(game_config + "/cvars.cfg", ConfigStackMode::Overwrite)) {
                std::cerr << "failed to load game cvars.cfg file, check your configure file of path" << std::endl;
            }

            return WG_OK;
        }
    };

}// namespace wmoge