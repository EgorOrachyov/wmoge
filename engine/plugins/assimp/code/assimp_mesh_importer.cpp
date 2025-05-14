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

#include "assimp_mesh_importer.hpp"

#include "assimp_processor.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_import_settings.hpp"
#include "mesh/mesh_loader.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status AssimpMeshImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"obj", "fbx", "gltf", "glTF", "3ds"};
        return WG_OK;
    }

    Status AssimpMeshImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        auto& preset    = presets.emplace_back();
        preset.name     = "default";
        preset.settings = make_ref<MeshImportSettings>();
        return WG_OK;
    }

    Status AssimpMeshImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status AssimpMeshImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("AssimpMeshImporter::import");

        Ref<MeshImportSettings> mesh_settings = settings.cast<MeshImportSettings>();
        if (!mesh_settings) {
            WG_LOG_ERROR("failed to get settings for " << path);
            return StatusCode::InvalidData;
        }

        std::vector<std::uint8_t> file;
        if (context.get_file_system()->read_file(path, file)) {
            WG_LOG_ERROR("failed to read file for " << path);
            return StatusCode::FailedRead;
        }

        AssimpProcessorMesh processor;
        if (!processor.read(path, file, mesh_settings->process)) {
            WG_LOG_ERROR("failed to read assimp mesh " << path);
            return StatusCode::FailedRead;
        }

        processor.set_attribs(mesh_settings->attributes);
        if (!processor.process()) {
            WG_LOG_ERROR("failed to process assimp mesh " << path);
            return StatusCode::FailedParse;
        }

        MeshBuilder& builder = processor.get_builder();
        if (!builder.build()) {
            WG_LOG_ERROR("failed to build mesh " << path);
            return StatusCode::Error;
        }

        MeshDesc& desc = builder.get_mesh();
        desc.flags.set(MeshFlag::Managed);
        desc.flags.set(MeshFlag::FromDisk);
        desc.name = path;

        Ref<Mesh> mesh = make_ref<Mesh>(std::move(desc));

        context.set_main_asset_simple(mesh, path, MeshLoader::get_class_static());
        return WG_OK;
    }

}// namespace wmoge