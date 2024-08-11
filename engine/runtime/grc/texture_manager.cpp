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

#include "texture_manager.hpp"

#include "core/array_view.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <cassert>

namespace wmoge {

    TextureManager::TextureManager() {
        WG_AUTO_PROFILE_GRC("TextureManager::TextureManager");

        m_gfx_driver = IocContainer::iresolve_v<GfxDriver>();
        m_pool       = std::make_unique<TexturePool>(*m_gfx_driver);
        m_callback   = std::make_shared<std::function<void(Texture*)>>([this](Texture* texture) { remove(texture); });

        init_default_textures();
        init_default_samplers();
    }

    void TextureManager::update() {
        WG_AUTO_PROFILE_GRC("TextureManager::update");

        std::lock_guard guard(m_mutex);
        init_default_textures();
        init_textures();
    }

    Ref<Texture2d> TextureManager::create_2d(TextureFlags flags, GfxFormat format, int width, int height, GfxTexSwizz swizz) {
        flags.set(TextureFlag::Managed);

        auto texture = make_ref<Texture2d>(flags, GfxTextureDesc::make_2d(format, width, height, swizz));
        texture->set_sampler(get_sampler(DefaultSampler::Default));
        add(texture);
        return texture;
    }

    Ref<TextureCube> TextureManager::create_cube(TextureFlags flags, GfxFormat format, int width, int height) {
        flags.set(TextureFlag::Managed);

        auto texture = make_ref<TextureCube>(flags, GfxTextureDesc::make_cube(format, width, height));
        texture->set_sampler(get_sampler(DefaultSampler::Default));
        add(texture);
        return texture;
    }

    void TextureManager::add(const Ref<Texture>& texture) {
        assert(texture);
        assert(!has(texture.get()));

        std::lock_guard guard(m_mutex);
        texture->set_texture_callback(m_callback);
        m_textures[texture.get()].weak_ref = texture;
    }

    void TextureManager::remove(Texture* texture) {
        assert(texture);
        assert(has(texture));

        std::lock_guard guard(m_mutex);

        auto entry = m_textures.find(texture);
        if (entry->second.state.get(TextureState::Inited)) {
            texture->delete_gfx_resource(*m_pool);
        }
        m_textures.erase(entry);
    }

    void TextureManager::init(Texture* texture) {
        assert(texture);
        assert(has(texture));

        std::lock_guard guard(m_mutex);
        m_textures[texture].state.set(TextureState::PendingInit);
    }

    bool TextureManager::has(Texture* texture) const {
        std::lock_guard guard(m_mutex);
        return m_textures.find(texture) != m_textures.end();
    }

    const Ref<GfxTexture>& TextureManager::get_texture(DefaultTexture texture) {
        assert(int(texture) < int(DefaultTexture::Total));
        return m_default_textures[int(texture)];
    }

    const Ref<GfxSampler>& TextureManager::get_sampler(DefaultSampler sampler) {
        assert(int(sampler) < int(DefaultSampler::Total));
        return m_default_samplers[int(sampler)];
    }

    void TextureManager::init_default_samplers() {
        WG_AUTO_PROFILE_GRC("TextureManager::init_default_samplers");

        const GfxSamplerDesc samp_descs[int(DefaultSampler::Total)] = {
                GfxSamplerDesc(),
                GfxSamplerDesc::make(GfxSampFlt::Linear, m_gfx_driver->device_caps().max_anisotropy, GfxSampAddress::Repeat),
                GfxSamplerDesc::make(GfxSampFlt::Nearest, 0.0f, GfxSampAddress::Repeat),
        };

        const char* samp_names[int(DefaultSampler::Total)] = {
                "default",
                "linear",
                "nearest",
        };

        for (int i = 0; i < int(DefaultSampler::Total); i++) {
            m_default_samplers[i] = m_gfx_driver->make_sampler(samp_descs[i], SID(samp_names[i]));
        }
    }

    void TextureManager::init_default_textures() {
        WG_AUTO_PROFILE_GRC("TextureManager::init_default_textures");

        const char* tex_names[int(DefaultTexture::Total)] = {
                "white",
                "black",
                "red",
                "green",
                "blue",
                "gray",
        };

        for (int i = 0; i < int(DefaultTexture::Total); i++) {
            m_default_textures[i] = m_gfx_driver->make_texture_2d(1, 1, 1, GfxFormat::RGBA8, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID(tex_names[i]));
        }
    }

    void TextureManager::upload_default_textures() {
        WG_AUTO_PROFILE_GRC("TextureManager::upload_default_textures");

        if (!m_need_upload_default) {
            return;
        }

        const std::array<std::uint8_t, 4> tex_colors[int(DefaultTexture::Total)] = {
                {0xff, 0xff, 0xff, 0xff},
                {0x00, 0x00, 0x00, 0xff},
                {0xff, 0x00, 0x00, 0xff},
                {0x00, 0xff, 0x00, 0xff},
                {0x00, 0x00, 0xff, 0xff},
                {0x7f, 0x7f, 0x7f, 0xff},
        };

        auto cmd = m_gfx_driver->acquire_cmd_list(GfxQueueType::Graphics);

        for (int i = 0; i < int(DefaultTexture::Total); i++) {
            cmd->barrier_image(m_default_textures[i], GfxTexBarrierType::Undefined, GfxTexBarrierType::CopyDestination);
            cmd->update_texture_2d(m_default_textures[i], 0, Rect2i(0, 0, 1, 1), array_view<const std::uint8_t>(tex_colors[i].data(), sizeof(std::uint8_t[4])));
            cmd->barrier_image(m_default_textures[i], GfxTexBarrierType::CopyDestination, GfxTexBarrierType::Sampling);
        }

        m_gfx_driver->submit_cmd_list(cmd);
        m_need_upload_default = false;
    }

    void TextureManager::init_textures() {
        std::vector<Texture*> for_upload;
        for (auto& iter : m_textures) {
            auto  texture = iter.first;
            auto& entry   = iter.second;

            if (!entry.state.get(TextureState::PendingInit)) {
                continue;
            }
            if (!texture->create_gfx_resource(*m_pool)) {
                continue;
            }

            entry.state.set(TextureState::PendingInit, false);
            entry.state.set(TextureState::Inited, true);
            for_upload.push_back(texture);
        }

        if (for_upload.empty()) {
            return;
        }

        std::vector<GfxTexture*> for_barrier;
        for (Texture* texture : for_upload) {
            for_barrier.push_back(texture->get_texture().get());
        }

        auto cmd = m_gfx_driver->acquire_cmd_list(GfxQueueType::Graphics);
        cmd->barrier_images(for_barrier, GfxTexBarrierType::Undefined, GfxTexBarrierType::CopyDestination);
        for (Texture* texture : for_upload) {
            texture->upload_gfx_data(cmd);
        }
        cmd->barrier_images(for_barrier, GfxTexBarrierType::CopyDestination, GfxTexBarrierType::Sampling);
        m_gfx_driver->submit_cmd_list(cmd);

        WG_LOG_INFO("uploaded " << for_upload.size() << " textures to gpu");
    }

}// namespace wmoge