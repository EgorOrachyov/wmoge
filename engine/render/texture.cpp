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
#include "io/yaml.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

#include <cassert>

namespace wmoge {

    Texture::Texture(GfxFormat format, int width, int height, int depth, int array_slices, GfxTexSwizz swizz) {
        m_format       = format;
        m_width        = width;
        m_height       = height;
        m_depth        = depth;
        m_array_slices = array_slices;
        m_mips         = 1;
        m_swizz        = swizz;
    }

    void Texture::set_source_images(std::vector<Ref<Image>> images) {
        m_images = std::move(images);
    }
    void Texture::set_sampler(const Ref<GfxSampler>& sampler) {
        m_sampler = sampler;
    }
    void Texture::set_sampler_from_desc(const GfxSamplerDesc& desc) {
        set_sampler(Engine::instance()->gfx_driver()->make_sampler(desc, SID(desc.to_str())));
    }
    void Texture::set_compression(const GrcTexCompressionParams& params) {
        m_compression = params;
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
        return StatusCode::Ok;
    }
    Status Texture::generate_compressed_data() {
        WG_AUTO_PROFILE_ASSET("Texture::generate_compressed_data");

        if (m_compression.format == GrcTexCompressionFormat::Unknown) {
            WG_LOG_INFO("no compression setup for texture " << get_name());
            return StatusCode::Ok;
        }
        if (m_images.empty()) {
            WG_LOG_INFO("no source to compress " << get_name());
            return StatusCode::Ok;
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

        if (!GrcTexCompression::compress(m_compression, source_data, dest_data)) {
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

        return StatusCode::Ok;
    }
    Status Texture::generate_gfx_resource() {
        WG_AUTO_PROFILE_ASSET("Texture::generate_gfx_resource");

        if (!m_sampler) {
            GfxSamplerDesc sampler_desc{};
            set_sampler_from_desc(sampler_desc);
        }

        GfxFormat format     = m_format;
        bool      compressed = false;

        if (!m_compressed.empty() && m_compression.format != GrcTexCompressionFormat::Unknown) {
            format     = m_format_compressed;
            compressed = true;
        }

        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();

        switch (m_tex_type) {
            case GfxTex::Tex2d:
                m_texture = gfx_driver->make_texture_2d(m_width, m_height, m_mips, format, m_usages, m_mem_usage, m_swizz, get_name());
                break;
            case GfxTex::Tex2dArray:
                m_texture = gfx_driver->make_texture_2d_array(m_width, m_height, m_mips, m_array_slices, format, m_usages, m_mem_usage, get_name());
                break;
            case GfxTex::TexCube:
                m_texture = gfx_driver->make_texture_cube(m_width, m_height, m_mips, format, m_usages, m_mem_usage, get_name());
                break;
            default:
                WG_LOG_ERROR("unknown texture gfx type " << get_name());
                return StatusCode::InvalidParameter;
        }

        assert(m_depth == 1);
        assert(m_array_slices >= 1);
        assert(m_mips >= 1);

        for (int array_slice = 0; array_slice < m_array_slices; array_slice++) {
            for (int mip = 0; mip < m_mips; mip++) {
                const int index = array_slice * m_mips + mip;

                Ref<Data> data = m_images[index]->get_pixel_data();
                Rect2i    rect = {0, 0, m_images[index]->get_width(), m_images[index]->get_height()};

                if (compressed) {
                    data = m_compressed[index].data;
                    rect = {0, 0, m_compressed[index].width, m_compressed[index].height};
                }

                switch (m_tex_type) {
                    case GfxTex::Tex2d:
                        gfx_ctx->update_texture_2d(m_texture, mip, rect, data);
                        continue;
                    case GfxTex::Tex2dArray:
                        gfx_ctx->update_texture_2d_array(m_texture, mip, array_slice, rect, data);
                        continue;
                    case GfxTex::TexCube:
                        gfx_ctx->update_texture_cube(m_texture, mip, array_slice, rect, data);
                        continue;
                    default:
                        assert(false);
                }
            }
        }

        return StatusCode::Ok;
    }

    Texture2d::Texture2d(GfxFormat format, int width, int height, GfxTexSwizz swizz) : Texture(format, width, height, 1, 1, swizz) {
        m_tex_type = GfxTex::Tex2d;
    }

    TextureCube::TextureCube(GfxFormat format, int width, int height) : Texture(format, width, height, 1, 6) {
        m_tex_type = GfxTex::TexCube;
    }

}// namespace wmoge