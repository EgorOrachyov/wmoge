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

    /** @brief Managed texture state */
    enum class TextureState {
        PendingInit = 0,
        PendingUpload,
        Inited
    };

    /**
     * @class TextureManager
     * @brief Manager for memory, gfx and streaming of texture assets
    */
    class TextureManager {
    public:
        TextureManager(class IocContainer* ioc);

        Ref<Texture2d>   create_2d(TextureFlags flags, GfxFormat format, int width, int height, GfxTexSwizz swizz = GfxTexSwizz::None);
        Ref<TextureCube> create_cube(TextureFlags flags, GfxFormat format, int width, int height);
        void             add(const Ref<Texture>& texture);
        void             remove(Texture* texture);
        void             init(Texture* texture);
        bool             has(Texture* texture) const;
        void             update();

        [[nodiscard]] const Ref<GfxTexture>& get_texture(DefaultTexture texture);
        [[nodiscard]] const Ref<GfxSampler>& get_sampler(DefaultSampler sampler);

    private:
        void init_default_samplers();
        void init_default_textures();
        void upload_default_textures();
        void init_textures();

    private:
        struct Entry {
            WeakRef<Texture>   weak_ref;
            Mask<TextureState> state;
        };

        flat_map<Texture*, Entry>    m_textures;
        Texture::CallbackRef         m_callback;
        std::unique_ptr<TexturePool> m_pool;
        bool                         m_need_upload_default = true;

        Ref<GfxTexture> m_default_textures[int(DefaultTexture::Total)];
        Ref<GfxSampler> m_default_samplers[int(DefaultSampler::Total)];

        GfxDriver* m_gfx_driver;

        mutable std::mutex m_mutex;
    };

}// namespace wmoge