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
#include "resource/mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <magic_enum.hpp>

#include <cassert>
#include <vector>

namespace wmoge {

    struct AssimpImportContext {
        const StringId*            name    = nullptr;
        const ResourceMeta*        meta    = nullptr;
        const aiScene*             scene   = nullptr;
        unsigned int               options = 0;
        MeshAttribs                attribs;
        ref_ptr<Mesh>              mesh;
        std::vector<std::uint8_t>  buffers[Mesh::MAX_BUFFER];
        std::vector<std::uint32_t> indices;
        int                        total_vertices = 0;
        int                        total_indices  = 0;
    };

    bool ResourceLoaderAssimp::load(const StringId& name, const ResourceMeta& meta, ref_ptr<Resource>& res) {
        WG_AUTO_PROFILE_RESOURCE();

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("no import options file for " << name);
            return false;
        }

        auto params     = meta.import_options.value()["params"];
        auto process    = params["process"];
        auto attributes = params["attributes"];

        AssimpImportContext import_ctx;

        bool ai_triangulate             = true;
        bool ai_tangent_space           = false;
        bool ai_flip_uv                 = true;
        bool ai_gen_normals             = true;
        bool ai_gen_smooth_normals      = false;
        bool ai_join_identical_vertices = true;
        bool ai_limit_bone_weights      = true;
        bool ai_improve_cache_locality  = false;
        bool ai_sort_by_ptype           = true;
        bool ai_gen_uv                  = false;

        process["triangulate"] >> ai_triangulate;
        process["tangent_space"] >> ai_tangent_space;
        process["flip_uv"] >> ai_flip_uv;
        process["gen_normals"] >> ai_gen_normals;
        process["gen_smooth_normals"] >> ai_gen_smooth_normals;
        process["join_identical_vertices"] >> ai_join_identical_vertices;
        process["limit_bone_weights"] >> ai_limit_bone_weights;
        process["improve_cache_locality"] >> ai_improve_cache_locality;
        process["sort_by_ptype"] >> ai_sort_by_ptype;
        process["gen_uv"] >> ai_gen_uv;

        if (ai_triangulate) import_ctx.options |= aiProcess_Triangulate;
        if (ai_tangent_space) import_ctx.options |= aiProcess_CalcTangentSpace;
        if (ai_flip_uv) import_ctx.options |= aiProcess_FlipUVs;
        if (ai_gen_normals) import_ctx.options |= aiProcess_GenNormals;
        if (ai_gen_smooth_normals) import_ctx.options |= aiProcess_GenSmoothNormals;
        if (ai_join_identical_vertices) import_ctx.options |= aiProcess_JoinIdenticalVertices;
        if (ai_limit_bone_weights) import_ctx.options |= aiProcess_LimitBoneWeights;
        if (ai_improve_cache_locality) import_ctx.options |= aiProcess_ImproveCacheLocality;
        if (ai_sort_by_ptype) import_ctx.options |= aiProcess_SortByPType;
        if (ai_gen_uv) import_ctx.options |= aiProcess_GenUVCoords;

        import_ctx.options |= aiProcess_GenBoundingBoxes;

        for (auto attrib = attributes.first_child(); attrib.valid(); attrib = attrib.next_sibling()) {
            std::string attrib_name;
            attrib >> attrib_name;
            import_ctx.attribs.set(magic_enum::enum_cast<MeshAttrib>(attrib_name).value());
        }

        std::string source_file;
        params["source_file"] >> source_file;

        FileSystem* file_system = Engine::instance()->file_system();

        std::vector<std::uint8_t> file_data;
        if (!file_system->read_file(source_file, file_data)) {
            WG_LOG_ERROR("failed to load file " << source_file);
            return false;
        }

        Assimp::Importer importer;
        import_ctx.scene = importer.ReadFileFromMemory(file_data.data(), file_data.size(), import_ctx.options);
        if (!import_ctx.scene || !import_ctx.scene->mRootNode || import_ctx.scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
            WG_LOG_ERROR("failed load " << source_file << " log: " << importer.GetErrorString());
            return false;
        }

        import_ctx.meta = &meta;
        import_ctx.name = &name;
        import_ctx.mesh = Class::class_ptr(SID("Mesh"))->instantiate().cast<Mesh>();

        if (!import_ctx.mesh) {
            WG_LOG_ERROR("failed to instantiate mesh " << name);
            return false;
        }

        import_ctx.mesh->set_name(name);
        import_ctx.mesh->set_attribs(import_ctx.attribs);

        if (!process_node(import_ctx, import_ctx.scene->mRootNode, Math3d::identity(), Math3d::identity())) {
            WG_LOG_ERROR("failed to process scene of " << source_file);
            return false;
        }

        import_ctx.mesh->set_index_params(import_ctx.total_indices, GfxIndexType::Uint32);
        import_ctx.mesh->set_vertex_params(import_ctx.total_vertices, GfxPrimType::Triangles);
        import_ctx.mesh->set_index_buffer(make_ref<Data>(import_ctx.indices.data(), import_ctx.indices.size()));

        for (int i = 0; i < Mesh::MAX_BUFFER; i++) {
            if (!import_ctx.buffers[i].empty()) {
                import_ctx.mesh->set_vertex_buffer(i, make_ref<Data>(import_ctx.buffers[i].data(), import_ctx.buffers[i].size()));
            }
        }

        import_ctx.mesh->update_aabb();
        import_ctx.mesh->update_gfx_buffers();

