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

#include "core/buffered_vector.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_shader.hpp"
#include "rtti/traits.hpp"

#include <cinttypes>
#include <vector>

namespace wmoge {

    /** @brief Serializable render pass desc */
    struct FilePsoRenderPass {
        WG_RTTI_STRUCT(FilePsoRenderPass);

        buffered_vector<GfxFormat> color_target_fmts;
        buffered_vector<GfxRtOp>   color_target_ops;
        GfxFormat                  depth_stencil_fmt;
        GfxRtOp                    depth_op;
        GfxRtOp                    stencil_op;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoRenderPass) {
        WG_RTTI_FIELD(color_target_fmts, {});
        WG_RTTI_FIELD(color_target_ops, {});
        WG_RTTI_FIELD(depth_stencil_fmt, {});
        WG_RTTI_FIELD(depth_op, {});
        WG_RTTI_FIELD(stencil_op, {});
    }
    WG_RTTI_END;

    /** @brief Serializable vertex format */
    struct FilePsoVertFormat {
        WG_RTTI_STRUCT(FilePsoVertFormat);

        buffered_vector<GfxVertElement> elements;
        Strid                           name;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoVertFormat) {
        WG_RTTI_FIELD(elements, {});
        WG_RTTI_FIELD(name, {});
    }
    WG_RTTI_END;

    /** @brief Serializable pso layout desc (redirection indices) */
    struct FilePsoLayout {
        WG_RTTI_STRUCT(FilePsoLayout);

        buffered_vector<int, 4> layouts;
        Strid                   name;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoLayout) {
        WG_RTTI_FIELD(layouts, {});
        WG_RTTI_FIELD(name, {});
    }
    WG_RTTI_END;

    /** @brief Serialized shader info */
    struct FilePsoShader {
        WG_RTTI_STRUCT(FilePsoShader);

        Sha256          shader_hash;
        GfxShaderModule module_type;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoShader) {
        WG_RTTI_FIELD(shader_hash, {});
        WG_RTTI_FIELD(module_type, {});
    }
    WG_RTTI_END;

    /** @brief Serializable graphics pso desc */
    struct FilePsoStateGraphics {
        WG_RTTI_STRUCT(FilePsoStateGraphics);

        buffered_vector<FilePsoShader, 2> program;
        int                               layout;
        int                               vert_format;
        int                               pass;
        GfxPrimType                       prim_type;
        GfxPolyMode                       poly_mode;
        GfxPolyCullMode                   cull_mode;
        GfxPolyFrontFace                  front_face;
        int                               depth_enable;
        int                               depth_write;
        GfxCompFunc                       depth_func;
        int                               stencil_enable;
        int                               stencil_wmask;
        int                               stencil_rvalue;
        int                               stencil_cmask;
        GfxCompFunc                       stencil_comp_func;
        GfxOp                             stencil_sfail;
        GfxOp                             stencil_dfail;
        GfxOp                             stencil_dpass;
        bool                              blending;
        Strid                             name;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoStateGraphics) {
        WG_RTTI_FIELD(program, {});
        WG_RTTI_FIELD(layout, {});
        WG_RTTI_FIELD(vert_format, {});
        WG_RTTI_FIELD(pass, {});
        WG_RTTI_FIELD(prim_type, {});
        WG_RTTI_FIELD(poly_mode, {});
        WG_RTTI_FIELD(cull_mode, {});
        WG_RTTI_FIELD(front_face, {});
        WG_RTTI_FIELD(depth_enable, {});
        WG_RTTI_FIELD(depth_write, {});
        WG_RTTI_FIELD(depth_func, {});
        WG_RTTI_FIELD(stencil_enable, {});
        WG_RTTI_FIELD(stencil_wmask, {});
        WG_RTTI_FIELD(stencil_rvalue, {});
        WG_RTTI_FIELD(stencil_cmask, {});
        WG_RTTI_FIELD(stencil_comp_func, {});
        WG_RTTI_FIELD(stencil_sfail, {});
        WG_RTTI_FIELD(stencil_dfail, {});
        WG_RTTI_FIELD(stencil_dpass, {});
        WG_RTTI_FIELD(blending, {});
        WG_RTTI_FIELD(name, {});
    }
    WG_RTTI_END;

    /** @brief Serializable compute pso desc */
    struct FilePsoStateCompute {
        WG_RTTI_STRUCT(FilePsoStateCompute);

        FilePsoShader shader;
        int           layout;
        Strid         name;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoStateCompute) {
        WG_RTTI_FIELD(shader, {});
        WG_RTTI_FIELD(layout, {});
        WG_RTTI_FIELD(name, {});
    }
    WG_RTTI_END;

    /** @brief Serializable pso data (to construct pso from file) */
    struct FilePsoData {
        WG_RTTI_STRUCT(FilePsoData);

        GfxShaderPlatform                 platform;
        std::vector<FilePsoRenderPass>    render_passes;
        std::vector<FilePsoVertFormat>    vert_formats;
        std::vector<GfxDescSetLayoutDesc> desc_layouts;
        std::vector<FilePsoLayout>        pso_layouts;
        std::vector<FilePsoStateGraphics> pso_graphics;
        std::vector<FilePsoStateCompute>  pso_compute;
    };

    WG_RTTI_STRUCT_BEGIN(FilePsoData) {
        WG_RTTI_FIELD(platform, {});
        WG_RTTI_FIELD(render_passes, {});
        WG_RTTI_FIELD(vert_formats, {});
        WG_RTTI_FIELD(desc_layouts, {});
        WG_RTTI_FIELD(pso_layouts, {});
        WG_RTTI_FIELD(pso_graphics, {});
        WG_RTTI_FIELD(pso_compute, {});
    }
    WG_RTTI_END;

}// namespace wmoge