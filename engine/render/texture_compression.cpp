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

#include "texture_compression.hpp"

#include <compressonator.h>

namespace wmoge {

    WG_IO_BEGIN(TexCompressionParams)
    WG_IO_FIELD_OPT(format)
    WG_IO_FIELD_OPT(use_channel_weighting)
    WG_IO_FIELD_OPT(weight_red)
    WG_IO_FIELD_OPT(weight_green)
    WG_IO_FIELD_OPT(weight_blue)
    WG_IO_FIELD_OPT(use_adaptive_weighting)
    WG_IO_FIELD_OPT(use_alpha)
    WG_IO_FIELD_OPT(alpha_threshold)
    WG_IO_FIELD_OPT(fquality)
    WG_IO_FIELD_OPT(num_threads)
    WG_IO_END(TexCompressionParams)

    static CMP_FORMAT get_source_format(GfxFormat format) {
        switch (format) {
            case GfxFormat::R8:
                return CMP_FORMAT_R_8;
            case GfxFormat::R8_SNORM:
                return CMP_FORMAT_R_8_S;
            case GfxFormat::R16:
                return CMP_FORMAT_R_16;
            case GfxFormat::R16_SNORM:
                return CMP_FORMAT_Unknown;
            case GfxFormat::RG8:
                return CMP_FORMAT_RG_8;
            case GfxFormat::RG8_SNORM:
                return CMP_FORMAT_RG_8_S;
            case GfxFormat::RG16:
                return CMP_FORMAT_RG_16;
            case GfxFormat::RG16_SNORM:
                return CMP_FORMAT_Unknown;
            case GfxFormat::RGB8:
                return CMP_FORMAT_RGB_888;
            case GfxFormat::RGB8_SNORM:
                return CMP_FORMAT_RGB_888_S;
            case GfxFormat::RGB16_SNORM:
                return CMP_FORMAT_Unknown;
            case GfxFormat::RGBA8:
                return CMP_FORMAT_RGBA_8888;
            case GfxFormat::RGBA8_SNORM:
                return CMP_FORMAT_RGBA_8888_S;
            case GfxFormat::RGBA16:
                return CMP_FORMAT_RGBA_16;
            case GfxFormat::SRGB8:
                return CMP_FORMAT_RGB_888;
            case GfxFormat::SRGB8_ALPHA8:
                return CMP_FORMAT_RGBA_8888;
            case GfxFormat::SBGR8_ALPHA8:
                return CMP_FORMAT_BGRA_8888;
            case GfxFormat::R16F:
                return CMP_FORMAT_R_16F;
            case GfxFormat::RG16F:
                return CMP_FORMAT_RG_16F;
            case GfxFormat::RGB16F:
                return CMP_FORMAT_Unknown;
            case GfxFormat::RGBA16F:
                return CMP_FORMAT_RGBA_16F;
            case GfxFormat::R32F:
                return CMP_FORMAT_R_32F;
            case GfxFormat::RG32F:
                return CMP_FORMAT_RG_32F;
            case GfxFormat::RGB32F:
                return CMP_FORMAT_RGB_32F;
            case GfxFormat::RGBA32F:
                return CMP_FORMAT_RGBA_32F;
            default:
                return CMP_FORMAT_Unknown;
        }
    }

    static int get_source_px_size(GfxFormat format) {
        switch (format) {
            case GfxFormat::R8:
                return 1;
            case GfxFormat::R8_SNORM:
                return 1;
            case GfxFormat::R16:
                return 2;
            case GfxFormat::R16_SNORM:
                return 2;
            case GfxFormat::RG8:
                return 2;
            case GfxFormat::RG8_SNORM:
                return 2;
            case GfxFormat::RG16:
                return 4;
            case GfxFormat::RG16_SNORM:
                return 4;
            case GfxFormat::RGB8:
                return 3;
            case GfxFormat::RGB8_SNORM:
                return 3;
            case GfxFormat::RGB16_SNORM:
                return 6;
            case GfxFormat::RGBA8:
                return 4;
            case GfxFormat::RGBA8_SNORM:
                return 4;
            case GfxFormat::RGBA16:
                return 8;
            case GfxFormat::SRGB8:
                return 3;
            case GfxFormat::SRGB8_ALPHA8:
                return 4;
            case GfxFormat::SBGR8_ALPHA8:
                return 4;
            case GfxFormat::R16F:
                return 2;
            case GfxFormat::RG16F:
                return 4;
            case GfxFormat::RGB16F:
                return 6;
            case GfxFormat::RGBA16F:
                return 8;
            case GfxFormat::R32F:
                return 4;
            case GfxFormat::RG32F:
                return 8;
            case GfxFormat::RGB32F:
                return 12;
            case GfxFormat::RGBA32F:
                return 16;
            default:
                return 0;
        }
    }

