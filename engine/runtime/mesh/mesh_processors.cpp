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

#include "mesh_processors.hpp"

#include "gfx/gfx_driver.hpp"
#include "material/material.hpp"
#include "mesh/mesh_batch.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    bool MeshPassProcessorGBuffer::filter(const MeshBatch& batch) {
        return true;
    }
    Status MeshPassProcessorGBuffer::compile(const MeshBatch& batch, Ref<GfxPsoGraphics>& out_pipeline) {
        WG_AUTO_PROFILE_MESH("MeshPassProcessorGBuffer::compile");

        // Material*           material       = batch.material;
        // Shader*             shader         = material->get_shader().get();
        // ShaderPipelineState pipeline_state = shader->get_pipeline_state();

        buffered_vector<std::string> defines;
        {
            defines.push_back("MESH_PASS_GBUFFER");
        }

        GfxVertAttribs attribs;

        // additional attribute to fetch gpu data
        // attribs.set(GfxVertAttrib::PrimitiveIdi);

        GfxPsoStateGraphics gfx_pso_state;
        // gfx_pso_state.shader       = m_shader_manager->get_shader(shader->get_domain(), attribs, defines, shader);
        // gfx_pso_state.prim_type    = batch.prim_type;
        // gfx_pso_state.poly_mode    = pipeline_state.poly_mode;
        // gfx_pso_state.cull_mode    = pipeline_state.cull_mode;
        // gfx_pso_state.front_face   = pipeline_state.front_face;
        // gfx_pso_state.depth_enable = pipeline_state.depth_enable;
        // gfx_pso_state.depth_write  = pipeline_state.depth_write;
        // gfx_pso_state.depth_func   = pipeline_state.depth_func;
        // gfx_pso_state.blending     = false;

        // out_pipeline = m_gfx_driver->make_pso_graphics(gfx_pso_state);

        return WG_OK;
    }
    std::string MeshPassProcessorGBuffer::get_name() const {
        return "MeshPassProcessorGBuffer";
    }
    MeshPassType MeshPassProcessorGBuffer::get_pass_type() const {
        return MeshPassType::GBuffer;
    }

}// namespace wmoge