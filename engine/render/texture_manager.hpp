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

#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"

namespace wmoge {

    /**
     * @class TextureManager
     * @brief Manages allocation and streaming of all engine textures
     */
    class TextureManager final {
    public:
        TextureManager();

        [[nodiscard]] const Ref<GfxTexture>& get_gfx_default_texture_white() const { return m_gfx_default_texture_white; }
        [[nodiscard]] const Ref<GfxTexture>& get_gfx_default_texture_black() const { return m_gfx_default_texture_black; }
        [[nodiscard]] const Ref<GfxTexture>& get_gfx_default_texture_red() const { return m_gfx_default_texture_red; }
        [[nodiscard]] const Ref<GfxSampler>& get_gfx_default_sampler() const { return m_gfx_default_sampler; }

    private:
        Ref<GfxTexture> m_gfx_default_texture_white;
        Ref<GfxTexture> m_gfx_default_texture_black;
        Ref<GfxTexture> m_gfx_default_texture_red;
        Ref<GfxSampler> m_gfx_default_sampler;

        class GfxDriver* m_gfx_driver;
        class GfxCtx*    m_gfx_ctx;
    };

}// namespace wmoge