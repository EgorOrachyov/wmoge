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

#include "asset_library_fs.hpp"

#include "asset/asset_manager.hpp"
#include "core/string_utils.hpp"
#include "io/async_file_system.hpp"
#include "io/tree.hpp"
#include "io/tree_yaml.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "rtti/type_storage.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    AssetLibraryFileSystem::AssetLibraryFileSystem(std::string directory, IocContainer* ioc) {
        m_directory         = std::move(directory);
        m_file_system       = ioc->resolve_value<FileSystem>();
        m_async_file_system = ioc->resolve_value<IoAsyncFileSystem>();
        m_rtti_storage      = ioc->resolve_value<RttiTypeStorage>();
    }

    std::string AssetLibraryFileSystem::get_name() const {
        return "AssetLibraryFileSystem";
    }

    static std::string make_asset_meta_path(const std::string& directory, const AssetId& name, const std::string& ext) {
        return directory + name.str() + ext;
    }

    bool AssetLibraryFileSystem::has_asset(const AssetId& name) {
        WG_AUTO_PROFILE_ASSET("AssetLibraryFileSystem::has_asset");
        return m_file_system->exists(make_asset_meta_path(m_directory, name, m_asset_ext));
    }

    Status AssetLibraryFileSystem::find_asset_meta(const AssetId& name, AssetMeta& meta) {
        WG_AUTO_PROFILE_ASSET("AssetLibraryFileSystem::find_asset_meta");

        IoContext context;
        context.add(m_file_system);
        context.add(m_rtti_storage);

        IoYamlTree tree;
        WG_CHECKED(tree.parse_file(m_file_system, make_asset_meta_path(m_directory, name, m_asset_ext)));
        WG_TREE_READ(context, tree, meta);

        return WG_OK;
    }

    static std::string make_asset_data_path(const std::string& directory, const AssetId& name) {
        return directory + name.str();
    }

    Status AssetLibraryFileSystem::find_asset_data_meta(const Strid& name, AssetDataMeta& meta) {
        WG_AUTO_PROFILE_ASSET("AssetLibraryFileSystem::find_asset_data_meta");

        WG_CHECKED(m_file_system->get_file_size(make_asset_data_path(m_directory, name), meta.size));
        meta.size_compressed = 0;
        meta.hash            = Sha256();
        meta.compression     = AssetCompressionMode::None;

        return WG_OK;
    }

    Async AssetLibraryFileSystem::read_data(const Strid& name, array_view<std::uint8_t> data) {
        WG_AUTO_PROFILE_ASSET("AssetLibraryFileSystem::read_data");
        return m_async_file_system->read_file(make_asset_data_path(m_directory, name), data).as_async();
    }

}// namespace wmoge
