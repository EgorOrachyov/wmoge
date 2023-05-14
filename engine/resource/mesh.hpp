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
     * @class MeshAttrib
     * @brief Attributes of mesh vertices for gpu rendering
     */
    enum class MeshAttrib {
        Position = 0, /** Stored in buffer[0] type of float-3 */
        Normal,       /** Stored in buffer[0] type of float-3 */
        Tangent,      /** Stored in buffer[0] type of float-3 */
        BoneIds,      /** Stored in buffer[1] type of int-4 */
        BoneWeights,  /** Stored in buffer[1] type of float-4 */
        Color0,       /** Stored in buffer[2] type of float-4 */
        Color1,       /** Stored in buffer[2] type of float-4 */
        Color2,       /** Stored in buffer[2] type of float-4 */
        Color3,       /** Stored in buffer[2] type of float-4 */
        Uv0,          /** Stored in buffer[2] type of float-2 */
        Uv1,          /** Stored in buffer[2] type of float-2 */
        Uv2,          /** Stored in buffer[2] type of float-2 */
        Uv3,          /** Stored in buffer[2] type of float-2 */
        Max
    };

    /**
     * @class MeshAttribs
     * @brief Set of attributes describing a mesh
     */
    using MeshAttribs = Mask<MeshAttrib>;

    /**
     * @class MeshChunk
     * @brief Represents single mesh chunk in a mesh which can be rendered individually
     */
    struct MeshChunk {
        StringId name;
        Aabbf    aabb;
        int      vertex_offset;
        int      index_offset;
        int      index_count;
    };

    /**
     * @class Mesh
     * @brief Vertex and index data structured as chunks which can be rendered
     */
    class Mesh : public Resource {
    public:
        static const int MAX_BUFFER       = 3;
        static const int MAX_ATTRIB       = static_cast<int>(MeshAttrib::Max);
        static const int BUFF0_MAX_ATTRIB = 3;
        static const int BUFF1_MAX_ATTRIB = 5;
        static const int BUFF2_MAX_ATTRIB = MAX_ATTRIB;

        WG_OBJECT(Mesh, Resource);

        void add_chunk(const MeshChunk& mesh_chunk);
        void set_vertex_params(int num_vertices, GfxPrimType prim_type);
        void set_vertex_buffer(int index, Ref<Data> buffer);
        void set_index_params(int num_indices, GfxIndexType index_type);
        void set_index_buffer(Ref<Data> buffer);
        void set_attribs(MeshAttribs attribs);

        void update_aabb();
        void update_gfx_buffers();

        const std::vector<MeshChunk>& get_chunks();
        const MeshChunk&              get_chunk(int i);
        const Ref<Data>&              get_vertex_buffer(int i);
        const Ref<Data>&              get_index_buffer();
        const Ref<GfxVertBuffer>&     get_gfx_vertex_buffer(int i);
        const Ref<GfxIndexBuffer>&    get_gfx_index_buffer();
        int                           get_buffer_stride(int buffer);
        int                           get_attrib_offset(MeshAttrib attrib);
        GfxIndexType                  get_index_type();
        GfxPrimType                   get_prim_type();
        MeshAttribs                   get_attribs();
        int                           get_num_vertices();
        int                           get_num_indices();
        Aabbf                         get_aabb();

        static void      get_buffer_attribs(int buffer, int& start, int& stop);
        static int       get_attrib_size(MeshAttrib attrib);
        static int       get_attrib_buffer(MeshAttrib attrib);
        static Ref<Mesh> create_cube(const Vec3f& size);
        static Ref<Mesh> create_sphere(const Vec3f& size);

    private:
        std::vector<MeshChunk> m_chunks;
        int                    m_attributes_offsets[MAX_ATTRIB];
        int                    m_buffers_strides[MAX_BUFFER];
        Ref<GfxVertBuffer>     m_gfx_vertex_buffers[MAX_BUFFER];
        Ref<Data>              m_vertex_buffers[MAX_BUFFER];
        Ref<GfxIndexBuffer>    m_gfx_index_buffer;
        Ref<Data>              m_index_buffer;
        GfxIndexType           m_index_type;
        GfxPrimType            m_prim_type;
        MeshAttribs            m_attribs;
        int                    m_num_vertices = 0;
        int                    m_num_indices  = 0;
        Aabbf                  m_aabb;
    };

}// namespace wmoge

#endif//WMOGE_MESH_HPP
