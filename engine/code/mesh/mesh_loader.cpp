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

#include "mesh_loader.hpp"

#include "core/ioc_container.hpp"
#include "mesh/mesh.hpp"
#include "mesh/mesh_manager.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status MeshLoader::load(AssetLoadContext& context, const UUID& asset_id, Ref<Asset>& asset) {
        WG_PROFILE_CPU_MESH("MeshLoader::load");

        auto artifacts = context.get_artifacts();
        if (artifacts.empty()) {
            WG_LOG_ERROR("no artifacts to load " << context.get_name());
            return StatusCode::InvalidState;
        }
        auto artifact = artifacts[0];
        if (!artifact) {
            WG_LOG_ERROR("null artifact at [0] to load " << context.get_name());
            return StatusCode::InvalidState;
        }
        Ref<Mesh> mesh = artifact.cast<Mesh>();
        if (!mesh) {
            WG_LOG_ERROR("invalid artifact type to load " << context.get_name());
            return StatusCode::InvalidState;
        }

        MeshManager* mesh_manager = context.get_ioc_container()->resolve_value<MeshManager>();

        mesh_manager->add_mesh(mesh);
        mesh_manager->queue_mesh_upload(mesh.get());
        asset = mesh;
        return WG_OK;
    }

}// namespace wmoge