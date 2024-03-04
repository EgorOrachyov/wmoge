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

#include "core/fast_vector.hpp"
#include "core/status.hpp"
#include "gfx/gfx_defs.hpp"
#include "math/vec.hpp"
#include "resource/array_mesh.hpp"
#include "resource/mesh.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class MeshBuilder
     * @brief Utility class to build a mesh from arrays
     */
    class MeshBuilder {
    public:
        void set_mesh(Ref<Mesh> mesh);
        void add_chunk(const Strid& name, const Ref<ArrayMesh>& data);
        void add_child(int parent_idx, int child_idx);

        Status build();

        [[nodiscard]] const Ref<Mesh>& get_mesh() const { return m_mesh; }

    private:
        std::vector<Ref<ArrayMesh>>   m_chunks;
        std::vector<Strid>            m_chunks_names;
        std::vector<int>              m_chunks_parents;
        std::vector<fast_vector<int>> m_chunks_children;

        Ref<Mesh> m_mesh;
    };

}// namespace wmoge