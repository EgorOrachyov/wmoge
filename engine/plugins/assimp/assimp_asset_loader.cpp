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

#include "assimp_asset_loader.hpp"

#include "assimp_import_data.hpp"
#include "assimp_importer.hpp"
#include "core/data.hpp"
#include "core/ioc_container.hpp"
#include "math/math_utils3d.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_builder.hpp"
#include "mesh/mesh_manager.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"

#include <cassert>
#include <cstring>
#include <vector>

namespace wmoge {

    Status AssimpMeshAssetLoader::fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) {
        Ref<AssimpMeshImportData> import_data = context.asset_meta.import_data.cast<AssimpMeshImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << asset_id);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << asset_id);
            return StatusCode::InvalidData;
        }
        request.add_data_file(FILE_TAG, import_data->source_files[0].file);
        return WG_OK;
    }

    Status AssimpMeshAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Mesh>& asset) {
        WG_AUTO_PROFILE_ASSET("AssimpMeshAssetLoader::load_typed");

        Ref<AssimpMeshImportData> import_data = context.asset_meta.import_data.cast<AssimpMeshImportData>();
        assert(import_data);

        AssimpMeshImporter importer;
        if (!importer.read(asset_id.str(), result.get_data_file(FILE_TAG), import_data->process)) {
            WG_LOG_ERROR("failed to import mesh " << asset_id);
            return StatusCode::Error;
        }

        importer.set_attribs(import_data->attributes);
        if (!importer.process()) {
            WG_LOG_ERROR("failed to process mesh " << asset_id);
            return StatusCode::Error;
        }

        MeshManager* mesh_manager = context.ioc->resolve_value<MeshManager>();

        MeshFlags flags;
        flags.set(MeshFlag::FromDisk);

        asset = mesh_manager->create_mesh(flags);
        asset->set_id(asset_id);

        MeshBuilder& builder = importer.get_builder();
        builder.set_mesh(asset);
        if (!builder.build()) {
            WG_LOG_ERROR("failed to build mesh " << asset_id);
            return StatusCode::Error;
        }

        mesh_manager->init_mesh(asset.get());

        return WG_OK;
    }

}// namespace wmoge