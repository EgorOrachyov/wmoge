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

#include "file_system.hpp"

#include "core/data.hpp"
#include "core/log.hpp"
#include "platform/common/mount_volume_physical.hpp"
#include "profiler/profiler.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <type_traits>

#include <FileWatch.hpp>
#include <whereami.h>

namespace wmoge {

    /**
     * @class FileSystemWatcher
     * @brief Wrapper for filewatch specifics
     */
    struct FileSystemWatcher {
        filewatch::FileWatch<std::string> watch;

        FileSystemWatcher(std::string path, std::function<void(const std::string&, const filewatch::Event)> callback)
            : watch(std::move(path), std::move(callback)) {}
    };

    FileSystem::FileSystem() {
        int         path_length = wai_getExecutablePath(nullptr, 0, nullptr);
        std::string path_exe(path_length + 1, '\0');
        wai_getExecutablePath(path_exe.data(), path_length, nullptr);

        m_executable_path = path_exe;
        m_root_volume     = make_ref<MountVolumePhysical>(std::filesystem::path(), "").as<MountVolume>();

        root(m_executable_path.parent_path());

        add_mounting({"", m_root_volume});
    }

    FileSystem::~FileSystem() = default;

    template<typename Result, typename Functor>
    static Result filter_mounts_and_apply(const std::deque<FileSystem::MountPoint> mount_points, const std::string& path, Result default_value, Functor functor) {
        for (const FileSystem::MountPoint& mount_point : mount_points) {
            const auto& prefix  = mount_point.first;
            const auto& adapter = mount_point.second;

            if (path.find(prefix) == 0) {
                return functor(adapter.get());
            }
        }

        return default_value;
    }

    std::string FileSystem::resolve_physical(const std::string& path) {
        return filter_mounts_and_apply<std::string>(m_mount_points, path, "", [&](MountVolume* adapter) {
            return adapter->resolve_physical(path);
        });
    }

    bool FileSystem::exists(const std::string& path) {
        return filter_mounts_and_apply<bool>(m_mount_points, path, false, [&](MountVolume* adapter) {
            return adapter->exists(path);
        });
    }

    bool FileSystem::exists_physical(const std::string& path) {
        return filter_mounts_and_apply<bool>(m_mount_points, path, false, [&](MountVolume* adapter) {
            return adapter->exists_physical(path);
        });
    }

    Status FileSystem::get_file_size(const std::string& path, std::size_t& size) {
        return filter_mounts_and_apply<Status>(m_mount_points, path, StatusCode::FailedFindFile, [&](MountVolume* adapter) {
            return adapter->get_file_size(path, size);
        });
    }

    Status FileSystem::get_file_timespamp(const std::string& path, DateTime& timespamp) {
        return filter_mounts_and_apply<Status>(m_mount_points, path, StatusCode::FailedFindFile, [&](MountVolume* adapter) {
            return adapter->get_file_timespamp(path, timespamp);
        });
    }

    Status FileSystem::read_file(const std::string& path, std::string& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::read_file");

        FileOpenModeFlags mode = {FileOpenMode::In, FileOpenMode::Binary};
        Ref<File>         file;

        if (!open_file(path, file, mode)) {
            return StatusCode::FailedOpenFile;
        }

        std::size_t size = 0;
        if (!file->size(size)) {
            return StatusCode::FailedRead;
        }

        data.resize(size);

        if (!file->nread(data.data(), size)) {
            return StatusCode::FailedRead;
        }

        return WG_OK;
    }

    Status FileSystem::read_file(const std::string& path, Ref<Data>& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::read_file");

        FileOpenModeFlags mode = {FileOpenMode::In, FileOpenMode::Binary};
        Ref<File>         file;

        if (!open_file(path, file, mode)) {
            return StatusCode::FailedOpenFile;
        }

        std::size_t size = 0;
        if (!file->size(size)) {
            return StatusCode::FailedRead;
        }

        data = make_ref<Data>(size);

        if (!file->nread(data->buffer(), size)) {
            return StatusCode::FailedRead;
        }

        return WG_OK;
    }