    GfxFormat get_gfx_format(TexCompressionFormat format) {
        switch (format) {
            case TexCompressionFormat::BC1_RGB:
                return GfxFormat::BC1_RGB;
            case TexCompressionFormat::BC1_RGB_SRGB:
                return GfxFormat::BC1_RGB_SRGB;
            case TexCompressionFormat::BC1_RGBA:
                return GfxFormat::BC1_RGBA;
            case TexCompressionFormat::BC1_RGBA_SRGB:
                return GfxFormat::BC1_RGBA_SRGB;
            case TexCompressionFormat::BC2:
                return GfxFormat::BC2;
            case TexCompressionFormat::BC2_SRGB:
                return GfxFormat::BC2_SRGB;
            case TexCompressionFormat::BC3:
                return GfxFormat::BC3;
            case TexCompressionFormat::BC3_SRGB:
                return GfxFormat::BC3_SRGB;
            case TexCompressionFormat::BC4:
                return GfxFormat::BC4;
            case TexCompressionFormat::BC4_SNORM:
                return GfxFormat::BC4_SNORM;
            case TexCompressionFormat::BC5:
                return GfxFormat::BC5;
            case TexCompressionFormat::BC5_SNORM:
                return GfxFormat::BC5_SNORM;
            case TexCompressionFormat::BC6H_UFLOAT:
                return GfxFormat::BC6H_UFLOAT;
            case TexCompressionFormat::BC6H_SFLOAT:
                return GfxFormat::BC6H_SFLOAT;
            case TexCompressionFormat::BC7:
                return GfxFormat::BC7;
            case TexCompressionFormat::BC7_SRGB:
                return GfxFormat::BC7_SRGB;
            default:
                return GfxFormat::Unknown;
        }
    }

    CMP_FORMAT get_compressed_format(TexCompressionFormat format) {
        switch (format) {
            case TexCompressionFormat::BC1_RGB:
            case TexCompressionFormat::BC1_RGB_SRGB:
            case TexCompressionFormat::BC1_RGBA:
            case TexCompressionFormat::BC1_RGBA_SRGB:
                return CMP_FORMAT_BC1;
            case TexCompressionFormat::BC2:
            case TexCompressionFormat::BC2_SRGB:
                return CMP_FORMAT_BC2;
            case TexCompressionFormat::BC3:
            case TexCompressionFormat::BC3_SRGB:
                return CMP_FORMAT_BC3;
            case TexCompressionFormat::BC4:
                return CMP_FORMAT_BC4;
            case TexCompressionFormat::BC4_SNORM:
                return CMP_FORMAT_BC4_S;
            case TexCompressionFormat::BC5:
                return CMP_FORMAT_BC5;
            case TexCompressionFormat::BC5_SNORM:
                return CMP_FORMAT_BC5_S;
            case TexCompressionFormat::BC6H_UFLOAT:
                return CMP_FORMAT_BC6H;
            case TexCompressionFormat::BC6H_SFLOAT:
                return CMP_FORMAT_BC6H_SF;
            case TexCompressionFormat::BC7:
            case TexCompressionFormat::BC7_SRGB:
                return CMP_FORMAT_BC7;
            default:
                return CMP_FORMAT_Unknown;
        }
    }

