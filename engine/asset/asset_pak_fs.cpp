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

#include "asset_pak_fs.hpp"

#include "asset/asset_manager.hpp"
#include "core/class.hpp"
#include "debug/profiler.hpp"
#include "io/yaml.hpp"
#include "platform/file_system.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    AssetPakFileSystem::AssetPakFileSystem() {
        m_file_system = IocContainer::instance()->resolve_v<FileSystem>();
    }
    std::string AssetPakFileSystem::get_name() const {
        return "pak_fs";
    }
    Status AssetPakFileSystem::get_meta(const AssetId& name, AssetMeta& meta) {
        WG_AUTO_PROFILE_ASSET("AssetPakFileSystem::meta");

        std::string meta_file_path = name.str() + AssetMetaFile::FILE_EXTENSION;

        auto res_tree = yaml_parse_file(meta_file_path);
        if (res_tree.empty()) {
            WG_LOG_ERROR("failed to parse tree file " << meta_file_path);
            return StatusCode::FailedParse;
        }

        AssetMetaFile asset_file;

        if (!yaml_read(res_tree, asset_file)) {
            WG_LOG_ERROR("failed to parse .asset file " << meta_file_path);
            return StatusCode::FailedRead;
        }

        auto loader = IocContainer::instance()->resolve_v<AssetManager>()->find_loader(asset_file.loader);

        meta.uuid        = asset_file.uuid;
        meta.cls         = Class::class_ptr(asset_file.rtti);
        meta.pak         = this;
        meta.loader      = loader.value_or(nullptr);
        meta.deps        = std::move(asset_file.deps);
        meta.import_data = asset_file.import_data;

        return StatusCode::Ok;
    }

}// namespace wmoge
