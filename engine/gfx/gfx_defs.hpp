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

#ifndef WMOGE_GFX_DEFS_HPP
#define WMOGE_GFX_DEFS_HPP

#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /** @brief Status of the shader */
    enum class GfxShaderStatus {
        Compiling,
        Compiled,
        Failed
    };

    /** @brief Status of the gfx pipeline */
    enum class GfxPipelineStatus {
        Default,
        Creating,
        Created,
        Failed
    };

    /** @brief Type of elements in index buffer */
    enum class GfxIndexType {
        Uint32,
        Uint16
    };

    /** @brief Gfx memory usage */
    enum class GfxMemUsage {
        /** Cpu resident buffer (slow), which can be used on GPU as well */
        CpuVisibleGpu,
        /** Gpu resident buffer (good), which can be mapped on CPU for frequent updates */
        GpuVisibleCpu,
        /** Gpu resident buffer (fast), can update from CPU by write only operations */
        GpuLocal,
        /** Gpu resident buffer (fastest), prefer dedicated allocation (for RT textures) */
        GpuDedicated
    };

    /** @brief Texture type */
    enum class GfxTex {
        Tex2d,
        Tex2dArray,
        TexCube
    };

    /** @brief Texture usage */
    enum class GfxTexUsageFlag {
        /** Texture can be used as render target depth-stencil attachment */
        DepthStencilTarget = 1,
        /** Texture can be used as render target depth attachment */
        DepthTarget = 2,
        /** Texture can be used as render target color attachment */
        ColorTarget = 3,
        /** Texture can be sampled within shader */
        Sampling = 4
    };

    /** @brief Texture usage mask */
    using GfxTexUsages = Mask<GfxTexUsageFlag, 8>;

    /** @brief Face id in cube map texture */
    enum class GfxTexCubeFace {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    /** @brief Formats used to specify internal storage format */
    enum class GfxFormat {
        R8,
        R8_SNORM,
        R16,
        R16_SNORM,
        RG8,
        RG8_SNORM,
        RG16,
        RG16_SNORM,
        RGB8,
        RGB8_SNORM,
        RGB16_SNORM,
        RGBA8,
        RGBA8_SNORM,
        RGBA16,
        SRGB8,
        SRGB8_ALPHA8,
        SBGR8_ALPHA8,
        R32I,
        RG32I,
        RGB32I,
        RGBA32I,
        R16F,
        RG16F,
        RGB16F,
        RGBA16F,
        R32F,
        RG32F,
        RGB32F,
        RGBA32F,
        DEPTH32F,
        DEPTH32F_STENCIL8,
        DEPTH24_STENCIL8
    };

    /** @brief Types of the geometry formed by input vertex data */
    enum class GfxPrimType {
        Triangles,
        Lines,
        Points
    };

    /** @brief How primitives rasterized */
    enum class GfxPolyMode {
        Fill,
        Line,
        Point
    };

    /** @brief Which sides of polygons are culled */
    enum class GfxPolyCullMode {
        Disabled,
        Front,
        Back,
        FrontAndBack
    };

    /** @brief How front of the primitive is defined */
    enum class GfxPolyFrontFace {
        Clockwise,
        CounterClockwise
    };

    /** @brief Compare function */
    enum class GfxCompFunc {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        GreaterEqual,
        NotEqual,
        Always
    };

    /** @brief Operation */
    enum class GfxOp {
        Keep,
        Zero,
        Replace,
        Increment,
        Decrement,
        Invert
    };

    /** @brief Blend factor */
    enum class GfxBlendFac {
        /** = 0 */
        Zero,
        /** = 1 */
        One,
        /** = src */
        SrcColor,
        /** = 1 - src */
        OneMinusSrcColor,
        /** = dst */
        DstColor,
        /** = 1 - dst */
        OneMinusDstColor,
        /** = src.a */
        SrcAlpha,
        /** = 1 - src.a */
        OneMinusSrcAlpha,
        /** = dst.a */
        DstAlpha,
        /** = 1 - dst.a  */
        OneMinusDstAlpha
    };

    /** @brief Blend operation */
    enum class GfxBlendOp {
        /** Fragment color added to the color buffer */
        Add,
        /** Fragment color subtracted from the color buffer */
        Subtract,
        /** Color buffer color is subtracted from fragment color  */
        ReverseSubtract,
        /** The min between the fragment and color buffer */
        Min,
        /** The max between the fragment and color buffer */
        Max
    };

    enum class GfxSampFlt {
        /** @brief Returns the value of the texture element that is nearest */
        Nearest,
        /** @brief Returns the weighted average of the four texture elements
         * that are closest to the center of the pixel being textured */
        Linear,
        /** Chooses the mipmap that most closely matches the size of the pixel
         * being textured and uses the Nearest criterion */
        NearestMipmapNearest,
        /** @brief Chooses the mipmap that most closely matches the size of the
         * pixel being textured and uses the Linear criterion */
        LinearMipmapNearest,
        /** @brief  Chooses the two mipmaps that most closely match the size of
         * the pixel being textured and uses the Nearest criterion */
        NearestMipmapLinear,
        /** @brief Chooses the two mipmaps that most closely match the size of
         * the pixel being textured and uses the Linear criterion */
        LinearMipmapLinear
    };

    /** @brief Sampler address mode */
    enum class GfxSampAddress {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClamToEdge,
    };

    /** @brief Sampler border color */
    enum class GfxSampBrdClr {
        Black,
        White
    };

    /** @brief Enumerates all possible gfx render passes */
    enum class GfxRenderPassType {
        Default,
        DebugDraw,
        AuxDraw
    };

    /** @brief How to bind resource to gfx pipeline */
    struct GfxLocation {
        int set;
        int binding;
    };

    /** @brief Gfx common device limits */
    struct GfxLimits {
        /** Total number of render passes in gfx driver */
        static const int MAX_RENDER_PASSES = 1;
        /** Vertex shader max input elements */
        static const int MAX_VERT_ATTRIBUTES = 8;
        /** Vertex shader max input vertex buffers */
        static const int MAX_VERT_BUFFERS = 4;
        /** Limit color attachment count */
        static const int MAX_COLOR_TARGETS = 8;
        /** Max number of cube texture faces (it is cube, lol) */
        static const int MAX_CUBE_FACES = 6;
        /** Max number of simultaneously drawn frames (1 - draw frame and wait, 2 - draw and present one frame, prepare other, ...)*/
        static const int FRAMES_IN_FLIGHT = 2;
        /** Max resources sets count */
        static const int MAX_DESC_SETS = 3;
    };

    /** @brief Gfx device capabilities */
    struct GfxDeviceCaps {
        int   max_vertex_attributes          = -1;
        int   max_combined_uniform_blocks    = -1;
        int   max_texture_array_layers       = -1;
        int   max_texture_3d_size            = -1;
        int   max_texture_size               = -1;
        int   max_texture_units              = -1;
        int   max_color_attachments          = -1;
        int   max_framebuffer_width          = -1;
        int   max_framebuffer_height         = -1;
        int   uniform_block_offset_alignment = -1;
        float max_anisotropy                 = 0.0f;
        bool  support_anisotropy             = false;
    };

}// namespace wmoge

#endif//WMOGE_GFX_DEFS_HPP
