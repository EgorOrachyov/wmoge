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

#include "debug/profiler.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "system/engine.hpp"

namespace wmoge {

    TextureManager::TextureManager() {
        WG_AUTO_PROFILE_RENDER("TextureManager::TextureManager");

        Engine* engine = Engine::instance();
        m_gfx_driver   = engine->gfx_driver();
        m_gfx_ctx      = engine->gfx_ctx();

        unsigned char white[] = {0xff, 0xff, 0xff, 0xff};
        unsigned char black[] = {0x00, 0x00, 0x00, 0xff};
        unsigned char red[]   = {0xff, 0x00, 0x00, 0xff};

        m_gfx_default_sampler       = m_gfx_driver->make_sampler(GfxSamplerDesc{}, SID("default"));
        m_gfx_default_texture_white = m_gfx_driver->make_texture_2d(1, 1, 1, GfxFormat::RGBA8, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("default_1x1_white"));
        m_gfx_default_texture_black = m_gfx_driver->make_texture_2d(1, 1, 1, GfxFormat::RGBA8, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("default_1x1_black"));
        m_gfx_default_texture_red   = m_gfx_driver->make_texture_2d(1, 1, 1, GfxFormat::RGBA8, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, GfxTexSwizz::None, SID("default_1x1_red"));

        m_gfx_ctx->update_texture_2d(m_gfx_default_texture_white, 0, Rect2i(0, 0, 1, 1), make_ref<Data>(white, sizeof(white)));
        m_gfx_ctx->update_texture_2d(m_gfx_default_texture_black, 0, Rect2i(0, 0, 1, 1), make_ref<Data>(black, sizeof(black)));
        m_gfx_ctx->update_texture_2d(m_gfx_default_texture_red, 0, Rect2i(0, 0, 1, 1), make_ref<Data>(red, sizeof(red)));
    }

}// namespace wmoge
