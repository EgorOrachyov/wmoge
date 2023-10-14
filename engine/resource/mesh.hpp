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

#include "core/array_view.hpp"
#include "core/data.hpp"
#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "io/yaml.hpp"
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

            friend Status yaml_read(const YamlConstNodeRef& node, Process& process);
            friend Status yaml_write(YamlNodeRef node, const Process& process);
        };

        std::string                source_file;
        std::vector<GfxVertAttrib> attributes;
        Process                    process{};

        friend Status yaml_read(const YamlConstNodeRef& node, MeshImportOptions& options);
        friend Status yaml_write(YamlNodeRef node, const MeshImportOptions& options);
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

        friend Status yaml_read(const YamlConstNodeRef& node, MeshChunk& chunk);
        friend Status yaml_write(YamlNodeRef node, const MeshChunk& chunk);
    };

    /** 
     * @class MeshFile
     * @brief Struct used to serialize mesh resource data
     */
    struct MeshFile {
        static const int MAX_BUFFER = 3;

        std::vector<MeshChunk>            chunks;
        std::array<Ref<Data>, MAX_BUFFER> vertex_buffers;
        Ref<Data>                         index_buffer;
        GfxIndexType                      index_type;
        GfxPrimType                       prim_type;
        GfxVertAttribsStreams             attribs;
        int                               num_vertices = 0;
        int                               num_indices  = 0;
        Aabbf                             aabb;

        friend Status yaml_read(const YamlConstNodeRef& node, MeshFile& file);
        friend Status yaml_write(YamlNodeRef node, const MeshFile& file);
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

        [[nodiscard]] ArrayView<const MeshChunk> get_chunks() const;
        [[nodiscard]] const MeshChunk&           get_chunk(int i) const;
        [[nodiscard]] const Ref<Data>&           get_vertex_buffer(int i) const;
        [[nodiscard]] const Ref<Data>&           get_index_buffer() const;
        [[nodiscard]] const Ref<GfxVertBuffer>&  get_gfx_vertex_buffer(int i) const;
        [[nodiscard]] const Ref<GfxIndexBuffer>& get_gfx_index_buffer() const;
        [[nodiscard]] GfxIndexType               get_index_type() const;
        [[nodiscard]] GfxPrimType                get_prim_type() const;
        [[nodiscard]] GfxVertAttribsStreams      get_attribs() const;
        [[nodiscard]] int                        get_num_vertices() const;
        [[nodiscard]] int                        get_num_indices() const;
        [[nodiscard]] Aabbf                      get_aabb() const;
        [[nodiscard]] GfxVertBuffersSetup        get_gfx_vert_buffes_setup() const;

    private:
        std::vector<MeshChunk>                     m_chunks;
        std::array<Ref<GfxVertBuffer>, MAX_BUFFER> m_gfx_vertex_buffers;
        std::array<Ref<Data>, MAX_BUFFER>          m_vertex_buffers;
        Ref<GfxIndexBuffer>                        m_gfx_index_buffer;
        Ref<Data>                                  m_index_buffer;
        GfxIndexType                               m_index_type;
        GfxPrimType                                m_prim_type;
        GfxVertAttribsStreams                      m_attribs;
        int                                        m_num_vertices = 0;
        int                                        m_num_indices  = 0;
        Aabbf                                      m_aabb;
    };

}// namespace wmoge

#endif//WMOGE_MESH_HPP
