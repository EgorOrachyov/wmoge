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

#ifndef WMOGE_HGFX_PASS_TEXT_HPP
#define WMOGE_HGFX_PASS_TEXT_HPP

#include "hgfx/hgfx_pass.hpp"

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_texture.hpp"
#include "math/color.hpp"
#include "math/math_utils3d.hpp"

namespace wmoge {

    /**
     * @class HgfxPassText
     * @brief Render simple 2d text on a screen with blending support
     */
    class HgfxPassText : public HgfxPass {
    public:
        WG_OBJECT(HgfxPassText, HgfxPass);

        StringId        name;
        Vec2f           screen_size = Vec2f(1280, 720);
        float           gamma       = 2.2f;
        bool            out_srgb    = false;
        Ref<GfxTexture> font_texture;
        Ref<GfxSampler> font_sampler;

        bool compile(GfxCtx* gfx_ctx) override;
        bool configure(GfxCtx* gfx_ctx) override;

        StringId     get_pass_name() override;
        HgfxPassType get_pass_type() override;

    private:
        Ref<GfxPipeline>      m_pipeline;
        GfxUniformBufferSetup m_buffer_setup;
    };

}// namespace wmoge

#endif//WMOGE_HGFX_PASS_TEXT_HPP
