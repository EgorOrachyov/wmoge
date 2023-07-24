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

#include "resource_loader_assimp.hpp"

#include "core/data.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "math/math_utils3d.hpp"
#include "platform/file_system.hpp"
#include "render/mesh_builder.hpp"
#include "resource/mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <magic_enum.hpp>

#include <cassert>
#include <cstring>
#include <vector>

namespace wmoge {

    struct AssimpImportContext {
        const StringId*     name    = nullptr;
        const ResourceMeta* meta    = nullptr;
        const aiScene*      scene   = nullptr;
        unsigned int        options = 0;
        GfxVertAttribs      attribs;
        MeshBuilder         builder;
    };

    bool ResourceLoaderAssimp::load(const StringId& name, const ResourceMeta& meta, Ref<Resource>& res) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderAssimp::load");

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("no import options file for " << name);
            return false;
        }

        MeshImportOptions options;
        WG_YAML_READ_AS(meta.import_options->crootref(), "params", options);

        AssimpImportContext import_ctx;

        if (options.process.triangulate) import_ctx.options |= aiProcess_Triangulate;
        if (options.process.tangent_space) import_ctx.options |= aiProcess_CalcTangentSpace;
        if (options.process.flip_uv) import_ctx.options |= aiProcess_FlipUVs;
        if (options.process.gen_normals) import_ctx.options |= aiProcess_GenNormals;
        if (options.process.gen_smooth_normals) import_ctx.options |= aiProcess_GenSmoothNormals;
        if (options.process.join_identical_vertices) import_ctx.options |= aiProcess_JoinIdenticalVertices;
        if (options.process.limit_bone_weights) import_ctx.options |= aiProcess_LimitBoneWeights;
        if (options.process.improve_cache_locality) import_ctx.options |= aiProcess_ImproveCacheLocality;
        if (options.process.sort_by_ptype) import_ctx.options |= aiProcess_SortByPType;
        if (options.process.gen_uv) import_ctx.options |= aiProcess_GenUVCoords;

        import_ctx.options |= aiProcess_GenBoundingBoxes;

        for (auto attrib : options.attributes) {
            import_ctx.attribs.set(attrib);
        }

        FileSystem* file_system = Engine::instance()->file_system();

        std::vector<std::uint8_t> file_data;
        if (!file_system->read_file(options.source_file, file_data)) {
            WG_LOG_ERROR("failed to load file " << options.source_file);
            return false;
        }

        Assimp::Importer importer;

        import_ctx.scene = importer.ReadFileFromMemory(file_data.data(), file_data.size(), import_ctx.options);
        if (!import_ctx.scene || !import_ctx.scene->mRootNode || import_ctx.scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            WG_LOG_ERROR("failed load " << options.source_file << " log: " << importer.GetErrorString());
            return false;
        }

        import_ctx.meta = &meta;
        import_ctx.name = &name;

        if (!process_node(import_ctx, import_ctx.scene->mRootNode, Math3d::identity(), Math3d::identity())) {
            WG_LOG_ERROR("failed to process scene of " << options.source_file);
            return false;
        }

        if (!import_ctx.builder.build()) {
            WG_LOG_ERROR("failed to build mesh " << name);
            return false;
        }

        res = import_ctx.builder.mesh;
        res->set_name(name);

        return true;
    }
    StringId ResourceLoaderAssimp::get_name() {
        return SID("assimp");
    }
    bool ResourceLoaderAssimp::process_node(AssimpImportContext& context, aiNode* node, const Mat4x4f& parent_transform, const Mat4x4f& inv_parent_transform) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderAssimp::process_node");

        Mat4x4f local_transform;
        Mat4x4f inv_local_transform;

        aiMatrix4x4 node_transform = node->mTransformation;
        std::copy(node_transform[0], node_transform[0] + 16, local_transform.values);
        node_transform.Inverse();
        std::copy(node_transform[0], node_transform[0] + 16, inv_local_transform.values);

        Mat4x4f global_transform     = parent_transform * local_transform;
        Mat4x4f inv_global_transform = inv_local_transform * inv_parent_transform;

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = context.scene->mMeshes[node->mMeshes[i]];
            process_mesh(context, mesh, global_transform, inv_global_transform);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            if (!process_node(context, node->mChildren[i], global_transform, inv_global_transform)) {
                return false;
            }
        }

        return true;
    }
    bool ResourceLoaderAssimp::process_mesh(struct AssimpImportContext& context, aiMesh* mesh, const Mat4x4f& transform, const Mat4x4f& inv_transform) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderAssimp::process_mesh");

        const Vec3f aabb_min     = Vec3f(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
        const Vec3f aabb_max     = Vec3f(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
        const auto  num_vertices = mesh->mNumVertices;
        const auto  num_faces    = mesh->mNumFaces;

        const GfxVertAttribs attribs = context.attribs;
        MeshBuilder&         builder = context.builder;

        MeshChunk chunk;
        chunk.name          = SID(std::string(mesh->mName.data, mesh->mName.length));
        chunk.index_count   = int(3 * num_faces);
        chunk.vertex_offset = builder.num_vertices;
        chunk.index_offset  = builder.num_indices;
        chunk.aabb          = Aabbf((aabb_min + aabb_max) * 0.5f, (aabb_max - aabb_min) * 0.5f);

        builder.chunks.push_back(chunk);
        builder.num_vertices += int(num_vertices);

        for (unsigned int vert_id = 0; vert_id < num_vertices; vert_id++) {
            if (attribs.get(GfxVertAttrib::Pos3f) && mesh->HasPositions()) {
                Vec3f pos;
                pos[0] = mesh->mVertices[vert_id].x;
                pos[1] = mesh->mVertices[vert_id].y;
                pos[2] = mesh->mVertices[vert_id].z;
                pos    = Math3d::transform(transform, pos);
                builder.pos3.push_back(pos);
            }
            if (attribs.get(GfxVertAttrib::Norm3f) && mesh->HasNormals()) {
                Vec3f norm;
                norm[0] = mesh->mNormals[vert_id].x;
                norm[1] = mesh->mNormals[vert_id].y;
                norm[2] = mesh->mNormals[vert_id].z;
                norm    = Math3d::transform_w0(inv_transform, norm);
                builder.norm.push_back(norm);
            }
            if (attribs.get(GfxVertAttrib::Tang3f) && mesh->HasTangentsAndBitangents()) {
                Vec3f tang;
                tang[0] = mesh->mTangents[vert_id].x;
                tang[1] = mesh->mTangents[vert_id].y;
                tang[2] = mesh->mTangents[vert_id].z;
                tang    = Math3d::transform_w0(inv_transform, tang);
                builder.tang.push_back(tang);
            }
            for (int i = 0; i < 4; i++) {
                if (attribs.get(static_cast<GfxVertAttrib>(int(GfxVertAttrib::Uv02f) + i)) && mesh->HasTextureCoords(i)) {
                    Vec2f uv;
                    uv[0] = mesh->mTextureCoords[i][vert_id].x;
                    uv[1] = mesh->mTextureCoords[i][vert_id].y;
                    builder.uv[i].push_back(uv);
                }
            }
        }

        for (unsigned int face_id = 0; face_id < num_faces; face_id++) {
            aiFace face = mesh->mFaces[face_id];
            assert(face.mNumIndices == 3);
            for (unsigned int i = 0; i < face.mNumIndices; i++) {
                builder.add_index(face.mIndices[i]);
            }
        }

        return true;
    }

}// namespace wmoge