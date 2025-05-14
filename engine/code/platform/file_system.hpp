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

#include "core/data.hpp"
#include "core/date_time.hpp"
#include "core/ref.hpp"
#include "core/sha256.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/string_utf.hpp"
#include "platform/file.hpp"
#include "platform/file_entry.hpp"
#include "platform/mount_volume.hpp"

#include <deque>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace wmoge {

    /** @brief Type of file system file actions */
    enum class FileSystemAction {
        Unknown,
        Added,
        Modified,
        Removed
    };

    /**
     * @class FileSystemEvent
     * @brief Action event dispatch from raw input and active mapping
     */
    struct FileSystemEvent {
        FileSystemAction action;
        std::string      path;
        std::string      entry;
    };

    /**
     * @class FileSystem
     * @brief Abstracts access to the engine and game files directory
     *
     * File system abstracts the way how engine files are stored on a target machine.
     * It uses '/' as a universal delimiter and directory separator.
     * It provides a domain prefix in a form of `<PREFIX>/<PATH>` to specify paths.
     * Standards prefixes are `engine/`, `asset/` and `cache/`.
     * This prefixes must be used for all paths to access engine files.
     *
     * Prefixes description:
     *  - `engine/` prefix to a file relative to the engine files directory
     *  - `local/`  prefix to a file relative to the local (project) files directory
     *  - `asset/`  prefix relative to project assets directory
     *  - `cache/`  prefix relative to project cache directory for cached files
     *  - `logs/`   prefix relative to project logs directory
     */
    class FileSystem {
    public:
        /** @brief Mount point allowing to virtualize file system files structure */
        using MountPoint = std::pair<std::string, Ref<MountVolume>>;

        FileSystem();
        ~FileSystem();

        std::string resolve_physical(const std::string& path);
        bool        exists(const std::string& path);
        bool        exists_physical(const std::string& path);
        Status      get_file_size(const std::string& path, std::size_t& size);
        Status      get_file_timestamp(const std::string& path, DateTime& timespamp);
        Status      read_file(const std::string& path, std::string& data);
        Status      read_file(const std::string& path, Ref<Data>& data);
        Status      read_file(const std::string& path, std::vector<std::uint8_t>& data);
        Status      open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode);
        Status      open_file_physical(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode);
        Status      save_file(const std::string& path, const std::string& data);
        Status      save_file(const std::string& path, const std::vector<std::uint8_t>& data);
        Status      hash_file(const std::string& path, Sha256& file_hash);
        Status      remove_file(const std::string& path);
        Status      list_directory(const std::string& path, std::vector<FileEntry>& entries);
        void        watch(const std::string& path, std::function<void(const FileSystemEvent&)> callback);
        void        add_mounting(const MountPoint& point, bool front = false);
        void        root(const std::filesystem::path& path);

        [[nodiscard]] const std::filesystem::path& executable_path() const;
        [[nodiscard]] const std::filesystem::path& root_path() const;

    private:
        using FileSystemWatcherPtr = std::unique_ptr<struct FileSystemWatcher>;

        std::deque<MountPoint>            m_mount_points;   // serached after resouliton second, ordered by priority
        std::filesystem::path             m_executable_path;// absolute exe path
        std::filesystem::path             m_root_path;      // path to root directory of engine files (virtual)
        std::vector<FileSystemWatcherPtr> m_watchers;       // wather instances tracking file changes
        Ref<MountVolume>                  m_root_volume;    // default root volume of fs
    };

}// namespace wmoge