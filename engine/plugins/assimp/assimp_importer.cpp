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

#include "assimp_importer.hpp"

#include "core/data.hpp"
#include "core/log.hpp"
#include "math/math_utils3d.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cassert>
#include <cstring>
#include <vector>

namespace wmoge {

    Status AssimpImporter::read(std::string file_name, array_view<const std::uint8_t> data, const AssimpProcess& flags) {
        WG_PROFILE_CPU_ASSET("AssimpImporter::read");

        if (flags.triangulate) m_options |= aiProcess_Triangulate;
        if (flags.tangent_space) m_options |= aiProcess_CalcTangentSpace;
        if (flags.flip_uv) m_options |= aiProcess_FlipUVs;
        if (flags.gen_normals) m_options |= aiProcess_GenNormals;
        if (flags.gen_smooth_normals) m_options |= aiProcess_GenSmoothNormals;
        if (flags.join_identical_vertices) m_options |= aiProcess_JoinIdenticalVertices;
        if (flags.limit_bone_weights) m_options |= aiProcess_LimitBoneWeights;
        if (flags.improve_cache_locality) m_options |= aiProcess_ImproveCacheLocality;
        if (flags.sort_by_ptype) m_options |= aiProcess_SortByPType;
        if (flags.gen_uv) m_options |= aiProcess_GenUVCoords;

        m_options |= aiProcess_GenBoundingBoxes;

        m_file_name = std::move(file_name);

        m_scene = m_importer.ReadFileFromMemory(data.data(), data.size(), m_options);
        if (!m_scene || !m_scene->mRootNode || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            WG_LOG_ERROR("failed load " << m_file_name << " log: " << m_importer.GetErrorString());
            return StatusCode::FailedParse;
        }

        return WG_OK;
    }

    Status AssimpImporter::process() {
        if (!process_node(m_scene->mRootNode, Math3d::identity(), Math3d::identity(), std::nullopt)) {
            WG_LOG_ERROR("failed to process scene of " << m_file_name);
            return StatusCode::Error;
        }

        return WG_OK;
    }

    Status AssimpImporter::process_node(aiNode* node, const Mat4x4f& parent_transform, const Mat4x4f& inv_parent_transform, std::optional<int> parent) {
        WG_PROFILE_CPU_ASSET("AssimpImporter::process_node");

        Mat4x4f local_transform;
        Mat4x4f inv_local_transform;

        const aiMatrix4x4 node_transform = node->mTransformation;
        std::copy(node_transform[0], node_transform[0] + 16, local_transform.values);

        const aiMatrix4x4 inv_node_transform = node->mTransformation.Inverse();
        std::copy(inv_node_transform[0], inv_node_transform[0] + 16, inv_local_transform.values);

        const Mat4x4f global_transform     = parent_transform * local_transform;
        const Mat4x4f inv_global_transform = inv_local_transform * inv_parent_transform;

        std::optional<int> mesh_id;

        if (node->mNumMeshes > 1) {
            WG_LOG_ERROR("More than 1 mesh in a single node, check asset " << m_file_name);
            return StatusCode::InvalidData;
        }

        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = m_scene->mMeshes[node->mMeshes[i]];
            if (!process_mesh(mesh, global_transform, inv_global_transform, parent)) {
                return StatusCode::Error;
            }
            mesh_id = m_next_mesh_id;
            m_next_mesh_id += 1;
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            if (!process_node(node->mChildren[i], global_transform, inv_global_transform, mesh_id)) {
                return StatusCode::Error;
            }
        }

        return WG_OK;
    }

    Status AssimpMeshImporter::process_mesh(aiMesh* mesh, const Mat4x4f& transform, const Mat4x4f& inv_transform, std::optional<int> parent) {
        WG_PROFILE_CPU_ASSET("AssimpMeshImporter::process_mesh");

        const Vec3f aabb_min     = Vec3f(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
        const Vec3f aabb_max     = Vec3f(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
        const auto  num_vertices = mesh->mNumVertices;
        const auto  num_faces    = mesh->mNumFaces;

        const auto name = SID(std::string(mesh->mName.data, mesh->mName.length));
        const auto aabb = Aabbf((aabb_min + aabb_max) * 0.5f, (aabb_max - aabb_min) * 0.5f);

        const GfxVertAttribs attribs = m_attribs;

        Ref<ArrayMesh> array_mesh = make_ref<ArrayMesh>();
        array_mesh->set_id(SID(get_file_name() + "." + name.str()));
        array_mesh->set_aabb(aabb);

        for (unsigned int vert_id = 0; vert_id < num_vertices; vert_id++) {
            MeshVertex vertex;

            if (attribs.get(GfxVertAttrib::Pos3f) && mesh->HasPositions()) {
                Vec3f pos;
                pos[0]      = mesh->mVertices[vert_id].x;
                pos[1]      = mesh->mVertices[vert_id].y;
                pos[2]      = mesh->mVertices[vert_id].z;
                vertex.pos3 = Math3d::transform(transform, pos);
                vertex.attribs.set(GfxVertAttrib::Pos3f);
            }
            if (attribs.get(GfxVertAttrib::Norm3f) && mesh->HasNormals()) {
                Vec3f norm;
                norm[0]     = mesh->mNormals[vert_id].x;
                norm[1]     = mesh->mNormals[vert_id].y;
                norm[2]     = mesh->mNormals[vert_id].z;
                vertex.norm = Math3d::transform_w0(inv_transform, norm);
                vertex.attribs.set(GfxVertAttrib::Norm3f);
            }
            if (attribs.get(GfxVertAttrib::Tang3f) && mesh->HasTangentsAndBitangents()) {
                Vec3f tang;
                tang[0]     = mesh->mTangents[vert_id].x;
                tang[1]     = mesh->mTangents[vert_id].y;
                tang[2]     = mesh->mTangents[vert_id].z;
                vertex.tang = Math3d::transform_w0(inv_transform, tang);
                vertex.attribs.set(GfxVertAttrib::Tang3f);
            }
            for (int i = 0; i < 4; i++) {
                if (attribs.get(static_cast<GfxVertAttrib>(int(GfxVertAttrib::Uv02f) + i)) && mesh->HasTextureCoords(i)) {
                    GfxVertAttrib uv_ids[] = {GfxVertAttrib::Uv02f, GfxVertAttrib::Uv12f, GfxVertAttrib::Uv22f, GfxVertAttrib::Uv32f};
                    Vec2f         uv;
                    uv[0]        = mesh->mTextureCoords[i][vert_id].x;
                    uv[1]        = mesh->mTextureCoords[i][vert_id].y;
                    vertex.uv[i] = uv;
                    vertex.attribs.set(uv_ids[i]);
                }
            }

            array_mesh->set_attribs(vertex.attribs);
            array_mesh->add_vertex(vertex);
        }

        for (unsigned int face_id = 0; face_id < num_faces; face_id++) {
            aiFace face = mesh->mFaces[face_id];
            assert(face.mNumIndices == 3);
            array_mesh->add_face(MeshFace(face.mIndices[0], face.mIndices[1], face.mIndices[2]));
        }

        m_builder.add_chunk(name, array_mesh);

        return WG_OK;
    }

}// namespace wmoge