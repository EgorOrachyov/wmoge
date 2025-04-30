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

#include "asset/asset.hpp"
#include "core/array_view.hpp"
#include "core/data.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/aabb.hpp"
#include "math/vec.hpp"
#include "mesh/mesh_array.hpp"
#include "rtti/traits.hpp"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace wmoge {

    /** @brief Flag assigned to mesh asset */
    enum class MeshFlag {
        Managed = 0,
        Streamed,
        FromDisk
    };

    /** @brief Flags assigned to mesh asset */
    using MeshFlags = Mask<MeshFlag>;

    /**
     * @class MeshChunk
     * @brief Represents single mesh chunk in a mesh which can be rendered individually with material
     */
    struct MeshChunk {
        WG_RTTI_STRUCT(MeshChunk);

        Strid          name;
        Aabbf          aabb;
        GfxVertAttribs attribs;
        GfxPrimType    prim_type          = GfxPrimType::Triangles;
        int            elem_count         = 0;
        int            vert_stream_offset = -1;
        int            vert_stream_count  = 0;
        int            index_stream       = -1;
    };

    WG_RTTI_STRUCT_BEGIN(MeshChunk) {
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(aabb, {});
        WG_RTTI_FIELD(attribs, {});
        WG_RTTI_FIELD(prim_type, {});
        WG_RTTI_FIELD(elem_count, {});
        WG_RTTI_FIELD(vert_stream_offset, {});
        WG_RTTI_FIELD(vert_stream_count, {});
        WG_RTTI_FIELD(index_stream, {});
    }
    WG_RTTI_END;

    /** 
     * @class MeshDesc
     * @brief Struct used to serialize mesh asset data
     */
    struct MeshDesc {
        WG_RTTI_STRUCT(MeshDesc);

        std::vector<MeshChunk>       chunks;
        std::vector<Ref<MeshArray>>  array_meshes;
        std::vector<Ref<Data>>       vertex_buffers;
        std::vector<Ref<Data>>       index_buffers;
        std::vector<MeshVertStream>  vert_streams;
        std::vector<MeshIndexStream> index_streams;
        Aabbf                        aabb;
        MeshFlags                    flags;
    };

    WG_RTTI_STRUCT_BEGIN(MeshDesc) {
        WG_RTTI_FIELD(chunks, {});
        WG_RTTI_FIELD(array_meshes, {});
        WG_RTTI_FIELD(vertex_buffers, {});
        WG_RTTI_FIELD(index_buffers, {});
        WG_RTTI_FIELD(vert_streams, {});
        WG_RTTI_FIELD(index_streams, {});
        WG_RTTI_FIELD(aabb, {});
        WG_RTTI_FIELD(flags, {});
    }
    WG_RTTI_END;

    /**
     * @class Mesh
     * @brief Vertex and index data structured as chunks which can be rendered
     */
    class Mesh : public Asset {
    public:
        WG_RTTI_CLASS(Mesh, Asset);

        using Callback    = std::function<void(Mesh*)>;
        using CallbackRef = std::shared_ptr<Callback>;

        Mesh() = default;
        ~Mesh() override;

        Mesh(MeshDesc& mesh_desc);

        void set_mesh_callback(CallbackRef callback);
        void set_gfx_vertex_buffers(std::vector<Ref<GfxVertBuffer>> gfx_vertex_buffers);
        void set_gfx_index_buffers(std::vector<Ref<GfxIndexBuffer>> gfx_index_buffers);
        void release_gfx_buffers();

        [[nodiscard]] GfxVertBuffersSetup                   get_vert_buffers_setup(int chunk_id) const;
        [[nodiscard]] GfxIndexBufferSetup                   get_index_buffer_setup(int chunk_id) const;
        [[nodiscard]] array_view<const MeshChunk>           get_chunks() const;
        [[nodiscard]] array_view<const Ref<MeshArray>>      get_array_meshes() const;
        [[nodiscard]] array_view<const Ref<Data>>           get_vertex_buffers() const;
        [[nodiscard]] array_view<const Ref<Data>>           get_index_buffers() const;
        [[nodiscard]] array_view<const Ref<GfxVertBuffer>>  get_gfx_vertex_buffers() const;
        [[nodiscard]] array_view<const Ref<GfxIndexBuffer>> get_gfx_index_buffers() const;
        [[nodiscard]] const MeshChunk&                      get_chunk(int i) const;
        [[nodiscard]] const Ref<GfxVertBuffer>&             get_gfx_vertex_buffer(int i) const;
        [[nodiscard]] const Ref<GfxIndexBuffer>&            get_gfx_index_buffer(int i) const;
        [[nodiscard]] const MeshVertStream&                 get_vert_stream(int i) const;
        [[nodiscard]] const MeshIndexStream&                get_index_stream(int i) const;
        [[nodiscard]] const Aabbf&                          get_aabb() const;
        [[nodiscard]] const MeshFlags&                      get_flags() const;
        [[nodiscard]] GfxMemUsage                           get_mem_usage() const;

    private:
        std::vector<MeshChunk>           m_chunks;
        std::vector<Ref<MeshArray>>      m_array_meshes;
        std::vector<Ref<GfxVertBuffer>>  m_gfx_vertex_buffers;
        std::vector<Ref<Data>>           m_vertex_buffers;
        std::vector<Ref<GfxIndexBuffer>> m_gfx_index_buffers;
        std::vector<Ref<Data>>           m_index_buffers;
        std::vector<MeshVertStream>      m_vert_streams;
        std::vector<MeshIndexStream>     m_index_streams;
        Aabbf                            m_aabb;
        MeshFlags                        m_flags;
        GfxMemUsage                      m_mem_usage = GfxMemUsage::GpuLocal;
        CallbackRef                      m_callback;
    };

    WG_RTTI_CLASS_BEGIN(Mesh) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge