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

#include "grc_texture_manager.hpp"

#include "core/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    GrcTextureManager::GrcTextureManager() {
        m_gfx_driver = IocContainer::instance()->resolve_v<GfxDriver>();
        m_gfx_ctx    = IocContainer::instance()->resolve_v<GfxCtx>();

        init_default_textures();
        init_default_samplers();
    }

    const Ref<GfxTexture>& GrcTextureManager::get_texture(GrcDefaultTexture texture) {
        assert(int(texture) < int(GrcDefaultTexture::Total));
        return m_default_textures[int(texture)];
    }

    const Ref<GfxSampler>& GrcTextureManager::get_sampler(GrcDefaultSampler sampler) {
        assert(int(sampler) < int(GrcDefaultSampler::Total));
        return m_default_samplers[int(sampler)];
    }

    void GrcTextureManager::init_default_textures() {
        const std::array<std::uint8_t, 4> tex_colors[int(GrcDefaultTexture::Total)] = {
                {0xff, 0xff, 0xff, 0xff},//
                {0x00, 0x00, 0x00, 0xff},//
                {0xff, 0x00, 0x00, 0xff},//
                {0x00, 0xff, 0x00, 0xff},//
                {0x00, 0x00, 0xff, 0xff},//
                {0x7f, 0x7f, 0x7f, 0xff},//
        };

        const char* tex_names[int(GrcDefaultTexture::Total)] = {
                "white",
                "black",
                "red",
                "green",
                "blue",
                "gray",
        };

        for (int i = 0; i < int(GrcDefaultTexture::Total); i++) {
            m_default_textures[i] = m_gfx_driver->make_texture_2d(1, 1, 1, GfxFormat::RGBA8, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID(tex_names[i]));
        }

        for (int i = 0; i < int(GrcDefaultTexture::Total); i++) {
            m_gfx_ctx->update_texture_2d(m_default_textures[i], 0, Rect2i(0, 0, 1, 1), make_ref<Data>(tex_colors[i].data(), sizeof(std::uint8_t[4])));
        }
    }

    void GrcTextureManager::init_default_samplers() {
        const GfxSamplerDesc samp_descs[int(GrcDefaultSampler::Total)] = {
                GfxSamplerDesc(),
                GfxSamplerDesc::make(GfxSampFlt::Linear, m_gfx_driver->device_caps().max_anisotropy, GfxSampAddress::Repeat),
                GfxSamplerDesc::make(GfxSampFlt::Nearest, 0.0f, GfxSampAddress::Repeat),
        };

        const char* samp_names[int(GrcDefaultSampler::Total)] = {
                "default",
                "linear",
                "nearest",
        };

        for (int i = 0; i < int(GrcDefaultSampler::Total); i++) {
            m_default_samplers[i] = m_gfx_driver->make_sampler(samp_descs[i], SID(samp_names[i]));
        }
    }

}// namespace wmoge