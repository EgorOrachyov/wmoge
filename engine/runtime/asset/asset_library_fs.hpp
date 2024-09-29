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

#include "asset/asset_library.hpp"

namespace wmoge {

    /**
     * @class AssetLibraryFileSystem
     * @brief Assets pak based on the filesystem asset directory access
     */
    class AssetLibraryFileSystem final : public AssetLibrary {
    public:
        AssetLibraryFileSystem(std::string directory, class IocContainer* ioc);
        ~AssetLibraryFileSystem() override = default;

        std::string get_name() const override;
        Status      find_asset_meta(const AssetId& name, AssetMeta& meta) override;
        Status      find_asset_data_meta(const Strid& name, AssetDataMeta& meta) override;
        Async       read_data(const Strid& name, array_view<std::uint8_t> data) override;

    private:
        class FileSystem*      m_file_system;
        class RttiTypeStorage* m_rtti_storage;
        std::string            m_directory;
        std::string            m_asset_ext = ".asset";
    };

}// namespace wmoge