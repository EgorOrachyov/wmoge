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
#include "core/data.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/aabb.hpp"
#include "math/vec.hpp"
#include "rtti/traits.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /** @brief Represetns indexed mesh triangle primitive */
    using MeshFace = Vec3u;

    /** @brief Represetns indexed mesh line primitive */
    using MeshLine = Vec2u;

    /**
     * @class MeshVertStream
     * @brief Provides setup for a stream of vertex attributes packed into vertex buffer
    */
    struct MeshVertStream {
        WG_RTTI_STRUCT(MeshVertStream);

        GfxVertAttribs attribs;
        int            buffer = -1;
        int            offset = 0;
        int            size   = 0;
        int            stride = 0;
    };

    WG_RTTI_STRUCT_BEGIN(MeshVertStream) {
        WG_RTTI_FIELD(attribs, {});
        WG_RTTI_FIELD(buffer, {});
        WG_RTTI_FIELD(offset, {});
        WG_RTTI_FIELD(size, {});
        WG_RTTI_FIELD(stride, {});
    }
    WG_RTTI_END;

    /**
     * @class MeshIndexStream
     * @brief Provides setup with index data packed into index buffer
    */
    struct MeshIndexStream {
        WG_RTTI_STRUCT(MeshIndexStream);

        GfxIndexType index_type = GfxIndexType::None;
        int          buffer     = -1;
        int          offset     = 0;
        int          size       = 0;
    };

    WG_RTTI_STRUCT_BEGIN(MeshIndexStream) {
        WG_RTTI_FIELD(index_type, {});
        WG_RTTI_FIELD(buffer, {});
        WG_RTTI_FIELD(offset, {});
        WG_RTTI_FIELD(size, {});
    }
    WG_RTTI_END;

    /**
     * @class MeshVertex
     * @brief Holds the data of a single vertex
    */
    struct MeshVertex {
        Vec3f          pos3;
        Vec2f          pos2;
        Vec3f          norm;
        Vec3f          tang;
        Vec4i          bone_ids;
        Vec4f          bone_weights;
        Vec4f          col[4];
        Vec2f          uv[4];
        GfxVertAttribs attribs;
    };

    /**
     * @class MeshArrayData
     * @brief Arrays of mesh attributes for io
    */
    struct MeshArrayData {
        WG_RTTI_STRUCT(MeshArrayData);

        std::vector<MeshFace> faces;
        std::vector<MeshLine> lines;
        std::vector<Vec3f>    pos3;
        std::vector<Vec2f>    pos2;
        std::vector<Vec3f>    norm;
        std::vector<Vec3f>    tang;
        std::vector<Vec4i>    bone_ids;
        std::vector<Vec4f>    bone_weights;
        std::vector<Vec4f>    col0, col1, col2, col3;
        std::vector<Vec2f>    uv0, uv1, uv2, uv3;
        Aabbf                 aabb;
        GfxVertAttribs        attribs;
    };

    WG_RTTI_STRUCT_BEGIN(MeshArrayData) {
        WG_RTTI_FIELD(faces, {});
        WG_RTTI_FIELD(lines, {});
        WG_RTTI_FIELD(pos3, {});
        WG_RTTI_FIELD(pos2, {});
        WG_RTTI_FIELD(norm, {});
        WG_RTTI_FIELD(tang, {});
        WG_RTTI_FIELD(bone_ids, {});
        WG_RTTI_FIELD(bone_weights, {});
        WG_RTTI_FIELD(col0, {});
        WG_RTTI_FIELD(col1, {});
        WG_RTTI_FIELD(col2, {});
        WG_RTTI_FIELD(col3, {});
        WG_RTTI_FIELD(uv0, {});
        WG_RTTI_FIELD(uv1, {});
        WG_RTTI_FIELD(uv2, {});
        WG_RTTI_FIELD(uv3, {});
        WG_RTTI_FIELD(aabb, {});
        WG_RTTI_FIELD(attribs, {});
    }
    WG_RTTI_END;

    /**
     * @class MeshArray
     * @brief Editable indexed mesh data which can be converted to a render mesh
    */
    class MeshArray : public Asset {
    public:
        WG_RTTI_CLASS(MeshArray, Asset);

        MeshArray()           = default;
        ~MeshArray() override = default;

        void set_attribs(GfxVertAttribs attribs) { m_data.attribs = attribs; }
        void set_aabb(const Aabbf& aabb) { m_data.aabb = aabb; }
        void set_data(MeshArrayData&& data) { m_data = std::move(data); }
        void add_vertex(const MeshVertex& vertex);
        void add_face(const MeshFace& face);
        void pack_attribs(const GfxVertAttribsStreams& layout, Ref<Data>& buffer, buffered_vector<MeshVertStream>& streams) const;
        void pack_faces(Ref<Data>& buffer, MeshIndexStream& stream) const;

        [[nodiscard]] const MeshArrayData& get_data() const { return m_data; }
        [[nodiscard]] const int            get_num_faces() const { return int(m_data.faces.size()); }
        [[nodiscard]] const int            get_num_vertices() const { return int(m_data.pos3.size()); }

    private:
        MeshArrayData m_data;
    };

    WG_RTTI_CLASS_BEGIN(MeshArray) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_data, {});
    }
    WG_RTTI_END;

}// namespace wmoge