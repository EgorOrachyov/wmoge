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
#include "core/ioc_container.hpp"
#include "grc/texture_compression.hpp"
#include "profiler/profiler_cpu.hpp"
#include "profiler/profiler_gpu.hpp"

#include <cassert>

namespace wmoge {

    TextureManager::TextureManager(IocContainer* ioc) {
        WG_PROFILE_CPU_GRC("TextureManager::TextureManager");

        m_gfx_driver = ioc->resolve_value<GfxDriver>();
        m_pool       = std::make_unique<TexturePool>(*m_gfx_driver);
        m_callback   = std::make_shared<Texture::Callback>([this](Texture* texture) { remove_texture(texture); });

        init_default_textures();
        init_default_samplers();
    }

    Ref<Texture2d> TextureManager::create_texture_2d(TextureDesc& desc) {
        WG_PROFILE_CPU_GRC("TextureManager::create_texture_2d");
        std::lock_guard guard(m_mutex);

        assert(desc.tex_type == GfxTex::Tex2d);
        assert(desc.array_slices == 1);

        desc.flags |= {TextureFlag::Managed};
        auto texture = make_ref<Texture2d>(desc);
        init_texture(texture.get());
        return texture;
    }

    Ref<Texture2d> TextureManager::create_texture_2d(TextureFlags flags, GfxFormat format, int width, int height, int mips, GfxTexSwizz swizz) {
        WG_PROFILE_CPU_GRC("TextureManager::create_texture_2d");

        TextureDesc desc;
        desc.tex_type      = GfxTex::Tex2d;
        desc.flags         = flags;
        desc.width         = width;
        desc.height        = height;
        desc.mips          = mips;
        desc.swizz         = swizz;
        desc.format        = format;
        desc.format_source = format;
        return create_texture_2d(desc);
    }

    Ref<TextureCube> TextureManager::create_texture_cube(TextureDesc& desc) {
        WG_PROFILE_CPU_GRC("TextureManager::create_texture_cube");
        std::lock_guard guard(m_mutex);

        assert(desc.tex_type == GfxTex::TexCube);
        assert(desc.array_slices == 6);

        desc.flags |= {TextureFlag::Managed};
        auto texture = make_ref<TextureCube>(desc);
        init_texture(texture.get());
        return texture;
    }

    void TextureManager::add_texture(const Ref<Texture>& texture) {
        assert(texture);
        assert(!has_texture(texture.get()));

        std::lock_guard guard(m_mutex);

        texture->set_texture_callback(m_callback);
        m_textures[texture.get()].weak_ref = texture;
    }

    void TextureManager::remove_texture(Texture* texture) {
        assert(texture);
        assert(has_texture(texture));

        std::lock_guard guard(m_mutex);

        auto entry = m_textures.find(texture);
        delete_texture(texture);
        m_textures.erase(entry);
    }

    void TextureManager::queue_texture_upload(Texture* texture) {
        assert(texture);
        assert(has_texture(texture));

        std::lock_guard guard(m_mutex);
        m_textures[texture].state.set(State::PendingUpload);
    }

    bool TextureManager::has_texture(Texture* texture) const {
        assert(texture);

        std::lock_guard guard(m_mutex);
        return m_textures.find(texture) != m_textures.end();
    }

