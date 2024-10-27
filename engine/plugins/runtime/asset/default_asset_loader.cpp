#include "default_asset_loader.hpp"
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

#include "default_asset_loader.hpp"

#include "asset/asset_manager.hpp"
#include "core/ioc_container.hpp"
#include "io/tree_yaml.hpp"
#include "profiler/profiler_cpu.hpp"
#include "rtti/type_storage.hpp"

namespace wmoge {

    Status DefaultAssetLoader::fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) {
        Ref<AssetImportData> import_data = context.asset_meta.import_data.cast<AssetImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data to load " << asset_id);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << asset_id);
            return StatusCode::InvalidData;
        }
        request.add_data_file(FILE_TAG, import_data->source_files[0].file);
        return WG_OK;
    }

    Status DefaultAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Asset>& asset) {
        WG_PROFILE_CPU_ASSET("DefaultAssetLoader::load_typed");

        Ref<AssetImportData> import_data = context.asset_meta.import_data.cast<AssetImportData>();
        assert(import_data);

        RttiTypeStorage* type_storage = context.ioc->resolve_value<RttiTypeStorage>();
        if (!type_storage) {
            WG_LOG_ERROR("no rtti storage for " << asset_id);
            return StatusCode::InvalidState;
        }

        RttiClass* rtti = type_storage->find_class(context.asset_meta.rtti);
        if (!rtti) {
            WG_LOG_ERROR("no rtti type for " << asset_id);
            return StatusCode::InvalidData;
        }

        asset = rtti->instantiate().cast<Asset>();
        if (!asset) {
            WG_LOG_ERROR("failed to instantiate asset " << asset_id);
            return StatusCode::FailedInstantiate;
        }

        asset->set_id(asset_id);

        IoYamlTree asset_tree;
        WG_CHECKED(asset_tree.parse_data(result.get_data_file(FILE_TAG)));

        context.io_context.add<RttiTypeStorage*>(type_storage);
        context.io_context.add<AssetManager*>(context.ioc->resolve_value<AssetManager>());

        if (!asset->read_from_tree(context.io_context, asset_tree)) {
            WG_LOG_ERROR("failed to load asset from file " << asset_id);
            return StatusCode::FailedRead;
        }

        return WG_OK;
    }

}// namespace wmoge