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

    static void log_texture_compression_result(const Strid& name, const TextureDesc& desc, const TexCompressionStats& stats) {
#ifdef WG_DEBUG
        WG_LOG_INFO("compressed texture "
                    << name
                    << " dim=" << Vec3i(desc.width, desc.height, desc.depth)
                    << " array=" << desc.array_slices
                    << " fmt=" << magic_enum::enum_name(desc.format)
                    << " from=" << StringUtils::from_mem_size(stats.source_size)
                    << " to=" << StringUtils::from_mem_size(stats.result_size)
                    << " ratio=" << stats.ratio << "%");
#endif
    }

    TextureBuilder::TextureBuilder(Strid name, TextureManager* texture_manager)
        : m_builder_name(name),
          m_texture_manager(texture_manager) {
    }

    TextureBuilder& TextureBuilder::set_flags(TextureFlags flags) {
        m_flags = flags;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_image(Ref<Image> image, GfxFormat format) {
        m_source_image = std::move(image);
        m_format       = format;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_images(std::vector<Ref<Image>> images, GfxFormat format) {
        m_source_images = std::move(images);
        m_format        = format;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_mipmaps(bool mips) {
        m_mipmaps = mips;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_swizz(GfxTexSwizz swizz) {
        m_swizz = swizz;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_resize(TexResizeParams resize) {
        m_resize = resize;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_compression(TexCompressionFormat format) {
        m_compression.format = format;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_compression(TexCompressionParams compression) {
        m_compression = compression;
        return *this;
    }

    TextureBuilder& TextureBuilder::set_sampler(DefaultSampler sampler) {
        m_sampler = m_texture_manager->get_sampler(sampler);
        return *this;
    }

    TextureBuilder& TextureBuilder::set_sampler(GfxSamplerDesc sampler) {
        m_sampler = m_texture_manager->get_gfx_driver()->make_sampler(sampler, SIDDBG(sampler.to_string()));
        return *this;
    }

    Status TextureBuilder::build_2d(Ref<Texture2d>& out_texture) {
        WG_PROFILE_CPU_ASSET("TextureBuilder::build_2d");

        if (!TexResize::resize(m_resize, *m_source_image)) {
            WG_LOG_ERROR("failed to resize source image " << m_builder_name);
            return StatusCode::FailedResize;
        }

        TextureFlags flags = m_flags;
        flags.set(TextureFlag::Compressed, m_compression.format != TexCompressionFormat::Unknown);

        const int num_mips = m_mipmaps ? Image::max_mips_count(m_source_image->get_width(), m_source_image->get_height(), 1) : 1;

        TextureDesc desc;
        desc.tex_type      = GfxTex::Tex2d;
        desc.flags         = flags;
        desc.width         = m_source_image->get_width();
        desc.height        = m_source_image->get_height();
        desc.mips          = num_mips;
        desc.format_source = m_format;
        desc.format        = m_format;
        desc.swizz         = m_swizz;
        desc.sampler       = m_sampler;
        desc.images        = {m_source_image};

        if (m_mipmaps) {
            std::vector<Ref<Image>> mips;
            if (!m_texture_manager->generate_mips(desc.images, mips)) {
                WG_LOG_ERROR("failed to gen mip chain for " << m_builder_name);
                return StatusCode::Error;
            }
            std::swap(desc.images, mips);
        }

        if (m_compression.format != TexCompressionFormat::Unknown) {
            if (!m_texture_manager->generate_compressed_data(desc.images, m_format, m_compression, desc.compressed, desc.format, m_compression_stats)) {
                WG_LOG_ERROR("failed to compress data for " << m_builder_name);
                return StatusCode::Error;
            }
            if (m_log) {
                log_texture_compression_result(m_builder_name, desc, m_compression_stats);
            }
        }

        out_texture = m_texture_manager->create_texture_2d(desc);
        if (!out_texture) {
            WG_LOG_ERROR("failed to instantiate texture " << m_builder_name);
            return StatusCode::FailedInstantiate;
        }

        m_texture_manager->queue_texture_upload(out_texture.get());

        return WG_OK;
    }

    Status TextureBuilder::build_cube(Ref<TextureCube>& out_texture) {
        WG_PROFILE_CPU_ASSET("TextureBuilder::build_cube");

        for (Ref<Image>& source_image : m_source_images) {
            if (!TexResize::resize(m_resize, *source_image)) {
                WG_LOG_ERROR("failed to resize source image " << m_builder_name);
                return StatusCode::FailedResize;
            }
        }

        TextureFlags flags;
        flags.set(TextureFlag::Compressed, m_compression.format != TexCompressionFormat::Unknown);

        const int num_mips = m_mipmaps ? Image::max_mips_count(m_source_images[0]->get_width(), m_source_images[0]->get_height(), 1) : 1;

        TextureDesc desc;
        desc.tex_type      = GfxTex::TexCube;
        desc.flags         = flags;
        desc.width         = m_source_images[0]->get_width();
        desc.height        = m_source_images[0]->get_height();
        desc.array_slices  = 6;
        desc.mips          = num_mips;
        desc.format_source = m_format;
        desc.format        = m_format;
        desc.swizz         = m_swizz;
        desc.sampler       = m_sampler;
        desc.images        = std::move(m_source_images);

        if (m_mipmaps) {
            std::vector<Ref<Image>> mips;
            if (!m_texture_manager->generate_mips(desc.images, mips)) {
                WG_LOG_ERROR("failed to gen mip chain for " << m_builder_name);
                return StatusCode::Error;
            }
            std::swap(desc.images, mips);
        }

        if (m_compression.format != TexCompressionFormat::Unknown) {
            if (!m_texture_manager->generate_compressed_data(desc.images, m_format, m_compression, desc.compressed, desc.format, m_compression_stats)) {
                WG_LOG_ERROR("failed to compress data for " << m_builder_name);
                return StatusCode::Error;
            }
            if (m_log) {
                log_texture_compression_result(m_builder_name, desc, m_compression_stats);
            }
        }

        out_texture = m_texture_manager->create_texture_cube(desc);
        if (!out_texture) {
            WG_LOG_ERROR("failed to instantiate texture " << m_builder_name);
            return StatusCode::FailedInstantiate;
        }

        m_texture_manager->queue_texture_upload(out_texture.get());

        return WG_OK;
    }

}// namespace wmoge
