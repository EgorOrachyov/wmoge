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

#include "core/date_time.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/string_utils.hpp"
#include "platform/file.hpp"
#include "platform/file_entry.hpp"

#include <filesystem>

namespace wmoge {

    /**
     * @class MountVolume
     * @brief An interface which allows to mount into system virtual file volumes
    */
    class MountVolume : public RefCnt {
    public:
        ~MountVolume() override = default;

        virtual std::string resolve_physical(const std::string& path) { return ""; }
        virtual bool        exists(const std::string& path) { return false; }
        virtual bool        exists_physical(const std::string& path) { return false; }
        virtual Status      get_file_size(const std::string& path, std::size_t& size) { return StatusCode::NotImplemented; }
        virtual Status      get_file_timestamp(const std::string& path, DateTime& timespamp) { return StatusCode::NotImplemented; }
        virtual Status      open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode) { return StatusCode::NotImplemented; }
        virtual Status      open_file_physical(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode) { return StatusCode::NotImplemented; }
        virtual Status      remove_file(const std::string& path) { return StatusCode::NotImplemented; }
        virtual Status      list_directory(const std::string& path, std::vector<FileEntry>& entries) { return StatusCode::NotImplemented; }
        virtual Status      mounted() { return WG_OK; }
    };

}// namespace wmoge