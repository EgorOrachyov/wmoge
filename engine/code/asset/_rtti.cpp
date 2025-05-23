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

#include "_rtti.hpp"

#include "asset/asset.hpp"
#include "asset/asset_artifact.hpp"
#include "asset/asset_data.hpp"
#include "asset/asset_import_env.hpp"
#include "asset/asset_import_settings.hpp"
#include "asset/asset_importer.hpp"
#include "asset/asset_loader.hpp"
#include "asset/asset_manifest.hpp"
#include "asset/asset_meta_data.hpp"
#include "asset/asset_native_loader.hpp"

namespace wmoge {

    void rtti_asset() {
        rtti_type<Asset>();
        rtti_type<AssetArtifact>();
        rtti_type<AssetSource>();
        rtti_type<AssetData>();
        rtti_type<AssetManifest>();
        rtti_type<AssetMetaData>();
        rtti_type<AssetImporter>();
        rtti_type<AssetImportError>();
        rtti_type<AssetImportEnv>();
        rtti_type<AssetImportSettings>();
        rtti_type<AssetImportPreset>();
        rtti_type<AssetLoader>();
        rtti_type<AssetNativeLoader>();
    }

}// namespace wmoge