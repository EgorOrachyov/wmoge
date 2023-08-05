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

#ifndef WMOGE_FILE_SYSTEM_HPP
#define WMOGE_FILE_SYSTEM_HPP

#include "core/data.hpp"
#include "core/string_id.hpp"
#include "core/string_utf.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class FileSystem
     * @brief Abstracts access to the engine and game files directory
     *
     * File system abstracts the way how engine files are stored on a target machine.
     * It uses '/' as a universal delimiter and directory separator.
     * It provides a domain prefix in a form of `<PREFIX>://<PATH>` to specify paths.
     * Standards prefixes are `root://`, `res://` and `cache://`.
     * This prefixes must be used for all paths to access engine files.
     *
     * Prefixes description:
     *  - `root://` prefix to a file relative to the engine directory
     *  - `res://` prefix relative to a engine resources directory
     *  - `cache://` prefix relative to engine cache directory for a cached files
     */
    class FileSystem {
    public:
        FileSystem();
        ~FileSystem();

        std::filesystem::path resolve(const std::string& path);
        bool                  exists(const std::string& path);
        bool                  read_file(const std::string& path, std::string& data);
        bool                  read_file(const std::string& path, Ref<Data>& data);
        bool                  read_file(const std::string& path, std::vector<std::uint8_t>& data);
        bool                  open_file(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode);
        bool                  save_file(const std::string& path, const std::string& data);
        bool                  save_file(const std::string& path, const std::vector<std::uint8_t>& data);
        void                  watch(const std::string& path);

        void root(const std::filesystem::path& path);

        [[nodiscard]] const std::filesystem::path& executable_path() const;
        [[nodiscard]] const std::filesystem::path& root_path() const;
        [[nodiscard]] const std::filesystem::path& resources_path() const;
        [[nodiscard]] const std::filesystem::path& cache_path() const;
        [[nodiscard]] const std::filesystem::path& debug_path() const;

    private:
        std::filesystem::path m_executable_path;
        std::filesystem::path m_root_path;
        std::filesystem::path m_resources_path;
        std::filesystem::path m_cache_path;
        std::filesystem::path m_debug_path;

        std::vector<std::unique_ptr<struct FileSystemWatcher>> m_watchers;
    };

}// namespace wmoge

#endif//WMOGE_FILE_SYSTEM_HPP
