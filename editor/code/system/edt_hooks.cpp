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

#include "edt_hooks.hpp"

#include "platform/common/mount_volume_physical.hpp"
#include "platform/file_system.hpp"

namespace wmoge {

    namespace edt_hooks {

        void editor(CmdLineOptions& options, CmdLineHookList& list, IocContainer* ioc) {
            options.add_string("editor_remap", "remap for editor directory (for debug mostly)", "editor/");

            list.add([ioc](CmdLineParseResult& result) {
                FileSystem* file_system = ioc->resolve_value<FileSystem>();

                const bool                  mount_front = true;
                const std::filesystem::path root_path   = file_system->root_path();

                Ref<MountVolumePhysical> volume_editor = make_ref<MountVolumePhysical>(root_path / result.get_string("editor_remap"), "editor/");
                file_system->add_mounting({"editor/", std::move(volume_editor)}, mount_front);

                Ref<MountVolumePhysical> volume_local = make_ref<MountVolumePhysical>(root_path / ".wgeditor", "editor_local/");
                file_system->add_mounting({"editor_local/", std::move(volume_local)}, mount_front);

                return WG_OK;
            });
        }

    };// namespace edt_hooks

}// namespace wmoge
