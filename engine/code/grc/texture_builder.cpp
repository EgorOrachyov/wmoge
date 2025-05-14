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

#include "texture_builder.hpp"

#include "core/log.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    static void log_texture_compression_result(const Strid& name, const TextureParams& params, const TexCompressionStats& stats) {
#ifdef WG_DEBUG
        WG_LOG_INFO("compressed texture "
                    << name
                    << " dim=" << Vec3i(params.width, params.height, params.depth)
                    << " array=" << params.array_slices
                    << " fmt=" << magic_enum::enum_name(params.format)
                    << " from=" << StringUtils::from_mem_size(stats.source_size)
                    << " to=" << StringUtils::from_mem_size(stats.result_size)
                    << " ratio=" << stats.ratio << "%");
#endif
    }

    TextureDescBuilder::TextureDescBuilder(std::string name)
        : m_name(std::move(name)) {
    }

    TextureDescBuilder& TextureDescBuilder::set_flags(TextureFlags flags) {
        m_flags = flags;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_image(Ref<Image> image, GfxFormat format) {
        m_source_image = std::move(image);
        m_format       = format;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_images(std::vector<Ref<Image>> images, GfxFormat format) {
        m_source_images = std::move(images);
        m_format        = format;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_mipmaps(bool mips) {
        m_mipmaps = mips;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_swizz(GfxTexSwizz swizz) {
        m_swizz = swizz;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_resize(TexResizeParams resize) {
        m_resize = resize;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_compression(TexCompressionFormat format) {
        m_compression.format = format;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_compression(TexCompressionParams compression) {
        m_compression = compression;
        return *this;
    }

    TextureDescBuilder& TextureDescBuilder::set_sampler(DefaultSampler sampler) {
        m_sampler = sampler;
        return *this;
    }

    Status TextureDescBuilder::build_desc_2d(TextureDesc& out_desc) {
        WG_PROFILE_CPU_ASSET("TextureDescBuilder::build_desc_2d");

        if (!TexResize::resize(m_resize, *m_source_image)) {
            WG_LOG_ERROR("failed to resize source image " << m_name);
            return StatusCode::FailedResize;
        }

        TextureFlags flags = m_flags;
        flags.set(TextureFlag::Compressed, m_compression.format != TexCompressionFormat::Unknown);

        const int num_mips = m_mipmaps ? Image::max_mips_count(m_source_image->get_width(), m_source_image->get_height(), 1) : 1;

        out_desc.name                 = m_name;
        out_desc.params.tex_type      = GfxTex::Tex2d;
        out_desc.params.flags         = flags;
        out_desc.params.width         = m_source_image->get_width();
        out_desc.params.height        = m_source_image->get_height();
        out_desc.params.mips          = num_mips;
        out_desc.params.format_source = m_format;
        out_desc.params.format        = m_format;
        out_desc.params.swizz         = m_swizz;
        out_desc.sampler              = m_sampler;
        out_desc.images               = {m_source_image};

        if (m_mipmaps) {
            std::vector<Ref<Image>> mips;
            if (!Image::generate_mips(out_desc.images, mips)) {
                WG_LOG_ERROR("failed to gen mip chain for " << m_name);
                return StatusCode::Error;
            }
            std::swap(out_desc.images, mips);
        }

        if (m_compression.format != TexCompressionFormat::Unknown) {
            if (!TexCompression::compress(out_desc.images, m_format, m_compression, out_desc.compressed, out_desc.params.format, m_compression_stats)) {
                WG_LOG_ERROR("failed to compress data for " << m_name);
                return StatusCode::Error;
            }
            if (m_log) {
                log_texture_compression_result(m_name, out_desc.params, m_compression_stats);
            }
        }

        return WG_OK;
    }

    Status TextureDescBuilder::build_desc_cube(TextureDesc& out_desc) {
        WG_PROFILE_CPU_ASSET("TextureDescBuilder::build_desc_cube");

        for (Ref<Image>& source_image : m_source_images) {
            if (!TexResize::resize(m_resize, *source_image)) {
                WG_LOG_ERROR("failed to resize source image " << m_name);
                return StatusCode::FailedResize;
            }
        }

        TextureFlags flags;
        flags.set(TextureFlag::Compressed, m_compression.format != TexCompressionFormat::Unknown);

        const int num_mips = m_mipmaps ? Image::max_mips_count(m_source_images[0]->get_width(), m_source_images[0]->get_height(), 1) : 1;

        out_desc.name                 = m_name;
        out_desc.params.tex_type      = GfxTex::TexCube;
        out_desc.params.flags         = flags;
        out_desc.params.width         = m_source_images[0]->get_width();
        out_desc.params.height        = m_source_images[0]->get_height();
        out_desc.params.array_slices  = 6;
        out_desc.params.mips          = num_mips;
        out_desc.params.format_source = m_format;
        out_desc.params.format        = m_format;
        out_desc.params.swizz         = m_swizz;
        out_desc.sampler              = m_sampler;
        out_desc.images               = std::move(m_source_images);

        if (m_mipmaps) {
            std::vector<Ref<Image>> mips;
            if (!Image::generate_mips(out_desc.images, mips)) {
                WG_LOG_ERROR("failed to gen mip chain for " << m_name);
                return StatusCode::Error;
            }
            std::swap(out_desc.images, mips);
        }

        if (m_compression.format != TexCompressionFormat::Unknown) {
            if (!TexCompression::compress(out_desc.images, m_format, m_compression, out_desc.compressed, out_desc.params.format, m_compression_stats)) {
                WG_LOG_ERROR("failed to compress data for " << m_name);
                return StatusCode::Error;
            }
            if (m_log) {
                log_texture_compression_result(m_name, out_desc.params, m_compression_stats);
            }
        }

        return WG_OK;
    }

    TextureBuilder::TextureBuilder(std::string name, TextureManager* texture_manager)
        : TextureDescBuilder(std::move(name)),
          m_texture_manager(texture_manager) {
    }

    Status TextureBuilder::build_2d(Ref<Texture2d>& out_texture) {
        WG_PROFILE_CPU_ASSET("TextureBuilder::build_2d");

        TextureDesc desc;
        WG_CHECKED(build_desc_2d(desc));

        out_texture = m_texture_manager->create_texture_2d(desc);
        if (!out_texture) {
            WG_LOG_ERROR("failed to instantiate texture " << m_name);
            return StatusCode::FailedInstantiate;
        }

        m_texture_manager->queue_texture_upload(out_texture.get());

        return WG_OK;
    }

    Status TextureBuilder::build_cube(Ref<TextureCube>& out_texture) {
        WG_PROFILE_CPU_ASSET("TextureBuilder::build_cube");

        TextureDesc desc;
        WG_CHECKED(build_desc_cube(desc));

        out_texture = m_texture_manager->create_texture_cube(desc);
        if (!out_texture) {
            WG_LOG_ERROR("failed to instantiate texture " << m_name);
            return StatusCode::FailedInstantiate;
        }

        m_texture_manager->queue_texture_upload(out_texture.get());

        return WG_OK;
    }

}// namespace wmoge
