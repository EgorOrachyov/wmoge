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

#include "mount_volume_physical.hpp"

#include "platform/common/file_physical.hpp"

namespace wmoge {

    MountVolumePhysical::MountVolumePhysical(std::filesystem::path path, std::string mapping) {
        m_path    = std::move(path);
        m_mapping = std::move(mapping);
    }

    void MountVolumePhysical::change_path(std::filesystem::path path) {
        m_path = std::move(path);
    }

    bool MountVolumePhysical::exists(const std::string& path) {
        auto prefix = path.find(m_mapping);
        if (prefix != 0) {
            return false;
        }

        const std::filesystem::path remapped = m_path / path.substr(m_mapping.length());
        return std::filesystem::exists(remapped);
    }

    Status MountVolumePhysical::open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode) {
        auto prefix = path.find(m_mapping);
        if (prefix != 0) {
            return StatusCode::FailedOpenFile;
        }

        const std::filesystem::path remapped      = m_path / path.substr(m_mapping.length());
        Ref<FilePhysical>           file_physical = make_ref<FilePhysical>();

        if (mode.get(FileOpenMode::Out)) {
            std::filesystem::create_directories(remapped.parent_path());
        }
        if (!file_physical->open(remapped, mode)) {
            return StatusCode::FailedOpenFile;
        }

        file = file_physical.as<File>();
        return WG_OK;
    }

    Status MountVolumePhysical::mounted() {
        return WG_OK;
    }

}// namespace wmoge