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

#ifndef WMOGE_MESH_BUILDER_HPP
#define WMOGE_MESH_BUILDER_HPP

#include "core/status.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/vec.hpp"
#include "resource/mesh.hpp"

#include <vector>

namespace wmoge {

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
     * @class MeshBuilder
     * @brief Utility class to build a mesh from arrays
     */
    class MeshBuilder {
    public:
        void set_mesh(Ref<Mesh> mesh);
        void add_index(std::uint32_t i);
        void add_triangle(std::uint32_t v0, std::uint32_t v1, std::uint32_t v2);
        void add_vertex(const MeshVertex& v);
        void add_chunk(const MeshChunk& chunk);

        Status build();

        [[nodiscard]] const Ref<Mesh>& get_mesh() const { return m_mesh; }
        [[nodiscard]] int              get_num_vertices() const { return m_num_vertices; }
        [[nodiscard]] int              get_num_indices() const { return m_num_indices; }

    private:
        std::vector<MeshChunk>     m_chunks;
        std::vector<std::uint32_t> m_indices;
        std::vector<Vec3f>         m_pos3;
        std::vector<Vec2f>         m_pos2;
        std::vector<Vec3f>         m_norm;
        std::vector<Vec3f>         m_tang;
        std::vector<Vec4i>         m_bone_ids;
        std::vector<Vec4f>         m_bone_weights;
        std::vector<Vec4f>         m_col[4];
        std::vector<Vec2f>         m_uv[4];

        int m_num_vertices = 0;
        int m_num_indices  = 0;

        Ref<Mesh> m_mesh;
    };

}// namespace wmoge

#endif//WMOGE_MESH_BUILDER_HPP
