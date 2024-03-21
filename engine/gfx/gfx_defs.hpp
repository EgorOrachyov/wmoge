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

#include "core/data.hpp"
#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "math/vec.hpp"

#include <array>
#include <bitset>
#include <cinttypes>

namespace wmoge {

    /** @brief Gfx common device limits */
    struct GfxLimits {
        /** Vertex shader max input elements */
        static constexpr int MAX_VERT_ATTRIBUTES = 8;
        /** Vertex shader max input vertex buffers */
        static constexpr int MAX_VERT_BUFFERS = 4;
        /** Vertex shader max input vertex streams with user data */
        static constexpr int MAX_VERT_STREAMS = 3;
        /** Limit color attachment count */
        static constexpr int MAX_COLOR_TARGETS = 8;
        /** Max number of cube texture faces (it is cube, lol) */
        static constexpr int MAX_CUBE_FACES = 6;
        /** Max number of simultaneously drawn frames (1 - draw frame and wait, 2 - draw and present one frame, prepare other, ...)*/
        static constexpr int FRAMES_IN_FLIGHT = 2;
        /** Max resources sets count */
        static constexpr int MAX_DESC_SETS = 3;
        /** Max size of a single desc set */
        static constexpr int MAX_DESC_SET_SIZE = 128;
    };

    /** @brief Type of gfx context behaviour */
    enum class GfxCtxType : int {
        Immediate,
        Deferred,
        Async
    };

    /** @brief Status of the shader */
    enum class GfxShaderStatus : int {
        Compiling,
        Compiled,
        Failed
    };

    /** @brief Status of the gfx pipeline */
    enum class GfxPipelineStatus : int {
        Default,
        Creating,
        Created,
        Failed
    };

    /** @brief Type of elements in index buffer */
    enum class GfxIndexType : int {
        None,
        Uint32,
        Uint16
    };

    /** @brief Gfx memory usage */
    enum class GfxMemUsage : int {
        /** Cpu resident buffer (slow), which can be used on GPU as well */
        CpuVisibleGpu,
        /** Gpu resident buffer (good), which can be mapped on CPU for frequent updates */
        GpuVisibleCpu,
        /** Gpu resident buffer (fast), can update from CPU by write only operations */
        GpuLocal,
        /** Gpu resident buffer (fastest), prefer dedicated allocation (for RT textures) */
        GpuDedicated
    };

    /** @brief Texture sub region */
    struct GfxTexRegion {
        int base_mip         = 0;
        int num_mips         = 0;
        int base_array_slice = 0;
        int num_array_slices = 0;
    };

    /** @brief Texture manual barrier type */
    enum class GfxTexBarrierType : int {
        RenderTarget,
        Sampling,
        Storage
    };

    /** @brief Texture type */
    enum class GfxTex : int {
        Unknown = 0,
        Tex2d,
        Tex2dArray,
        TexCube
    };

    /** @brief Texture usage */
    enum class GfxTexUsageFlag : int {
        /** Texture can be used as render target depth-stencil attachment */
        DepthStencilTarget = 1,
        /** Texture can be used as render target depth attachment */
        DepthTarget = 2,
        /** Texture can be used as render target color attachment */
        ColorTarget = 3,
        /** Texture can be used in compute shaders as storage image */
        Storage = 3,
        /** Texture can be sampled within shader */
        Sampling = 4
    };

    /** @brief Texture usage mask */
    using GfxTexUsages = Mask<GfxTexUsageFlag, 8>;

    /** @brief Face id in cube map texture */
    enum class GfxTexCubeFace : int {
        PositiveX = 0,
        NegativeX = 1,
        PositiveY = 2,
        NegativeY = 3,
        PositiveZ = 4,
        NegativeZ = 5,
    };

    /** @brief Tex view channels swizzling */
    enum class GfxTexSwizz : int {
        None       = 0,
        RRRRtoRGBA = 1
    };

    /** @brief Formats used to specify internal storage format */
    enum class GfxFormat : int {
        Unknown,

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

        BC1_RGB,
        BC1_RGB_SRGB,
        BC1_RGBA,
        BC1_RGBA_SRGB,
        BC2,
        BC2_SRGB,
        BC3,
        BC3_SRGB,
        BC4,
        BC4_SNORM,
        BC5,
        BC5_SNORM,
        BC6H_UFLOAT,
        BC6H_SFLOAT,
        BC7,
        BC7_SRGB,

        DEPTH32F,
        DEPTH32F_STENCIL8,
        DEPTH24_STENCIL8
    };

    /** @brief Types of the geometry formed by input vertex data */
    enum class GfxPrimType : int {
        Triangles,
        Lines,
        Points
    };

    /** @brief How primitives rasterized */
    enum class GfxPolyMode : int {
        Fill,
        Line,
        Point
    };

    /** @brief Which sides of polygons are culled */
    enum class GfxPolyCullMode : int {
        Disabled,
        Front,
        Back,
        FrontAndBack
    };

