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

#pragma once

#include "core/flat_set.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/texture.hpp"
#include "grc/texture_pool.hpp"

#include <mutex>
#include <vector>

namespace wmoge {

    /** @brief Built-in default textures */
    enum class DefaultTexture {
        White = 0,
        Black,
        Red,
        Green,
        Blue,
        Gray,
        Total
    };

    /** @brief Built-in default samplers */
    enum class DefaultSampler {
        Default = 0,
        Linear,
        Nearest,
        Total
    };

    /**
     * @class TextureManager
     * @brief Manager for memory, gfx and streaming of texture assets
    */
    class TextureManager {
    public:
        TextureManager(class IocContainer* ioc);

        Ref<Texture2d>   create_texture_2d(TextureDesc& desc);
        Ref<Texture2d>   create_texture_2d(TextureFlags flags, GfxFormat format, int width, int height, int mips, GfxTexSwizz swizz = GfxTexSwizz::None);
        Ref<TextureCube> create_texture_cube(TextureDesc& desc);
        void             add_texture(const Ref<Texture>& texture);
        void             remove_texture(Texture* texture);
        void             queue_texture_upload(Texture* texture);
        bool             has_texture(Texture* texture) const;
        void             flust_textures_upload();
        Status           generate_mips(const std::vector<Ref<Image>>& images, std::vector<Ref<Image>>& mips);
        Status           generate_compressed_data(const std::vector<Ref<Image>>& images, GfxFormat format, const TexCompressionParams& params, std::vector<GfxImageData>& compressed, GfxFormat& format_compressed, TexCompressionStats& stats);

        [[nodiscard]] const Ref<Texture>&    get_texture(DefaultTexture texture);
        [[nodiscard]] const Ref<GfxTexture>& get_texture_gfx(DefaultTexture texture);
        [[nodiscard]] const Ref<GfxSampler>& get_sampler(DefaultSampler sampler);

    private:
        void init_default_samplers();
        void init_default_textures();
        void init_texture(Texture* texture);
        void delete_texture(Texture* texture);
        void upload_texture(Texture* texture, const GfxCmdListRef& cmd);

    private:
        enum class State {
            PendingUpload = 0,
        };

        struct Entry {
            WeakRef<Texture> weak_ref;
            Mask<State>      state;
        };

        flat_map<Texture*, Entry>    m_textures;
        Texture::CallbackRef         m_callback;
        std::unique_ptr<TexturePool> m_pool;
        bool                         m_need_upload_default = true;
        Ref<Texture>                 m_default_textures[int(DefaultTexture::Total)];
        Ref<GfxSampler>              m_default_samplers[int(DefaultSampler::Total)];

        GfxDriver* m_gfx_driver;

        mutable std::mutex m_mutex;
    };

}// namespace wmoge