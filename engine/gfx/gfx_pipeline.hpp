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

#ifndef WMOGE_GFX_PIPELINE_HPP
#define WMOGE_GFX_PIPELINE_HPP

#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_vert_format.hpp"

#include <utility>

namespace wmoge {

    /**
     * @class GfxPipelineState
     * @brief Gfx pipeline state description
     */
    struct GfxPipelineState {
        GfxPipelineState();
        bool        operator==(const GfxPipelineState& other) const;
        std::size_t hash() const;

        ref_ptr<GfxShader>     shader;           // = nullptr;
        ref_ptr<GfxRenderPass> pass;             // = nullptr;
        ref_ptr<GfxVertFormat> vert_format;      // = nullptr;
        GfxPrimType            prim_type;        // = GfxPrimType::Triangles;
        GfxPolyMode            poly_mode;        // = GfxPolyMode::Fill;
        GfxPolyCullMode        cull_mode;        // = GfxPolyCullMode::Disabled;
        GfxPolyFrontFace       front_face;       // = GfxPolyFrontFace::CounterClockwise;
        int                    depth_enable;     // = false;
        int                    depth_write;      // = true;
        GfxCompFunc            depth_func;       // = GfxCompFunc::Less;
        int                    stencil_enable;   // = false;
        int                    stencil_wmask;    // = 0;
        int                    stencil_rvalue;   // = 0;
        int                    stencil_cmask;    // = 0;
        GfxCompFunc            stencil_comp_func;// = GfxCompFunc::Never;
        GfxOp                  stencil_sfail;    // = GfxOp::Keep;
        GfxOp                  stencil_dfail;    // = GfxOp::Keep;
        GfxOp                  stencil_dpass;    // = GfxOp::Keep;
        int                    blending;         // = false;
    };

    /**
     * @class GfxPipeline
     * @brief Represents created and compiled graphics pipeline state object
     *
     * Pipeline is a compete object which can be directly bound to the command list for the rendering.
     * Pipeline creation depends on a render pass and shader.
     * As soon as dependencies created pipeline creation is started.
     * Pipeline creation is asynchronous and done in the background.
     * When pipeline created it will be used in the rendering.
     */
    class GfxPipeline : public GfxResource {
    public:
        ~GfxPipeline() override                         = default;
        virtual GfxPipelineStatus       status() const  = 0;
        virtual std::string             message() const = 0;
        virtual const GfxPipelineState& state() const   = 0;
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::GfxPipelineState> {
        std::size_t operator()(const wmoge::GfxPipelineState& desc) const {
            return desc.hash();
        }
    };

}// namespace std

#endif//WMOGE_GFX_PIPELINE_HPP
