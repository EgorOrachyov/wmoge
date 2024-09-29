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

#include "texture.hpp"

#include "asset/asset_manager.hpp"
#include "core/data.hpp"
#include "core/string_utils.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/tree.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    Texture::~Texture() {
        if (m_callback) {
            (*m_callback)(this);
        }
    }

    Texture::Texture(TextureFlags flags, GfxTextureDesc desc) {
        m_flags        = flags;
        m_format       = desc.format;
        m_width        = desc.width;
        m_height       = desc.height;
        m_depth        = desc.depth;
        m_array_slices = desc.array_slices;
        m_mips         = desc.mips_count;
        m_swizz        = desc.swizz;
    }

    void Texture::set_source_images(std::vector<Ref<Image>> images) {
        m_images = std::move(images);
    }
    void Texture::set_sampler(const Ref<GfxSampler>& sampler) {
        m_sampler = sampler;
    }
    void Texture::set_compression(const TexCompressionParams& params) {
        m_compression = params;
    }
    void Texture::set_flags(const TextureFlags& flags) {
        m_flags = flags;
    }
    void Texture::set_texture_callback(CallbackRef callback) {
        m_callback = callback;
    }

    Status Texture::generate_mips() {
        WG_AUTO_PROFILE_ASSET("Texture::generate_mips");

        std::vector<Ref<Image>> mips;

        for (auto& image : m_images) {
            std::vector<Ref<Image>> face_mips;

            if (!image->generate_mip_chain(face_mips)) {
                WG_LOG_ERROR("failed to gen mip chain for texture " << get_name());
                return StatusCode::Error;
            }

            for (auto& mip : face_mips) {
                mips.push_back(mip);
            }

            assert(m_mips == 1 || m_mips == int(face_mips.size()));
            m_mips = int(face_mips.size());
        }

        m_images = std::move(mips);
        return WG_OK;
    }
    Status Texture::generate_compressed_data() {
        WG_AUTO_PROFILE_ASSET("Texture::generate_compressed_data");

        if (!m_flags.get(TextureFlag::Compressed)) {
            WG_LOG_INFO("no compression flag on texutre " << get_name());
            return StatusCode::InvalidState;
        }
        if (m_compression.format == TexCompressionFormat::Unknown) {
            WG_LOG_INFO("no compression setup for texture " << get_name());
            return StatusCode::InvalidState;
        }
        if (m_images.empty()) {
            WG_LOG_INFO("no source to compress " << get_name());
            return StatusCode::InvalidState;
        }

        std::vector<GfxImageData> source_data;
        std::vector<GfxImageData> dest_data;

        source_data.reserve(m_images.size());

        for (auto& image : m_images) {
            GfxImageData data;
            data.format = m_format;
            data.depth  = 1;
            data.width  = image->get_width();
            data.height = image->get_height();
            data.data   = image->get_pixel_data();
            source_data.push_back(data);
        }

        if (!TexCompression::compress(m_compression, source_data, dest_data)) {
            WG_LOG_ERROR("failed to compress texture " << get_name());
            return StatusCode::Error;
        }

        assert(source_data.size() == dest_data.size());

#ifdef WG_DEBUG
        std::size_t size_original   = 0;
        std::size_t size_compressed = 0;

        for (auto& entry : source_data) {
            size_original += entry.data->size();
        }
        for (auto& entry : dest_data) {
            size_compressed += entry.data->size();
        }

        WG_LOG_INFO("compressed texture "
                    << get_name()
                    << " dim=" << Vec3i(m_width, m_height, m_depth)
                    << " array=" << m_array_slices
                    << " fmt=" << magic_enum::enum_name(m_compression.format)
                    << " from=" << StringUtils::from_mem_size(size_original)
                    << " to=" << StringUtils::from_mem_size(size_compressed));
#endif

        m_format_compressed = dest_data.front().format;
        m_compressed        = std::move(dest_data);

        return WG_OK;
    }

    Status Texture::create_gfx_resource(TexturePool& pool) {
        WG_AUTO_PROFILE_ASSET("Texture::create_gfx_resource");
        assert(m_flags.get(TextureFlag::Pooled));

        const GfxTextureDesc desc = get_desc();
        m_texture                 = pool.allocate(desc, get_name());

        return WG_OK;
    }

    Status Texture::delete_gfx_resource(TexturePool& pool) {
        WG_AUTO_PROFILE_ASSET("Texture::delete_gfx_resource");
        assert(m_flags.get(TextureFlag::Pooled));

        pool.release(m_texture);
        m_texture.reset();

        return Status();
    }

    Status Texture::upload_gfx_data(GfxCmdListRef& cmd) {
        assert(m_depth == 1);
        assert(m_array_slices >= 1);
        assert(m_mips >= 1);

        const bool is_compressed = m_flags.get(TextureFlag::Compressed);
        assert(!is_compressed || m_format_compressed != GfxFormat::Unknown);
        assert(is_compressed || m_format_compressed == GfxFormat::Unknown);

        for (int array_slice = 0; array_slice < m_array_slices; array_slice++) {
            for (int mip = 0; mip < m_mips; mip++) {
                const int index = array_slice * m_mips + mip;

                Ref<Data> data = m_images[index]->get_pixel_data();
                Rect2i    rect = {0, 0, m_images[index]->get_width(), m_images[index]->get_height()};

                if (is_compressed) {
                    data = m_compressed[index].data;
                    rect = {0, 0, m_compressed[index].width, m_compressed[index].height};
                }

                array_view<const std::uint8_t> buffer(data->buffer(), data->size());

                switch (m_tex_type) {
                    case GfxTex::Tex2d:
                        cmd->update_texture_2d(m_texture, mip, rect, buffer);
                        continue;
                    case GfxTex::Tex2dArray:
                        cmd->update_texture_2d_array(m_texture, mip, array_slice, rect, buffer);
                        continue;
                    case GfxTex::TexCube:
                        cmd->update_texture_cube(m_texture, mip, array_slice, rect, buffer);
                        continue;
                    default:
                        assert(false);
                }
            }
        }

        return WG_OK;
    }

    GfxTextureDesc Texture::get_desc() const {
        GfxTextureDesc desc;
        desc.width        = m_width;
        desc.height       = m_height;
        desc.depth        = m_depth;
        desc.array_slices = m_array_slices;
        desc.mips_count   = m_mips;
        desc.mem_usage    = m_mem_usage;
        desc.usages       = m_usages;
        desc.swizz        = m_swizz;
        desc.format       = m_format_compressed != GfxFormat::Unknown ? m_format_compressed : m_format;
        desc.tex_type     = m_tex_type;
        return desc;
    }

    Texture2d::Texture2d(TextureFlags flags, GfxTextureDesc desc) : Texture(flags, desc) {
    }

    TextureCube::TextureCube(TextureFlags flags, GfxTextureDesc desc) : Texture(flags, desc) {
    }

}// namespace wmoge