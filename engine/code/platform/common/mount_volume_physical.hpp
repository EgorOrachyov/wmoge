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

#include "platform/mount_volume.hpp"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <utility>

namespace wmoge {

    /**
     * @class MountVolumePhysical
     * @brief Wrapper for a physical folder location
    */
    class MountVolumePhysical : public MountVolume {
    public:
        MountVolumePhysical(std::filesystem::path path, std::string mapping);
        ~MountVolumePhysical() override = default;

        void        change_path(std::filesystem::path path);
        std::string resolve_physical(const std::string& path) override;
        bool        exists(const std::string& path) override;
        bool        exists_physical(const std::string& path) override;
        Status      get_file_size(const std::string& path, std::size_t& size) override;
        Status      get_file_timespamp(const std::string& path, DateTime& timespamp) override;
        Status      open_file(const std::string& path, Ref<File>& file, const FileOpenModeFlags& mode) override;
        Status      open_file_physical(const std::string& path, std::fstream& fstream, std::ios_base::openmode mode) override;
        Status      mounted() override;

    private:
        bool                  check_prefix(const std::string& path);
        std::filesystem::path remap_path(const std::string& path);

    private:
        std::filesystem::path m_path;
        std::string           m_mapping;
    };

}// namespace wmoge