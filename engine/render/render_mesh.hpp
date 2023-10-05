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

#ifndef WMOGE_RENDER_MESH_HPP
#define WMOGE_RENDER_MESH_HPP

#include "core/fast_vector.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "render/render_object.hpp"
#include "render/shader_properties.hpp"
#include "resource/material.hpp"
#include "resource/mesh.hpp"
#include "resource/shader.hpp"

#include <vector>

namespace wmoge {

    struct MeshMaterialSlots {
        fast_vector<Ref<Material>, 1> materials;
    };

    struct MeshMaterialReferences {
        fast_vector<int> chunk_indeces;
    };

    struct MeshLods {
        fast_vector<Ref<Mesh>, 1>              lods;
        fast_vector<MeshMaterialReferences, 1> references;
        fast_vector<float, 1>                  distances;
    };

    /**
     * @class MeshCachedRenderData
     * @brief Incapsulates info required to render meshes
     */
    struct MeshCachedRenderData {
        fast_vector<ShaderProperties, 1> properties;
        fast_vector<Ref<GfxPipeline>, 1> pipelines;
    };

    class RenderMesh : public RenderObject {
    public:
        ~RenderMesh() override = default;

    protected:
        MeshMaterialSlots m_material_slots;
        MeshLods          m_lods;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_MESH_HPP