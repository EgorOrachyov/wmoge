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

#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_vert_format.hpp"

#include <utility>

namespace wmoge {

    /**
     * @class GfxPsoLayout
     * @brief Layout of pipeline for resources binding
    */
    class GfxPsoLayout : public GfxResource {
    public:
        ~GfxPsoLayout() override = default;

        [[nodiscard]] const GfxDescSetLayouts& get_layouts() const { return m_layouts; }

    protected:
        GfxDescSetLayouts m_layouts;
    };

    using GfxPsoLayoutRef = Ref<GfxPsoLayout>;

    /** @brief Graphics pipeline raster state */
    struct GfxRasterState {
        GfxPolyMode      poly_mode  = GfxPolyMode::Fill;
        GfxPolyCullMode  cull_mode  = GfxPolyCullMode::Disabled;
        GfxPolyFrontFace front_face = GfxPolyFrontFace::CounterClockwise;
    };

    /** @brief Graphics pipeline depth stencil state */
    struct GfxDepthStencilState {
        int         depth_enable      = false;
        int         depth_write       = true;
        GfxCompFunc depth_func        = GfxCompFunc::Less;
        int         stencil_enable    = false;
        int         stencil_wmask     = 0;
        int         stencil_rvalue    = 0;
        int         stencil_cmask     = 0;
        GfxCompFunc stencil_comp_func = GfxCompFunc::Never;
        GfxOp       stencil_sfail     = GfxOp::Keep;
        GfxOp       stencil_dfail     = GfxOp::Keep;
        GfxOp       stencil_dpass     = GfxOp::Keep;
    };

    /** @brief Graphics pipeline blending state */
    struct GfxBlendState {
        int blending = false;
    };

    /**
     * @class GfxPsoStateGraphics
     * @brief Gfx pipeline state description
     */
    struct GfxPsoStateGraphics {
        GfxPsoStateGraphics();
        bool        operator==(const GfxPsoStateGraphics& other) const;
        std::size_t hash() const;

        Ref<GfxRenderPass>    pass;       // = nullptr;
        Ref<GfxShaderProgram> program;    // = nullptr;
        Ref<GfxPsoLayout>     layout;     // = nullptr;
        Ref<GfxVertFormat>    vert_format;// = nullptr;
        GfxPrimType           prim_type;  // = GfxPrimType::Triangles;
        GfxRasterState        rs;         // = default
        GfxDepthStencilState  ds;         // = default
        GfxBlendState         bs;         // = default;
    };

    /**
     * @class GfxPsoStateCompute
     * @brief Gfx compute pipeline state description
     */
    struct GfxPsoStateCompute {
        GfxPsoStateCompute();
        bool        operator==(const GfxPsoStateCompute& other) const;
        std::size_t hash() const;

        Ref<GfxShaderProgram> program;// = nullptr;
        Ref<GfxPsoLayout>     layout; // = nullptr;
    };

    /**
     * @class GfxPso
     * @brief Base class for compiled pipeline state object
    */
    class GfxPso : public GfxResource {
    public:
        ~GfxPso() override = default;
    };

    using GfxPsoRef = Ref<GfxPso>;

    /**
     * @class GfxPsoGraphics
     * @brief Represents created and compiled graphics pipeline state object
     *
     * Pipeline is a compete object which can be directly bound to the command list for the rendering.
     */
    class GfxPsoGraphics : public GfxPso {
    public:
        ~GfxPsoGraphics() override = default;
    };

    using GfxPsoGraphicsRef = Ref<GfxPsoGraphics>;

    /**
     * @class GfxPsoCompute
     * @brief Represents created and compiled compute pipeline state object
     *
     * Pipeline is a compete object which can be directly bound to the command list for the compute dispatch.
     */
    class GfxPsoCompute : public GfxPso {
    public:
        ~GfxPsoCompute() override = default;
    };

    using GfxPsoComputeRef = Ref<GfxPsoCompute>;

    /**
     * @class GfxAsyncPsoRequestGraphics
     * @brief Request for async pso compilation
    */
    class GfxAsyncPsoRequestGraphics : public RefCnt {
    public:
        ~GfxAsyncPsoRequestGraphics() override = default;

        buffered_vector<GfxPsoStateGraphics, 1> states;
        buffered_vector<Strid, 1>               names;
        buffered_vector<Ref<GfxPsoGraphics>, 1> pso;
    };

    using GfxAsyncPsoRequestGraphicsRef = Ref<GfxAsyncPsoRequestGraphics>;

    /**
     * @class GfxAsyncPsoRequestCompute
     * @brief Request for async pso compilation
    */
    class GfxAsyncPsoRequestCompute : public RefCnt {
    public:
        ~GfxAsyncPsoRequestCompute() override = default;

        buffered_vector<GfxPsoStateCompute, 1> states;
        buffered_vector<Strid, 1>              names;
        buffered_vector<Ref<GfxPsoCompute>, 1> pso;
    };

    using GfxAsyncPsoRequestComputeRef = Ref<GfxAsyncPsoRequestCompute>;

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::GfxPsoStateGraphics> {
        std::size_t operator()(const wmoge::GfxPsoStateGraphics& desc) const {
            return desc.hash();
        }
    };

    template<>
    struct hash<wmoge::GfxPsoStateCompute> {
        std::size_t operator()(const wmoge::GfxPsoStateCompute& desc) const {
            return desc.hash();
        }
    };

}// namespace std