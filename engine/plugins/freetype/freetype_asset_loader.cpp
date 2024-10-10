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

#include "freetype_asset_loader.hpp"

#include "core/ioc_container.hpp"
#include "freetype_font.hpp"
#include "freetype_import_data.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status FreetypeAssetLoader::fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) {
        Ref<FreetypeImportData> import_data = context.asset_meta.import_data.cast<FreetypeImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no valid import data for " << asset_id);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source files for " << asset_id);
            return StatusCode::InvalidData;
        }
        request.add_data_file(FILE_TAG, import_data->source_files[0].file);
        return WG_OK;
    }

    Status FreetypeAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Font>& asset) {
        WG_PROFILE_CPU_ASSET("FreetypeAssetLoader::load_typed");

        Ref<FreetypeImportData> import_data = context.asset_meta.import_data.cast<FreetypeImportData>();
        assert(import_data);

        asset = make_ref<Font>();
        asset->set_id(asset_id);

        FreetypeFont loader(context.ioc);
        return loader.load(asset, result.get_data_file(FILE_TAG), import_data->height, import_data->glyphs_in_row);
    }

}// namespace wmoge