    /** @brief How front of the primitive is defined */
    enum class GfxPolyFrontFace : int {
        Clockwise,
        CounterClockwise
    };

    /** @brief Compare function */
    enum class GfxCompFunc : int {
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
    enum class GfxOp : int {
        Keep,
        Zero,
        Replace,
        Increment,
        Decrement,
        Invert
    };

    /** @brief Blend factor */
    enum class GfxBlendFac : int {
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
    enum class GfxBlendOp : int {
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

    enum class GfxSampFlt : int {
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
    enum class GfxSampAddress : int {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
        MirrorClamToEdge,
    };

    /** @brief Sampler border color */
    enum class GfxSampBrdClr : int {
        Black,
        White
    };

    /** @brief Option on render target in render pass */
    enum class GfxRtOp : int {
        LoadStore,
        ClearStore
    };

    /** @brief Expected language of shaders */
    enum class GfxShaderLang : int {
        GlslVk450,
        GlslGl410
    };

    /** @brief Shader modules types */
    enum class GfxShaderModule : int {
        Vertex,
        Fragment,
        Compute
    };

    /** @brief Data parameters which can be exposed by a shader */
    enum class GfxShaderParam : int {
        Int,
        Float,
        Vec2,
        Vec3,
        Vec4
    };

    /** @brief Named vertex attributes */
    enum class GfxVertAttrib : int {
        Pos3f,
        Pos2f,
        Norm3f,
        Tang3f,
        BoneIds4i,
        BoneWeights4f,
        Col04f,
        Col14f,
        Col24f,
        Col34f,
        Uv02f,
        Uv12f,
        Uv22f,
        Uv32f,
        PrimitiveIdi,
        None
    };

    /** @brief Vert attribute formats */
    static constexpr const GfxFormat GfxVertAttribFormats[] = {
            GfxFormat::RGB32F,
            GfxFormat::RG32F,
            GfxFormat::RGB32F,
            GfxFormat::RGB32F,
            GfxFormat::RGBA32I,
            GfxFormat::RGBA32F,
            GfxFormat::RGBA32F,
            GfxFormat::RGBA32F,
            GfxFormat::RGBA32F,
            GfxFormat::RGBA32F,
            GfxFormat::RG32F,
            GfxFormat::RG32F,
            GfxFormat::RG32F,
            GfxFormat::RG32F,
            GfxFormat::R32I};

    /** @brief Vert attribute sizes */
    static constexpr const int GfxVertAttribSizes[] = {
            12,
            8,
            12,
            12,
            16,
            16,
            16,
            16,
            16,
            16,
            8,
            8,
            8,
            8,
            4};

    /** @brief Vert attribute glsl type */
    static constexpr const char* GfxVertAttribGlslTypes[] = {
            "vec3",
            "vec2",
            "vec3",
            "vec3",
            "ivec4",
            "vec4",
            "vec4",
            "vec4",
            "vec4",
            "vec4",
            "vec2",
            "vec2",
            "vec2",
            "vec2",
            "int"};

    /** @brief Mask of vertex attributes */
    using GfxVertAttribs = Mask<GfxVertAttrib>;

    /** @brief Decl of vertex attributes streams */
    using GfxVertAttribsStreams = std::array<GfxVertAttribs, GfxLimits::MAX_VERT_STREAMS>;

    /** @brief Type of bindings to a pipeline */
    enum class GfxBindingType : std::uint8_t {
        Unknown = 0,
        SampledTexture,
        UniformBuffer,
        StorageBuffer,
        StorageImage
    };

    /** @brief How to bind resource to gfx pipeline */
    struct GfxLocation {
        std::int16_t set;
        std::int16_t binding;
    };

    /** @brief Holds data of an image in binary format */
    struct GfxImageData {
        Ref<Data> data;
        int       width  = 0;
        int       height = 0;
        int       depth  = 0;
        GfxFormat format = GfxFormat::RGBA8;
    };

    /** @brief Config to dispatch a draw */
    struct GfxDrawCall {
        int count     = 0;
        int base      = 0;
        int instances = 0;

        bool operator==(const GfxDrawCall& other) const {
            return count == other.count &&
                   base == other.base &&
                   instances == other.instances;
        }
    };

    /** @brief Gfx device capabilities */
    struct GfxDeviceCaps {
        int   max_vertex_attributes          = -1;
        int   max_texture_array_layers       = -1;
        int   max_texture_3d_size            = -1;
        int   max_texture_2d_size            = -1;
        int   max_texture_1d_size            = -1;
        int   max_shader_uniform_buffers     = -1;
        int   max_shader_storage_buffers     = -1;
        int   max_shader_sampled_textures    = -1;
        int   max_color_attachments          = -1;
        int   max_framebuffer_width          = -1;
        int   max_framebuffer_height         = -1;
        float max_anisotropy                 = 0.0f;
        bool  support_anisotropy             = false;
        int   uniform_block_offset_alignment = -1;
    };

}// namespace wmoge