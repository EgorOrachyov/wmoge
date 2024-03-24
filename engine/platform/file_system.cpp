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
#include "debug/profiler.hpp"
#include "event/event_filesystem.hpp"
#include "event/event_manager.hpp"
#include "system/engine.hpp"

#include <filesystem>
#include <functional>

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

    const std::string PREFIX_ROOT = "root://";

    const std::string PREFIX_RES = "res://";
    const std::string REMAP_RES  = "root://resources/";

    const std::string PREFIX_ENG = "eng://";
    const std::string REMAP_ENG  = "root://.wgengine/";

    const std::string PREFIX_CACHE = "cache://";
    const std::string REMAP_CACHE  = "eng://cache/";

    const std::string PREFIX_DEBUG = "debug://";
    const std::string REMAP_DEBUG  = "eng://debug/";

    const std::string PREFIX_LOG = "logs://";
    const std::string REMAP_LOG  = "eng://logs/";

    FileSystem::FileSystem() {
        int         path_length = wai_getExecutablePath(nullptr, 0, nullptr);
        std::string path_exe(path_length + 1, '\0');
        wai_getExecutablePath(path_exe.data(), path_length, nullptr);

        m_executable_path = path_exe;

        root(m_executable_path.parent_path());

        add_rule({PREFIX_RES, REMAP_RES});
        add_rule({PREFIX_ENG, REMAP_ENG});
        add_rule({PREFIX_CACHE, REMAP_CACHE});
        add_rule({PREFIX_DEBUG, REMAP_DEBUG});
        add_rule({PREFIX_LOG, REMAP_LOG});
    }

    FileSystem::~FileSystem() = default;

    std::filesystem::path FileSystem::resolve(const std::string& path) {
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

            if (!applied_rule) {
                if (current_resolve.find(PREFIX_ROOT) == 0) {
                    break;
                }
            }

            resolved = !applied_rule;
        }

        if (current_resolve.find(PREFIX_ROOT) == 0) {
            return m_root_path / current_resolve.substr(PREFIX_ROOT.length());
        }

        WG_LOG_ERROR("unknown domain of the file path " << path);
        return {};
    }
    bool FileSystem::exists(const std::string& path) {
        std::filesystem::path resolved = resolve(path);
        return !resolved.empty() && std::filesystem::exists(resolved);
    }
    Status FileSystem::read_file(const std::string& path, std::string& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::read_file");

        std::ios_base::openmode mode = std::ios::in | std::ios::ate | std::ios::binary;
        std::fstream            file;

        if (!open_file(path, file, mode)) return StatusCode::FailedOpenFile;

        auto size = file.tellg();
        data.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(data.data()), size);
        return StatusCode::Ok;
    }
    Status FileSystem::read_file(const std::string& path, Ref<Data>& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::read_file");

        std::ios_base::openmode mode = std::ios::in | std::ios::ate | std::ios::binary;
        std::fstream            file;

        if (!open_file(path, file, mode)) return StatusCode::FailedOpenFile;

        auto size = file.tellg();
        data      = make_ref<Data>(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(data->buffer()), size);
        return StatusCode::Ok;
    }
    Status FileSystem::read_file(const std::string& path, std::vector<std::uint8_t>& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::read_file");

        std::ios_base::openmode mode = std::ios::in | std::ios::ate | std::ios::binary;
        std::fstream            file;

        if (!open_file(path, file, mode)) return StatusCode::FailedOpenFile;

        auto size = file.tellg();
        data.resize(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(data.data()), size);
        return StatusCode::Ok;
    }
    Status FileSystem::open_file(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::open_file");

        std::filesystem::path resolved = resolve(path);
        std::fstream          file(resolved, mode);

        if (!file.is_open()) {
            return StatusCode::FailedOpenFile;
        }

        fstream = std::move(file);
        return StatusCode::Ok;
    }
    Status FileSystem::save_file(const std::string& path, const std::string& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::save_file");

        std::ios_base::openmode mode = std::ios::out | std::ios::binary;
        std::fstream            file;

        if (!open_file(path, file, mode)) return StatusCode::FailedOpenFile;

        file.write(data.data(), std::streamsize(data.size()));
        return StatusCode::Ok;
    }
    Status FileSystem::save_file(const std::string& path, const std::vector<std::uint8_t>& data) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::save_file");

        std::ios_base::openmode mode = std::ios::out | std::ios::binary;
        std::fstream            file;

        if (!open_file(path, file, mode)) return StatusCode::FailedOpenFile;

        file.write(reinterpret_cast<const char*>(data.data()), std::streamsize(data.size()));
        return StatusCode::Ok;
    }

    void FileSystem::watch(const std::string& path) {
        WG_AUTO_PROFILE_PLATFORM("FileSystem::watch");

        auto resolved_path = resolve(path);

        if (!resolved_path.empty()) {
            m_watchers.emplace_back(std::make_unique<FileSystemWatcher>(resolved_path.string(), [path](const std::string& dropped, const filewatch::Event change_type) {
                auto* engine        = Engine::instance();
                auto* event_manager = engine->event_manager();

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
    }

    void FileSystem::add_rule(const ResolutionRule& rule, bool front) {
        if (front) {
            m_resolution_rules.push_front(rule);
        } else {
            m_resolution_rules.push_back(rule);
        }
    }

    void FileSystem::root(const std::filesystem::path& path) {
        m_root_path = path;

        m_resources_path = m_root_path / "resources";
        m_eng_path       = m_root_path / ".wgengine";
        m_cache_path     = m_eng_path / "cache";
        m_debug_path     = m_eng_path / "debug";
        m_log_path       = m_eng_path / "logs";

        if (!std::filesystem::exists(m_resources_path))
            std::filesystem::create_directories(m_resources_path);
        if (!std::filesystem::exists(m_cache_path))
            std::filesystem::create_directories(m_cache_path);
        if (!std::filesystem::exists(m_debug_path))
            std::filesystem::create_directories(m_debug_path);
        if (!std::filesystem::exists(m_log_path))
            std::filesystem::create_directories(m_log_path);

        auto config_path = m_root_path / "config";

        if (!std::filesystem::exists(config_path))
            std::filesystem::create_directories(config_path);
    }

    const std::filesystem::path& FileSystem::executable_path() const {
        return m_executable_path;
    }
    const std::filesystem::path& FileSystem::root_path() const {
        return m_root_path;
    }
    const std::filesystem::path& FileSystem::resources_path() const {
        return m_resources_path;
    }
    const std::filesystem::path& FileSystem::cache_path() const {
        return m_cache_path;
    }
    const std::filesystem::path& FileSystem::debug_path() const {
        return m_debug_path;
    }

}// namespace wmoge
