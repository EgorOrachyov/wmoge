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
#include "core/status.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/image.hpp"
#include "rtti/traits.hpp"

#include <vector>

namespace wmoge {

    /**
     * @brief Supported types of block compression for textures
     */
    enum class TexCompressionType {
        Unknown = 0,
        /**
         * @brief BC1 Mode
         *
         *  Type of data:   RGB + optional 1-bit alpha
         *  Data rate:      0.5 byte/px
         *  Pallet size:    4
         *  Line segments:  1
         *  Use for:        Color maps, Cutout color maps (1-bit alpha), Normal maps, if memory is tight
         */
        BC1,
        /**
         * @brief BC2 Mode
         *
         *  Type of data:   RGB + 4-bit alpha
         *  Data rate:      1 byte/px
         *  Pallet size:    4
         *  Line segments:  1
         *  Use for:        n/a
         */
        BC2,
        /**
         * @brief BC3 Mode
         *
         *  Type of data:   RGBA
         *  Data rate:      1 byte/px
         *  Pallet size:    4 color + 8 alpha
         *  Line segments:  1 color + 1 alpha
         *  Use for:        Color maps with full alpha, Packing color and mono maps together
         */
        BC3,
        /**
         * @brief BC4 Mode
         *
         *  Type of data:   Grayscale
         *  Data rate:      0.5 byte/px
         *  Pallet size:    8
         *  Line segments:  1
         *  Use for:        Height maps, Gloss maps, Font atlases, Any grayscale image
         */
        BC4,
        /**
         * @brief BC5 Mode
         *
         *  Type of data:   2 × grayscale
         *  Data rate:      1 byte/px
         *  Pallet size:    8 per channel
         *  Line segments:  1 per channel
         *  Use for:        Tangent-space normal maps
         */
        BC5,
        /**
         * @brief BC6 Mode
         *
         *  Type of data:   RGB, floating-point
         *  Data rate:      1 byte/px
         *  Pallet size:    8–16
         *  Line segments:  1–2
         *  Use for:        HDR images
         */
        BC6,
        /**
         * @brief BC7 Mode
         *
         *  Type of data:   RGB or RGBA
         *  Data rate:      1 byte/px
         *  Pallet size:    4–16
         *  Line segments:  1–3
         *  Use for:        High-quality color maps, Color maps with full alpha
         */
        BC7
    };

    /**
     * @brief Formats of compressed textures
     */
    enum class TexCompressionFormat {
        Unknown,
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
        BC7_SRGB
    };

    /**
     * @class TexCompressionParams
     * @brief Set of parameters to compress texture data
     */
    struct TexCompressionParams {
        WG_RTTI_STRUCT(TexCompressionParams);

        TexCompressionFormat format                 = TexCompressionFormat::Unknown;
        bool                 use_channel_weighting  = false;
        float                weight_red             = 0.3f;
        float                weight_green           = 0.4f;
        float                weight_blue            = 0.3f;
        bool                 use_adaptive_weighting = false;
        bool                 use_alpha              = false;
        int                  alpha_threshold        = 128;
        float                fquality               = 0.05f;
        int                  num_threads            = 4;
    };

    WG_RTTI_STRUCT_BEGIN(TexCompressionParams) {
        WG_RTTI_FIELD(format, {RttiOptional});
        WG_RTTI_FIELD(use_channel_weighting, {RttiOptional});
        WG_RTTI_FIELD(weight_red, {RttiOptional});
        WG_RTTI_FIELD(weight_green, {RttiOptional});
        WG_RTTI_FIELD(weight_blue, {RttiOptional});
        WG_RTTI_FIELD(use_adaptive_weighting, {RttiOptional});
        WG_RTTI_FIELD(use_alpha, {RttiOptional});
        WG_RTTI_FIELD(alpha_threshold, {RttiOptional});
        WG_RTTI_FIELD(fquality, {RttiOptional});
        WG_RTTI_FIELD(num_threads, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class TexCompressionStats
     * @brief Stats returned after compression
     */
    struct TexCompressionStats {
        std::size_t source_size = 0;
        std::size_t result_size = 0;
        float       ratio       = 0.0f;
    };

    /**
     * @class TexCompression
     * @brief Provides methods for texture data compression
     */
    class TexCompression {
    public:
        /**
         * @brief Compress a list of image data using provided setup
         *
         * @param params Structure holding parameters for compression
         * @param source Vector with source data to compress
         * @param compressed Vector with a compressed daa
         * @param stats Stats
         *
         * @return Ok on success
         */
        static Status compress(const TexCompressionParams& params, std::vector<GfxImageData>& source, std::vector<GfxImageData>& compressed, TexCompressionStats& stats);
        static Status compress(const std::vector<Ref<Image>>& images, GfxFormat format, const TexCompressionParams& params, std::vector<GfxImageData>& compressed, GfxFormat& format_compressed, TexCompressionStats& stats);
    };

}// namespace wmoge