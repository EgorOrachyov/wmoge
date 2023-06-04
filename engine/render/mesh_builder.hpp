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

#include "gfx/gfx_defs.hpp"
#include "math/vec.hpp"
#include "resource/mesh.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class MeshBuilder
     * @brief Utility class to build a mesh from arrays
     */
    struct MeshBuilder {
        void add_index(std::uint32_t i);

        bool build();

        Ref<Mesh> mesh;

        std::vector<MeshChunk>     chunks;
        std::vector<std::uint32_t> indices;
        std::vector<Vec3f>         pos3;
        std::vector<Vec2f>         pos2;
        std::vector<Vec3f>         norm;
        std::vector<Vec3f>         tang;
        std::vector<Vec4i>         bone_ids;
        std::vector<Vec4f>         bone_weights;
        std::vector<Vec4f>         col[4];
        std::vector<Vec2f>         uv[4];

        int num_vertices = 0;
        int num_indices  = 0;
    };

}// namespace wmoge

#endif//WMOGE_MESH_BUILDER_HPP
