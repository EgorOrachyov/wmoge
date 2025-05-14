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

    std::string MountVolumePhysical::resolve_physical(const std::string& path) {
        auto prefix = path.find(m_mapping);
        if (prefix != 0) {
            return "";
        }

        const std::filesystem::path remapped = remap_path(path);
        return remapped.string();
    }

    bool MountVolumePhysical::exists(const std::string& path) {
        if (!check_prefix(path)) {
            return false;
        }

        const std::filesystem::path remapped = remap_path(path);
        return std::filesystem::exists(remapped);
    }

    bool MountVolumePhysical::exists_physical(const std::string& path) {
        if (!check_prefix(path)) {
            return false;
        }

        const std::filesystem::path remapped = remap_path(path);
        return std::filesystem::exists(remapped);
    }

    Status MountVolumePhysical::get_file_size(const std::string& path, std::size_t& size) {
        if (!check_prefix(path)) {
            return StatusCode::FailedOpenFile;
        }

        const std::filesystem::path remapped = remap_path(path);
        if (!std::filesystem::exists(remapped)) {
            return StatusCode::FailedOpenFile;
        }

        size = static_cast<std::size_t>(std::filesystem::file_size(remapped));
        return WG_OK;
    }

    Status MountVolumePhysical::get_file_timestamp(const std::string& path, DateTime& timespamp) {
        if (!check_prefix(path)) {
            return StatusCode::FailedOpenFile;
        }

        const std::filesystem::path remapped = remap_path(path);
        if (!std::filesystem::exists(remapped)) {
            return StatusCode::FailedOpenFile;
        }

        using DateTimeClock = DateTime::Clock;
        using FileClock     = std::filesystem::file_time_type::clock;

        auto last_write_time   = std::filesystem::last_write_time(remapped);
        auto fs_clock_duration = last_write_time.time_since_epoch();
        auto dt_clock_duration = std::chrono::duration_cast<DateTimeClock::duration>(fs_clock_duration);

        timespamp = DateTime(DateTime::TimePoint(dt_clock_duration));
        return WG_OK;
    }

    Status MountVolumePhysical::open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode) {
        if (!check_prefix(path)) {
            return StatusCode::FailedOpenFile;
        }

        const std::filesystem::path remapped      = remap_path(path);
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

    Status MountVolumePhysical::open_file_physical(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode) {
        if (!check_prefix(path)) {
            return StatusCode::FailedOpenFile;
        }

        const std::filesystem::path remapped = remap_path(path);

        if (mode & std::ios_base::out) {
            std::filesystem::create_directories(remapped.parent_path());
        }

        fstream.open(remapped, mode);

        if (!fstream.is_open()) {
            return StatusCode::FailedOpenFile;
        }

        return WG_OK;
    }

    Status MountVolumePhysical::remove_file(const std::string& path) {
        if (!check_prefix(path)) {
            return StatusCode::FailedFindFile;
        }

        const std::filesystem::path remapped = remap_path(path);
        if (!std::filesystem::exists(remapped)) {
            return WG_OK;
        }
        if (!std::filesystem::remove(path)) {
            return StatusCode::FailedRemoveFile;
        }

        return WG_OK;
    }

    Status MountVolumePhysical::list_directory(const std::string& path, std::vector<FileEntry>& entries) {
        if (!check_prefix(path)) {
            return StatusCode::FailedFindFile;
        }

        const std::filesystem::path remapped = remap_path(path);
        if (!std::filesystem::exists(remapped)) {
            return WG_OK;
        }

        for (const auto& iter : std::filesystem::directory_iterator(remapped)) {
            FileEntry& entry = entries.emplace_back();
            entry.name       = iter.path().filename().string();

            if (iter.is_regular_file()) {
                entry.type = FileEntryType::File;
            }
            if (iter.is_directory()) {
                entry.type = FileEntryType::Directory;
            }
        }

        return WG_OK;
    }

    Status MountVolumePhysical::mounted() {
        return WG_OK;
    }

    bool MountVolumePhysical::check_prefix(const std::string& path) {
        auto prefix = path.find(m_mapping);
        return prefix == 0;
    }

    std::filesystem::path MountVolumePhysical::remap_path(const std::string& path) {
        return std::move(m_path / path.substr(m_mapping.length()));
    }

}// namespace wmoge