        res = import_ctx.mesh.as<Resource>();
        return true;
    }
    bool ResourceLoaderAssimp::can_load(const StringId& resource_type) {
        return resource_type == SID("Mesh");
    }
    StringId ResourceLoaderAssimp::get_name() {
        return SID("assimp");
    }
    bool ResourceLoaderAssimp::process_node(AssimpImportContext& context, aiNode* node, const Mat4x4f& parent_transform, const Mat4x4f& inv_parent_transform) {
        WG_AUTO_PROFILE_RESOURCE();

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
        WG_AUTO_PROFILE_RESOURCE();

        std::size_t buffers_sizes[Mesh::MAX_BUFFER];
        int         buffers_strides[Mesh::MAX_BUFFER];
        int         attrib_offsets[Mesh::MAX_ATTRIB];
        std::size_t indices_size;
        auto&       buffers      = context.buffers;
        auto&       indices      = context.indices;
        const auto  num_vertices = mesh->mNumVertices;
        const auto  num_faces    = mesh->mNumFaces;

        const int buff_pos     = Mesh::get_attrib_buffer(MeshAttrib::Position);
        const int buff_norm    = Mesh::get_attrib_buffer(MeshAttrib::Normal);
        const int buff_tangent = Mesh::get_attrib_buffer(MeshAttrib::Tangent);
        const int buff_uv      = Mesh::get_attrib_buffer(MeshAttrib::Uv0);

        const int id_pos     = static_cast<int>(MeshAttrib::Position);
        const int id_norm    = static_cast<int>(MeshAttrib::Normal);
        const int id_tangent = static_cast<int>(MeshAttrib::Tangent);
        const int id_uv      = static_cast<int>(MeshAttrib::Uv0);

        for (int i = 0; i < Mesh::MAX_BUFFER; i++) {
            buffers_strides[i] = context.mesh->get_buffer_stride(i);
            buffers_sizes[i]   = buffers[i].size();
            buffers[i].resize(buffers_sizes[i] + buffers_strides[i] * num_vertices, 0);
        }
        for (int i = 0; i < Mesh::MAX_ATTRIB; i++) {
            attrib_offsets[i] = context.mesh->get_attrib_offset(static_cast<MeshAttrib>(i));
        }

        indices_size = indices.size();
        indices.resize(indices_size + 3 * num_faces);

        for (unsigned int vert_id = 0; vert_id < num_vertices; vert_id++) {
            if (context.attribs.get(MeshAttrib::Position) && mesh->HasPositions()) {
                auto pos = reinterpret_cast<Vec3f*>(buffers[buff_pos].data() +
                                                    buffers_sizes[buff_pos] +
                                                    vert_id * buffers_strides[buff_pos] +
                                                    attrib_offsets[id_pos]);

                pos->data()[0] = mesh->mVertices[vert_id].x;
                pos->data()[1] = mesh->mVertices[vert_id].y;
                pos->data()[2] = mesh->mVertices[vert_id].z;
                *pos           = Math3d::transform(transform, *pos);
            }
            if (context.attribs.get(MeshAttrib::Normal) && mesh->HasNormals()) {
                auto norm = reinterpret_cast<Vec3f*>(buffers[buff_norm].data() +
                                                     buffers_sizes[buff_norm] +
                                                     vert_id * buffers_strides[buff_norm] +
                                                     attrib_offsets[id_norm]);

                norm->data()[0] = mesh->mNormals[vert_id].x;
                norm->data()[1] = mesh->mNormals[vert_id].y;
                norm->data()[2] = mesh->mNormals[vert_id].z;
                *norm           = Math3d::transform_w0(inv_transform, *norm);
            }
            if (context.attribs.get(MeshAttrib::Tangent) && mesh->HasTangentsAndBitangents()) {
                auto tang = reinterpret_cast<Vec3f*>(buffers[buff_tangent].data() +
                                                     buffers_sizes[buff_tangent] +
                                                     vert_id * buffers_strides[buff_tangent] +
                                                     attrib_offsets[id_tangent]);

                tang->data()[0] = mesh->mTangents[vert_id].x;
                tang->data()[1] = mesh->mTangents[vert_id].y;
                tang->data()[2] = mesh->mTangents[vert_id].z;
                *tang           = Math3d::transform_w0(inv_transform, *tang);
            }
            for (int i = 0; i < 4; i++) {
                if (context.attribs.get(static_cast<MeshAttrib>(id_uv + i)) && mesh->HasTextureCoords(i)) {
                    auto uv = reinterpret_cast<Vec2f*>(buffers[buff_uv].data() +
                                                       buffers_sizes[buff_uv] +
                                                       vert_id * buffers_strides[buff_uv] +
                                                       attrib_offsets[id_uv + i]);

                    uv->data()[0] = mesh->mTextureCoords[i][vert_id].x;
                    uv->data()[1] = mesh->mTextureCoords[i][vert_id].y;
                }
            }
        }

        for (unsigned int face_id = 0; face_id < num_faces; face_id++) {
            aiFace face = mesh->mFaces[face_id];
            assert(face.mNumIndices == 3);
            for (unsigned int i = 0; i < face.mNumIndices; i++) {
                indices[indices_size + 3 * face_id + i] = face.mIndices[i];
            }
        }

        Vec3f aabb_min = Vec3f(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
        Vec3f aabb_max = Vec3f(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);

        MeshChunk chunk;
        chunk.name          = SID(std::string(mesh->mName.data, mesh->mName.length));
        chunk.index_count   = 3 * num_faces;
        chunk.vertex_offset = context.total_vertices;
        chunk.index_offset  = context.total_indices;
        chunk.aabb          = Aabbf((aabb_min + aabb_max) * 0.5f, (aabb_max - aabb_min) * 0.5f);
        context.mesh->add_chunk(chunk);

        context.total_vertices += num_vertices;
        context.total_indices += 3 * num_faces;

        return true;
    }

}// namespace wmoge