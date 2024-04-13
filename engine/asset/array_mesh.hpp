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
#include "io/serialization.hpp"
#include "math/aabb.hpp"
#include "math/vec.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /** 
     * @brief Represetns indexed mesh triangle primitive
    */
    using MeshFace = Vec3u;

    /** 
     * @brief Represetns indexed mesh line primitive
    */
    using MeshLine = Vec2u;

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
     * @class ArrayMeshData
     * @brief Arrays of mesh attributes for io
    */
    struct ArrayMeshData {
        std::vector<MeshFace> faces;
        std::vector<MeshLine> lines;
        std::vector<Vec3f>    pos3;
        std::vector<Vec2f>    pos2;
        std::vector<Vec3f>    norm;
        std::vector<Vec3f>    tang;
        std::vector<Vec4i>    bone_ids;
        std::vector<Vec4f>    bone_weights;
        std::vector<Vec4f>    col[4];
        std::vector<Vec2f>    uv[4];
        Aabbf                 aabb;
        GfxVertAttribs        attribs;

        WG_IO_DECLARE(ArrayMeshData);
    };

    /**
     * @class ArrayMesh
     * @brief Editable indexed mesh data which can be converted to a render mesh
    */
    class ArrayMesh : public Asset {
    public:
        WG_OBJECT(ArrayMesh, Asset)

        void set_attribs(GfxVertAttribs attribs) { m_data.attribs = attribs; }
        void set_aabb(const Aabbf& aabb) { m_data.aabb = aabb; }
        void set_data(ArrayMeshData&& data) { m_data = std::move(data); }
        void add_vertex(const MeshVertex& vertex);
        void add_face(const MeshFace& face);
        void pack_attribs(const GfxVertAttribsStreams& layout, Ref<Data>& buffer, buffered_vector<GfxVertStream>& streams) const;
        void pack_faces(Ref<Data>& buffer, GfxIndexStream& stream) const;

        [[nodiscard]] const ArrayMeshData& get_data() const { return m_data; }
        [[nodiscard]] const int            get_num_faces() const { return int(m_data.faces.size()); }
        [[nodiscard]] const int            get_num_vertices() const { return int(m_data.pos3.size()); }

        Status read_from_yaml(const YamlConstNodeRef& node) override;
        Status copy_to(Object& other) const override;

    private:
        ArrayMeshData m_data;
    };

}// namespace wmoge