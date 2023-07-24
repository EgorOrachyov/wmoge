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

#ifndef WMOGE_MESH_HPP
#define WMOGE_MESH_HPP

#include "core/data.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/aabb.hpp"
#include "math/vec.hpp"
#include "resource/resource.hpp"

#include <unordered_map>
#include <vector>

namespace wmoge {

    /**
     * @class MeshImportOptions
     * @brief Options to import mesh resource from external resource format
     */
    struct MeshImportOptions {
        /**
         * @class Process
         * @brief Controls post-process of imported mesh
         */
        struct Process {
            bool triangulate             = true;
            bool tangent_space           = false;
            bool flip_uv                 = true;
            bool gen_normals             = false;
            bool gen_smooth_normals      = false;
            bool join_identical_vertices = true;
            bool limit_bone_weights      = true;
            bool improve_cache_locality  = false;
            bool sort_by_ptype           = true;
            bool gen_uv                  = false;

            friend bool yaml_read(const YamlConstNodeRef& node, Process& process);
            friend bool yaml_write(YamlNodeRef node, const Process& process);
        };

        std::string                source_file;
        std::vector<GfxVertAttrib> attributes;
        Process                    process{};

        friend bool yaml_read(const YamlConstNodeRef& node, MeshImportOptions& options);
        friend bool yaml_write(YamlNodeRef node, const MeshImportOptions& options);
    };

    /**
     * @class MeshChunk
     * @brief Represents single mesh chunk in a mesh which can be rendered individually
     */
    struct MeshChunk {
        StringId name;
        Aabbf    aabb;
        int      vertex_offset{};
        int      index_offset{};
        int      index_count{};
    };

    /**
     * @class Mesh
     * @brief Vertex and index data structured as chunks which can be rendered
     */
    class Mesh : public Resource {
    public:
        static const int MAX_BUFFER = 3;

        WG_OBJECT(Mesh, Resource);

        void add_chunk(const MeshChunk& mesh_chunk);
        void set_vertex_params(int num_vertices, GfxPrimType prim_type);
        void set_vertex_buffer(int index, Ref<Data> buffer, GfxVertAttribs attribs);
        void set_index_buffer(Ref<Data> buffer, int num_indices, GfxIndexType index_type);

        void update_aabb();
        void update_gfx_buffers();

        const std::vector<MeshChunk>& get_chunks();
        const MeshChunk&              get_chunk(int i);
        const Ref<Data>&              get_vertex_buffer(int i);
        const Ref<Data>&              get_index_buffer();
        const Ref<GfxVertBuffer>&     get_gfx_vertex_buffer(int i);
        const Ref<GfxIndexBuffer>&    get_gfx_index_buffer();
        GfxIndexType                  get_index_type();
        GfxPrimType                   get_prim_type();
        GfxVertAttribsStreams         get_attribs();
        int                           get_num_vertices() const;
        int                           get_num_indices() const;
        Aabbf                         get_aabb();

        static Ref<Mesh> create_cube(const Vec3f& size);
        static Ref<Mesh> create_sphere(const Vec3f& size);

    private:
        std::vector<MeshChunk> m_chunks;
        Ref<GfxVertBuffer>     m_gfx_vertex_buffers[MAX_BUFFER];
        Ref<Data>              m_vertex_buffers[MAX_BUFFER];
        Ref<GfxIndexBuffer>    m_gfx_index_buffer;
        Ref<Data>              m_index_buffer;
        GfxIndexType           m_index_type;
        GfxPrimType            m_prim_type;
        GfxVertAttribsStreams  m_attribs;
        int                    m_num_vertices = 0;
        int                    m_num_indices  = 0;
        Aabbf                  m_aabb;
    };

}// namespace wmoge

#endif//WMOGE_MESH_HPP
