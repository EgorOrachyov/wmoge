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
#include "core/ioc_container.hpp"
#include "core/string_utils.hpp"
#include "gfx/gfx_driver.hpp"
#include "io/tree.hpp"
#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

#include <cassert>

namespace wmoge {

    Texture::~Texture() {
        if (m_callback) {
            (*m_callback)(this);
        }
    }

    Texture::Texture(TextureDesc& desc) {
        m_images        = std::move(desc.images);
        m_compressed    = std::move(desc.compressed);
        m_texture       = desc.texture;
        m_sampler       = desc.sampler;
        m_width         = desc.width;
        m_height        = desc.height;
        m_depth         = desc.depth;
        m_array_slices  = desc.array_slices;
        m_mips          = desc.mips;
        m_format        = desc.format;
        m_format_source = desc.format_source;
        m_tex_type      = desc.tex_type;
        m_swizz         = desc.swizz;
        m_mem_usage     = desc.mem_usage;
        m_usages        = desc.usages;
        m_srgb          = desc.srgb;
        m_compression   = desc.compression;
        m_flags         = desc.flags;
    }

    void Texture::set_source_images(std::vector<Ref<Image>> images, GfxFormat format) {
        m_images        = std::move(images);
        m_format_source = format;
    }
    void Texture::set_compressed(std::vector<GfxImageData> data, const TexCompressionParams& params) {
        m_compressed  = std::move(data);
        m_compression = params;
    }
    void Texture::set_texture(const Ref<GfxTexture>& texture) {
        m_texture = texture;
    }
    void Texture::set_sampler(const Ref<GfxSampler>& sampler) {
        m_sampler = sampler;
    }
    void Texture::set_flags(const TextureFlags& flags) {
        m_flags = flags;
    }
    void Texture::set_texture_callback(CallbackRef callback) {
        m_callback = callback;
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
        desc.format       = m_format;
        desc.tex_type     = m_tex_type;
        return desc;
    }

    Texture2d::Texture2d(TextureDesc& desc) : Texture(desc) {
    }

    TextureCube::TextureCube(TextureDesc& desc) : Texture(desc) {
    }

}// namespace wmoge