    void TextureManager::flust_textures_upload() {
        WG_PROFILE_CPU_GRC("TextureManager::flust_textures_upload");

        std::lock_guard guard(m_mutex);

        std::vector<Texture*> for_upload;
        for (auto& iter : m_textures) {
            auto& entry = iter.second;

            if (entry.state.get(State::PendingUpload)) {
                entry.state.set(State::PendingUpload, false);
                for_upload.push_back(iter.first);
            }
        }

        if (for_upload.empty()) {
            return;
        }

        std::vector<GfxTexture*> for_barrier;
        for (Texture* texture : for_upload) {
            for_barrier.push_back(texture->get_texture().get());
        }

        auto cmd = m_gfx_driver->acquire_cmd_list(GfxQueueType::Graphics);
        WG_PROFILE_GPU_BEGIN(cmd);
        {
            WG_PROFILE_GPU_SCOPE(cmd, "TextureManager::flust_textures_upload");

            cmd->barrier_images(for_barrier, GfxTexBarrierType::Undefined, GfxTexBarrierType::CopyDestination);
            for (Texture* texture : for_upload) {
                upload_texture(texture, cmd);
            }
            cmd->barrier_images(for_barrier, GfxTexBarrierType::CopyDestination, GfxTexBarrierType::Sampling);
        }
        WG_PROFILE_GPU_END(cmd);
        m_gfx_driver->submit_cmd_list(cmd);

        WG_LOG_INFO("uploaded " << for_upload.size() << " textures to gpu");
    }

    Status TextureManager::generate_mips(const std::vector<Ref<Image>>& images, std::vector<Ref<Image>>& mips) {
        WG_PROFILE_CPU_ASSET("TextureManager::generate_mips");

        for (auto& image : images) {
            std::vector<Ref<Image>> face_mips;
            WG_CHECKED(image->generate_mip_chain(face_mips));

            for (auto& mip : face_mips) {
                mips.push_back(mip);
            }
        }

        return WG_OK;
    }

    Status TextureManager::generate_compressed_data(const std::vector<Ref<Image>>& images, GfxFormat format, const TexCompressionParams& params, std::vector<GfxImageData>& compressed, GfxFormat& format_compressed, TexCompressionStats& stats) {
        WG_PROFILE_CPU_ASSET("TextureManager::generate_compressed_data");

        if (images.empty()) {
            return WG_OK;
        }

        std::vector<GfxImageData> source_data;
        source_data.reserve(images.size());

        for (auto& image : images) {
            GfxImageData data;
            data.format = format;
            data.depth  = 1;
            data.width  = image->get_width();
            data.height = image->get_height();
            data.data   = image->get_pixel_data();
            source_data.push_back(data);
        }

        WG_CHECKED(TexCompression::compress(params, source_data, compressed, stats));
        assert(source_data.size() == compressed.size());

        format_compressed = compressed.front().format;

        return WG_OK;
    }

    const Ref<Texture>& TextureManager::get_texture(DefaultTexture texture) {
        assert(int(texture) < int(DefaultTexture::Total));
        return m_default_textures[int(texture)];
    }

    const Ref<GfxTexture>& TextureManager::get_texture_gfx(DefaultTexture texture) {
        return get_texture(texture)->get_texture();
    }

    const Ref<GfxSampler>& TextureManager::get_sampler(DefaultSampler sampler) {
        assert(int(sampler) < int(DefaultSampler::Total));
        return m_default_samplers[int(sampler)];
    }

    void TextureManager::init_default_samplers() {
        WG_PROFILE_CPU_GRC("TextureManager::init_default_samplers");

        const char* samp_names[int(DefaultSampler::Total)] = {"default", "linear", "nearest"};

        const GfxSamplerDesc samp_descs[int(DefaultSampler::Total)] = {
                GfxSamplerDesc(),
                GfxSamplerDesc::make(GfxSampFlt::Linear, m_gfx_driver->device_caps().max_anisotropy, GfxSampAddress::Repeat),
                GfxSamplerDesc::make(GfxSampFlt::Nearest, 0.0f, GfxSampAddress::Repeat),
        };

        for (int i = 0; i < int(DefaultSampler::Total); i++) {
            m_default_samplers[i] = m_gfx_driver->make_sampler(samp_descs[i], SID(samp_names[i]));
        }
    }

