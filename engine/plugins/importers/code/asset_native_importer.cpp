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

#include "asset_native_importer.hpp"

#include "asset/asset_deps_collector.hpp"
#include "asset/asset_native_loader.hpp"
#include "io/tree_yaml.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status AssetNativeImporter::get_file_extensions(std::vector<std::string>& extensions) {
        return WG_OK;
    }

    Status AssetNativeImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        return WG_OK;
    }

    Status AssetNativeImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status AssetNativeImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("AssetNativeImporter::import");

        IoYamlTree tree;
        if (!tree.parse_file(context.get_file_system(), path)) {
            WG_LOG_ERROR("failed to parse file " << path);
            return StatusCode::FailedParse;
        }

        IoContext io_context = context.get_io_context();

        Ref<RttiObject> object;
        if (!tree_read(io_context, tree, object)) {
            WG_LOG_ERROR("failed to parse object " << path);
            return StatusCode::FailedParse;
        }

        Ref<Asset> asset = object.cast<Asset>();
        if (!asset) {
            WG_LOG_ERROR("failed to cast object to asset type " << path);
            return StatusCode::InvalidData;
        }

        AssetDepsCollector collector(asset, path);
        if (!collector.collect()) {
            WG_LOG_ERROR("failed to collect deps for " << path);
            return StatusCode::Error;
        }

        AssetImportAssetInfo info;
        info.uuid   = context.alloc_asset_uuid(path);
        info.path   = path;
        info.cls    = asset->get_class();
        info.loader = AssetNativeLoader::get_class_static();
        info.deps   = std::move(collector.to_vector());
        info.artifacts.push_back(AssetImportArtifactInfo{path, object});

        context.set_main_asset(std::move(info));
        context.add_source(path);
        return WG_OK;
    }

}// namespace wmoge