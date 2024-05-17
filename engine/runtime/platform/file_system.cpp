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
#include "event/event_filesystem.hpp"
#include "event/event_manager.hpp"
#include "platform/common/mount_volume_physical.hpp"
#include "profiler/profiler.hpp"
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <algorithm>
#include <filesystem>
#include <functional>

#include <FileWatch.hpp>
#include <whereami.h>

namespace wmoge {

    const std::string FileSystem::PREFIX_ENGINE = "engine://";
    const std::string FileSystem::PREFIX_ASSET  = "asset://";
    const std::string FileSystem::PREFIX_LOCAL  = "local://";
    const std::string FileSystem::PREFIX_CACHE  = "cache://";
    const std::string FileSystem::PREFIX_DEBUG  = "debug://";
    const std::string FileSystem::PREFIX_LOG    = "logs://";

    static const std::string REMAP_ROOT   = "../";
    static const std::string REMAP_ENGINE = "engine/";
    static const std::string REMAP_ASSET  = "assets/";
    static const std::string REMAP_LOCAL  = ".wgengine/";
    static const std::string REMAP_DEBUG  = "local://debug/";
    static const std::string REMAP_CACHE  = "local://cache/";
    static const std::string REMAP_LOG    = "local://logs/";

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

    std::string FileSystem::resolve(const std::string& path) {
        std::string current_resolve = path;
        bool        resolved        = false;

        while (!resolved) {
            bool applied_rule = false;

            for (const auto& rule : m_resolution_rules) {
                if (current_resolve.find(rule.first) == 0) {
                    current_resolve = rule.second + current_resolve.substr(rule.first.length());
                    applied_rule    = true;
                    break;
                }
            }

            resolved = !applied_rule;
        }

        return current_resolve;
    }
    std::filesystem::path FileSystem::resolve_physical(const std::string& path) {
        const std::string resolved_path = resolve(path);

        if (!resolved_path.empty()) {
            return m_root_path / resolved_path;
        }

        return {};
    }
    bool FileSystem::exists(const std::string& path) {
        std::string resolved = resolve(path);

        if (resolved.empty()) {
            return false;
        }

        for (const MountPoint& mount_point : m_mount_points) {
            const auto& prefix  = mount_point.first;
            const auto& adapter = mount_point.second;

            if (resolved.find(prefix) == 0) {
                if (adapter->exists(resolved)) {
                    return true;
                }
            }
        }

        return false;
    }
    bool FileSystem::exists_physical(const std::string& path) {
        std::filesystem::path resolved = resolve_physical(path);

        if (resolved.empty()) {
            return false;
        }

        return std::filesystem::exists(resolved);
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

    Status FileSystem::open_file_physical(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::open_file_physical");

        std::filesystem::path resolved = resolve_physical(path);
        std::fstream          file;

        if (mode & std::ios::out) {
            std::filesystem::create_directories(resolved.parent_path());
        }

        file.open(resolved, mode);

        if (!file.is_open()) {
            return StatusCode::FailedOpenFile;
        }

        fstream = std::move(file);
        return WG_OK;
    }

    Status FileSystem::open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::open_file");

        const std::string resolved_path = resolve(path);

        for (const MountPoint& mount_point : m_mount_points) {
            const auto& prefix  = mount_point.first;
            const auto& adapter = mount_point.second;

            if (resolved_path.find(prefix) == 0) {
                if (adapter->exists(resolved_path) || mode.get(FileOpenMode::Out)) {
                    return adapter->open_file(resolved_path, file, mode);
                }
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

    void FileSystem::watch(const std::string& path) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::watch");

        auto resolved_path = resolve_physical(path);

        if (resolved_path.empty()) {
            WG_LOG_ERROR("failed to resolve to physical path for a watch " << path);
            return;
        }

        EventManager* event_manager = IocContainer::iresolve_v<EventManager>();

        m_watchers.emplace_back(std::make_unique<FileSystemWatcher>(resolved_path.string(), [event_manager, path](const std::string& dropped, const filewatch::Event change_type) {
            auto event    = make_event<EventFileSystem>();
            event->path   = path;
            event->entry  = dropped;
            event->action = FileSystemAction::Unknown;

            switch (change_type) {
                case filewatch::Event::added:
                    event->action = FileSystemAction::Added;
                    break;
                case filewatch::Event::modified:
                    event->action = FileSystemAction::Modified;
                    break;
                case filewatch::Event::removed:
                    event->action = FileSystemAction::Removed;
                    break;
                default:
                    WG_LOG_ERROR("unknown event type on file path=" << event->path << " entry=" << dropped);
                    break;
            }

            event_manager->dispatch(event);
        }));
    }

    void FileSystem::add_rule(const ResolutionRule& rule, bool front) {
        auto iter = std::find_if(m_resolution_rules.begin(), m_resolution_rules.end(), [&](const ResolutionRule& r) {
            return r.first == rule.first;
        });
        if (iter != m_resolution_rules.end()) {
            m_resolution_rules.erase(iter);
        }
        if (front) {
            m_resolution_rules.push_front(rule);
        } else {
            m_resolution_rules.push_back(rule);
        }
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

    void FileSystem::setup_mappings() {
        Config* config = IocContainer::iresolve_v<Config>();

        m_resolution_rules.clear();

        add_rule({PREFIX_ENGINE, config->get_string_or_default(SID("file_system.engine_path"), REMAP_ENGINE)});
        add_rule({PREFIX_ASSET, config->get_string_or_default(SID("file_system.asset_path"), REMAP_ASSET)});
        add_rule({PREFIX_LOCAL, config->get_string_or_default(SID("file_system.local_path"), REMAP_LOCAL)});
        add_rule({PREFIX_CACHE, config->get_string_or_default(SID("file_system.cache_path"), REMAP_CACHE)});
        add_rule({PREFIX_DEBUG, config->get_string_or_default(SID("file_system.debug_path"), REMAP_DEBUG)});
        add_rule({PREFIX_LOG, config->get_string_or_default(SID("file_system.logs_path"), REMAP_LOG)});
    }

    const std::filesystem::path& FileSystem::executable_path() const {
        return m_executable_path;
    }
    const std::filesystem::path& FileSystem::root_path() const {
        return m_root_path;
    }

}// namespace wmoge
