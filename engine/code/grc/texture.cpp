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
#include "grc/texture_import_settings.hpp"
#include "grc/texture_loader.hpp"
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

    Texture::Texture(TextureDesc&& desc)
        : m_desc(std::move(desc)) {
    }

    void Texture::set_source_images(std::vector<Ref<Image>> images, GfxFormat format) {
        m_desc.images               = std::move(images);
        m_desc.params.format_source = format;
    }

    void Texture::set_texture(const Ref<GfxTexture>& texture) {
        m_texture = texture;
    }

    void Texture::set_sampler(const Ref<GfxSampler>& sampler) {
        m_sampler = sampler;
    }

    void Texture::set_texture_callback(CallbackRef callback) {
        m_callback = callback;
    }

    GfxTextureDesc Texture::get_gfx_desc() const {
        GfxTextureDesc desc;
        desc.width        = m_desc.params.width;
        desc.height       = m_desc.params.height;
        desc.depth        = m_desc.params.depth;
        desc.array_slices = m_desc.params.array_slices;
        desc.mips_count   = m_desc.params.mips;
        desc.mem_usage    = m_desc.params.mem_usage;
        desc.usages       = m_desc.params.usages;
        desc.swizz        = m_desc.params.swizz;
        desc.format       = m_desc.params.format;
        desc.tex_type     = m_desc.params.tex_type;
        return desc;
    }

    Texture2d::Texture2d(TextureDesc&& desc) : Texture(std::move(desc)) {
    }

    TextureCube::TextureCube(TextureDesc&& desc) : Texture(std::move(desc)) {
    }

    void rtti_grc_texture() {
        rtti_type<TextureDesc>();
        rtti_type<Texture>();
        rtti_type<Texture2d>();
        rtti_type<TextureCube>();
        rtti_type<TextureImportSettings>();
        rtti_type<Texture2dImportSettings>();
        rtti_type<TextureCubeImportSettings>();
        rtti_type<Texture2dLoader>();
        rtti_type<TextureCubeLoader>();
    }

}// namespace wmoge