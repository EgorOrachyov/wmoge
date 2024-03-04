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

#pragma once

#include "core/array_view.hpp"
#include "core/data.hpp"
#include "core/fast_vector.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "io/serialization.hpp"
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

            WG_IO_DECLARE(Process);
        };

        std::string                source_file;
        std::vector<GfxVertAttrib> attributes;
        Process                    process{};

        WG_IO_DECLARE(MeshImportOptions);
    };

    /**
     * @class MeshChunk
     * @brief Represents single mesh chunk in a mesh which can be rendered individually with material
     */
    struct MeshChunk {
        Strid            name;
        Aabbf            aabb;
        GfxVertAttribs   attribs;
        GfxPrimType      prim_type          = GfxPrimType::Triangles;
        int              elem_count         = 0;
        int              vert_stream_offset = -1;
        int              vert_stream_count  = 0;
        int              index_stream       = -1;
        int              parent             = -1;
        fast_vector<int> children;

        WG_IO_DECLARE(MeshChunk);
    };

    /** 
     * @class MeshFile
     * @brief Struct used to serialize mesh resource data
     */
    struct MeshFile {
        fast_vector<MeshChunk>      chunks;
        fast_vector<Ref<Data>>      vertex_buffers;
        fast_vector<Ref<Data>>      index_buffers;
        fast_vector<GfxVertStream>  vert_streams;
        fast_vector<GfxIndexStream> index_streams;
        fast_vector<int>            roots;
        Aabbf                       aabb;

        WG_IO_DECLARE(MeshFile);
    };

    /**
     * @class Mesh
     * @brief Vertex and index data structured as chunks which can be rendered
     */
    class Mesh : public Resource {
    public:
        WG_OBJECT(Mesh, Resource);

        void add_chunk(const MeshChunk& mesh_chunk);
        void add_vertex_buffer(Ref<Data> buffer);
        void add_index_buffer(Ref<Data> buffer);
        void add_vert_stream(const GfxVertStream& stream);
        void add_intex_stream(const GfxIndexStream& stream);

        void update_aabb();
        void update_gfx_buffers();

        [[nodiscard]] GfxVertBuffersSetup get_vert_buffers_setup(int chunk_id) const;
        [[nodiscard]] GfxIndexBufferSetup get_index_buffer_setup(int chunk_id) const;

        [[nodiscard]] ArrayView<const MeshChunk> get_chunks() const;
        [[nodiscard]] const MeshChunk&           get_chunk(int i) const;
        [[nodiscard]] const Ref<GfxVertBuffer>&  get_gfx_vertex_buffers(int i) const;
        [[nodiscard]] const Ref<GfxIndexBuffer>& get_gfx_index_buffers(int i) const;
        [[nodiscard]] const GfxVertStream&       get_vert_streams(int i) const;
        [[nodiscard]] const GfxIndexStream&      get_index_streams(int i) const;
        [[nodiscard]] ArrayView<const int>       get_roots() const;
        [[nodiscard]] Aabbf                      get_aabb() const;

    private:
        fast_vector<MeshChunk>           m_chunks;
        fast_vector<Ref<GfxVertBuffer>>  m_gfx_vertex_buffers;
        fast_vector<Ref<Data>>           m_vertex_buffers;
        fast_vector<Ref<GfxIndexBuffer>> m_gfx_index_buffers;
        fast_vector<Ref<Data>>           m_index_buffers;
        fast_vector<GfxVertStream>       m_vert_streams;
        fast_vector<GfxIndexStream>      m_index_streams;
        fast_vector<int>                 m_roots;
        Aabbf                            m_aabb;
    };

}// namespace wmoge