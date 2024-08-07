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

#include "profiler/profiler.hpp"

namespace wmoge {

    Status DefaultAssetLoader::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& asset) {
        WG_AUTO_PROFILE_ASSET("DefaultAssetLoader::load");

        Ref<AssetImportData> import_data = meta.import_data.cast<AssetImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data to load " << name);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << name);
            return StatusCode::InvalidData;
        }

        std::string path_on_disk = import_data->source_files[0].file;
        if (path_on_disk.empty()) {
            WG_LOG_ERROR("no path on disk to load asset file " << name);
            return StatusCode::InvalidData;
        }

        asset = meta.rtti->instantiate().cast<Asset>();
        if (!asset) {
            WG_LOG_ERROR("failed to instantiate asset " << name);
            return StatusCode::FailedInstantiate;
        }

        auto asset_tree = yaml_parse_file(path_on_disk);
        if (asset_tree.empty()) {
            WG_LOG_ERROR("failed to read parse file " << path_on_disk);
            return StatusCode::FailedParse;
        };

        asset->set_name(name);
        asset->set_import_data(meta.import_data);

        IoContext context;

        if (!asset->read_from_yaml(context, asset_tree.crootref())) {
            WG_LOG_ERROR("failed to load asset from file " << path_on_disk);
            return StatusCode::FailedRead;
        }

        return WG_OK;
    }

}// namespace wmoge