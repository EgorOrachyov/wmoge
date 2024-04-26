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
#include "math/math_utils3d.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_builder.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <cassert>
#include <cstring>
#include <vector>

namespace wmoge {

    Status AssimpMeshAssetLoader::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& asset) {
        WG_AUTO_PROFILE_ASSET("AssimpMeshAssetLoader::load");

        Ref<AssimpMeshImportData> import_data = meta.import_data.cast<AssimpMeshImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << name);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << name);
            return StatusCode::InvalidData;
        }

        FileSystem* file_system = IocContainer::instance()->resolve_v<FileSystem>();
        std::string file_name   = import_data->source_files[0].file;

        std::vector<std::uint8_t> file_data;
        if (!file_system->read_file(file_name, file_data)) {
            WG_LOG_ERROR("failed to load file " << file_name);
            return StatusCode::FailedRead;
        }

        AssimpMeshImporter importer;
        if (!importer.read(file_name, file_data, import_data->process)) {
            WG_LOG_ERROR("failed to import file " << file_name);
            return StatusCode::Error;
        }

        importer.set_attribs(import_data->attributes);
        if (!importer.process()) {
            WG_LOG_ERROR("failed to process file " << file_name);
            return StatusCode::Error;
        }

        Ref<Mesh> mesh = make_ref<Mesh>();

        asset = mesh;
        asset->set_name(name);
        asset->set_import_data(meta.import_data);

        MeshBuilder& builder = importer.get_builder();
        builder.set_mesh(mesh);
        if (!builder.build()) {
            WG_LOG_ERROR("failed to build mesh " << file_name);
            return StatusCode::Error;
        }

        return WG_OK;
    }

}// namespace wmoge