    void TextureManager::init_default_textures() {
        WG_PROFILE_CPU_GRC("TextureManager::init_default_textures");

        const char* tex_names[int(DefaultTexture::Total)] = {"white", "black", "red", "green", "blue", "gray"};

        const std::array<std::uint8_t, 4> tex_colors[int(DefaultTexture::Total)] = {
                {0xff, 0xff, 0xff, 0xff},
                {0x00, 0x00, 0x00, 0xff},
                {0xff, 0x00, 0x00, 0xff},
                {0x00, 0xff, 0x00, 0xff},
                {0x00, 0x00, 0xff, 0xff},
                {0x7f, 0x7f, 0x7f, 0xff},
        };

        for (int i = 0; i < int(DefaultTexture::Total); i++) {
            std::vector<Ref<Image>> source_images;
            Ref<Image>&             image = source_images.emplace_back(make_ref<Image>());

            image->create(1, 1, 4, 4);
            std::memcpy(image->get_pixel_data()->buffer(), tex_colors[i].data(), sizeof(std::uint8_t[4]));

            m_default_textures[i] = create_texture_2d({}, GfxFormat::RGBA8, 1, 1, 1, GfxTexSwizz::None);
            m_default_textures[i]->set_id(SID(tex_names[i]));
            m_default_textures[i]->set_source_images(std::move(source_images), GfxFormat::RGBA8);
            queue_texture_upload(m_default_textures[i].get());
        }
    }

    void TextureManager::init_texture(Texture* texture) {
        assert(texture);

        auto& entry    = m_textures[texture];
        entry.weak_ref = WeakRef<Texture>(texture);

        const bool           is_pooled = texture->get_flags().get(TextureFlag::Pooled);
        const GfxTextureDesc desc      = texture->get_desc();

        if (is_pooled) {
            texture->set_texture(m_pool->allocate(desc, SIDDBG(texture->get_name().str())));
        } else {
            texture->set_texture(m_gfx_driver->make_texture(desc, SIDDBG(texture->get_name().str())));
        }

        texture->set_texture_callback(m_callback);
    }

    void TextureManager::delete_texture(Texture* texture) {
        assert(texture);

        const bool is_pooled = texture->get_flags().get(TextureFlag::Pooled);

        if (is_pooled) {
            m_pool->release(texture->get_texture());
        }

        texture->set_texture(nullptr);
    }

    void TextureManager::upload_texture(Texture* texture, const GfxCmdListRef& cmd) {
        WG_PROFILE_CPU_GRC("TextureManager::upload_texture");
        WG_PROFILE_GPU_SCOPE(cmd, "TextureManager::upload_texture");

        assert(texture);

        const auto flags         = texture->get_flags();
        const auto format        = texture->get_format();
        const bool is_compressed = flags.get(TextureFlag::Compressed);
        auto&      images        = texture->get_images();
        auto&      compressed    = texture->get_compressed();

        assert(texture->get_depth() == 1);
        assert(texture->get_array_slices() >= 1);
        assert(texture->get_mips() >= 1);

        for (int array_slice = 0; array_slice < texture->get_array_slices(); array_slice++) {
            WG_PROFILE_GPU_SCOPE(cmd.get(), "upload_slice");

            for (int mip = 0; mip < texture->get_mips(); mip++) {
                WG_PROFILE_GPU_SCOPE(cmd.get(), "upload_mip");

                const int index = array_slice * texture->get_mips() + mip;

                Ref<Data> data = images[index]->get_pixel_data();
                Rect2i    rect = {0, 0, images[index]->get_width(), images[index]->get_height()};

                if (is_compressed) {
                    data = compressed[index].data;
                    rect = {0, 0, compressed[index].width, compressed[index].height};
                }

                array_view<const std::uint8_t> buffer(data->buffer(), data->size());

                switch (texture->get_tex_type()) {
                    case GfxTex::Tex2d:
                        cmd->update_texture_2d(texture->get_texture(), mip, rect, buffer);
                        continue;
                    case GfxTex::Tex2dArray:
                        cmd->update_texture_2d_array(texture->get_texture(), mip, array_slice, rect, buffer);
                        continue;
                    case GfxTex::TexCube:
                        cmd->update_texture_cube(texture->get_texture(), mip, array_slice, rect, buffer);
                        continue;
                    default:
                        assert(false);
                }
            }
        }
    }

}// namespace wmoge