    const char* get_error(CMP_ERROR error) {
        switch (error) {
            case CMP_OK:
                return "Ok";
            case CMP_ABORTED:
                return "The conversion was aborted";
            case CMP_ERR_INVALID_SOURCE_TEXTURE:
                return "The source texture is invalid";
            case CMP_ERR_INVALID_DEST_TEXTURE:
                return "The destination texture is invalid";
            case CMP_ERR_UNSUPPORTED_SOURCE_FORMAT:
                return "The source format is not a supported format";
            case CMP_ERR_UNSUPPORTED_DEST_FORMAT:
                return "The destination format is not a supported format";
            case CMP_ERR_UNSUPPORTED_GPU_ASTC_DECODE:
                return "The gpu hardware is not supported";
            case CMP_ERR_UNSUPPORTED_GPU_BASIS_DECODE:
                return "The gpu hardware is not supported";
            case CMP_ERR_SIZE_MISMATCH:
                return "The source and destination texture sizes do not match";
            case CMP_ERR_UNABLE_TO_INIT_CODEC:
                return "Compressonator was unable to initialize the codec needed for conversion";
            case CMP_ERR_UNABLE_TO_INIT_DECOMPRESSLIB:
                return "GPU_Decode Lib was unable to initialize the codec needed for decompression ";
            case CMP_ERR_UNABLE_TO_INIT_COMPUTELIB:
                return "Compute Lib was unable to initialize the codec needed for compression";
            case CMP_ERR_CMP_DESTINATION:
                return "Error in compressing destination texture";
            case CMP_ERR_MEM_ALLOC_FOR_MIPSET:
                return "Memory Error: allocating MIPSet compression level data buffer";
            case CMP_ERR_UNKNOWN_DESTINATION_FORMAT:
                return "The destination Codec Type is unknown! In SDK refer to GetCodecType()";
            case CMP_ERR_FAILED_HOST_SETUP:
                return "Failed to setup Host for processing";
            case CMP_ERR_PLUGIN_FILE_NOT_FOUND:
                return "The required plugin library was not found";
            case CMP_ERR_UNABLE_TO_LOAD_FILE:
                return "The requested file was not loaded";
            case CMP_ERR_UNABLE_TO_CREATE_ENCODER:
                return "Request to create an encoder failed";
            case CMP_ERR_UNABLE_TO_LOAD_ENCODER:
                return "Unable to load an encode library";
            case CMP_ERR_NOSHADER_CODE_DEFINED:
                return "No shader code is available for the requested framework";
            case CMP_ERR_GPU_DOESNOT_SUPPORT_COMPUTE:
                return "The GPU device selected does not support compute";
            case CMP_ERR_NOPERFSTATS:
                return "No Performance Stats are available";
            case CMP_ERR_GPU_DOESNOT_SUPPORT_CMP_EXT:
                return "The GPU does not support the requested compression extension!";
            case CMP_ERR_GAMMA_OUTOFRANGE:
                return "Gamma value set for processing is out of range";
            case CMP_ERR_PLUGIN_SHAREDIO_NOT_SET:
                return "The plugin C_PluginSetSharedIO call was not set and is required for this plugin to operate";
            case CMP_ERR_UNABLE_TO_INIT_D3DX:
                return "Unable to initialize DirectX SDK or get a specific DX API";
            case CMP_FRAMEWORK_NOT_INITIALIZED:
                return "CMP_InitFramework failed or not called";
            default:
                return "An unknown error occurred";
        }
    }

    Status TexCompression::compress(const TexCompressionParams& params, std::vector<GfxImageData>& source, std::vector<GfxImageData>& compressed) {
        if (params.format == TexCompressionFormat::Unknown) {
            return StatusCode::InvalidParameter;
        }
        if (source.empty()) {
            return StatusCode::Ok;
        }

        compressed.clear();
        compressed.reserve(source.size());

        for (const auto& entry : source) {
            CMP_Texture cmp_source_texture{};
            cmp_source_texture.dwSize     = sizeof(cmp_source_texture);
            cmp_source_texture.dwWidth    = entry.width;
            cmp_source_texture.dwHeight   = entry.height;
            cmp_source_texture.dwPitch    = entry.width * get_source_px_size(entry.format);
            cmp_source_texture.format     = get_source_format(entry.format);
            cmp_source_texture.dwDataSize = CMP_DWORD(entry.data->size());
            cmp_source_texture.pData      = entry.data->buffer();

            CMP_Texture cmd_dest_texture{};
            cmd_dest_texture.dwSize   = sizeof(cmd_dest_texture);
            cmd_dest_texture.dwWidth  = entry.width;
            cmd_dest_texture.dwHeight = entry.height;
            cmd_dest_texture.dwPitch  = 0;
            cmd_dest_texture.format   = get_compressed_format(params.format);

            const auto out_data_size = CMP_CalculateBufferSize(&cmd_dest_texture);
            const auto out_data      = make_ref<Data>(out_data_size);

            cmd_dest_texture.dwDataSize = out_data_size;
            cmd_dest_texture.pData      = out_data->buffer();

            CMP_CompressOptions cmp_options{};
            cmp_options.dwSize                = sizeof(cmp_options);
            cmp_options.bDXT1UseAlpha         = params.use_alpha;
            cmp_options.nAlphaThreshold       = params.alpha_threshold;
            cmp_options.bUseChannelWeighting  = params.use_channel_weighting;
            cmp_options.fWeightingRed         = params.weight_red;
            cmp_options.fWeightingGreen       = params.weight_green;
            cmp_options.fWeightingBlue        = params.weight_blue;
            cmp_options.bUseAdaptiveWeighting = params.use_adaptive_weighting;
            cmp_options.fquality              = params.fquality;
            cmp_options.dwnumThreads          = params.num_threads;

            CMP_ERROR cmp_status;
            cmp_status = CMP_ConvertTexture(&cmp_source_texture, &cmd_dest_texture, &cmp_options, nullptr);

            if (cmp_status != CMP_OK) {
                WG_LOG_ERROR("failed to compress texture with error: " << get_error(cmp_status));
                return StatusCode::Error;
            }

            GfxImageData compressed_image;
            compressed_image.data   = out_data;
            compressed_image.width  = entry.width;
            compressed_image.height = entry.height;
            compressed_image.depth  = entry.depth;
            compressed_image.format = get_gfx_format(params.format);

            compressed.push_back(compressed_image);
        }

        return StatusCode::Ok;
    }

}// namespace wmoge