    Status FileSystem::read_file(const std::string& path, std::vector<std::uint8_t>& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::read_file");

        FileOpenModeFlags mode = {FileOpenMode::In, FileOpenMode::Binary};
        Ref<File>         file;

        if (!open_file(path, file, mode)) {
            return StatusCode::FailedOpenFile;
        }

        std::size_t size = 0;
        if (!file->size(size)) {
            return StatusCode::FailedRead;
        }

        data.resize(size);

        if (!file->nread(data.data(), size)) {
            return StatusCode::FailedRead;
        }

        return WG_OK;
    }

    Status FileSystem::open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::open_file");

        for (const MountPoint& mount_point : m_mount_points) {
            const auto& prefix  = mount_point.first;
            const auto& adapter = mount_point.second;

            if (path.find(prefix) == 0) {
                if (adapter->exists(path) || mode.get(FileOpenMode::Out)) {
                    return adapter->open_file(path, file, mode);
                }
                return StatusCode::FailedOpenFile;
            }
        }

        return StatusCode::FailedOpenFile;
    }

    Status FileSystem::open_file_physical(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::open_file_physical");

        for (const MountPoint& mount_point : m_mount_points) {
            const auto& prefix  = mount_point.first;
            const auto& adapter = mount_point.second;

            if (path.find(prefix) == 0) {
                if (adapter->exists(path) || (mode & std::ios_base::out)) {
                    return adapter->open_file_physical(path, fstream, mode);
                }
                return StatusCode::FailedOpenFile;
            }
        }

        return StatusCode::FailedOpenFile;
    }

    Status FileSystem::save_file(const std::string& path, const std::string& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::save_file");

        FileOpenModeFlags mode = {FileOpenMode::Out, FileOpenMode::Binary};
        Ref<File>         file;

        if (!open_file(path, file, mode)) {
            return StatusCode::FailedOpenFile;
        }

        if (!file->nwrite(data.data(), data.size())) {
            return StatusCode::FailedWrite;
        }

        return WG_OK;
    }

    Status FileSystem::save_file(const std::string& path, const std::vector<std::uint8_t>& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::save_file");

        FileOpenModeFlags mode = {FileOpenMode::Out, FileOpenMode::Binary};
        Ref<File>         file;

        if (!open_file(path, file, mode)) {
            return StatusCode::FailedOpenFile;
        }

        if (!file->nwrite(data.data(), data.size())) {
            return StatusCode::FailedWrite;
        }

        return WG_OK;
    }

    Status FileSystem::hash_file(const std::string& path, Sha256& file_hash) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::hash_file");

        std::vector<std::uint8_t> file_data;
        WG_CHECKED(read_file(path, file_data));

        Sha256Builder sha_builder;
        file_hash = sha_builder.hash(file_data.data(), file_data.size()).get();

        return WG_OK;
    }

    void FileSystem::watch(const std::string& path, std::function<void(const FileSystemEvent&)> callback) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::watch");

        auto resolved_path = resolve_physical(path);

        if (resolved_path.empty()) {
            WG_LOG_ERROR("failed to resolve to physical path for a watch " << path);
            return;
        }

        m_watchers.emplace_back(std::make_unique<FileSystemWatcher>(resolved_path, [path, func = std::move(callback)](const std::string& dropped, const filewatch::Event change_type) {
            FileSystemEvent event;
            event.path   = path;
            event.entry  = dropped;
            event.action = FileSystemAction::Unknown;

            switch (change_type) {
                case filewatch::Event::added:
                    event.action = FileSystemAction::Added;
                    break;
                case filewatch::Event::modified:
                    event.action = FileSystemAction::Modified;
                    break;
                case filewatch::Event::removed:
                    event.action = FileSystemAction::Removed;
                    break;
                default:
                    WG_LOG_ERROR("unknown event type on file path=" << event.path << " entry=" << dropped);
                    break;
            }

            func(event);
        }));
    }

    void FileSystem::add_mounting(const MountPoint& point, bool front) {
        if (front) {
            m_mount_points.push_front(point);
        } else {
            m_mount_points.push_back(point);
        }
    }

    void FileSystem::root(const std::filesystem::path& path) {
        m_root_path = path;
        m_root_volume.cast<MountVolumePhysical>()->change_path(m_root_path);
    }

    const std::filesystem::path& FileSystem::executable_path() const {
        return m_executable_path;
    }

    const std::filesystem::path& FileSystem::root_path() const {
        return m_root_path;
    }

}// namespace wmoge
