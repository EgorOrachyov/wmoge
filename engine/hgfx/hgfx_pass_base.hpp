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

#ifndef WMOGE_HGFX_PASS_BASE_HPP
#define WMOGE_HGFX_PASS_BASE_HPP

#include "hgfx/hgfx_pass.hpp"

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "math/color.hpp"
#include "math/math_utils3d.hpp"

namespace wmoge {

    /**
     * @class HgfxPassBase
     * @brief Render simple 3d geometry with optional vertex color without texturing and lighting
     */
    class HgfxPassBase : public HgfxPass {
    public:
        WG_OBJECT(HgfxPassBase, HgfxPass);

        Strid            name;
        GfxVertAttribs   attribs_req    = {GfxVertAttrib::Pos3f};
        GfxVertAttribs   attribs_full   = {GfxVertAttrib::Pos3f};
        GfxPrimType      prim_type      = GfxPrimType::Triangles;
        GfxPolyMode      poly_mode      = GfxPolyMode::Fill;
        GfxPolyCullMode  cull_mode      = GfxPolyCullMode::Disabled;
        GfxPolyFrontFace front_face     = GfxPolyFrontFace::CounterClockwise;
        int              depth_enable   = true;
        int              depth_write    = true;
        GfxCompFunc      depth_func     = GfxCompFunc::Less;
        bool             out_srgb       = false;
        bool             no_alpha       = true;
        Mat4x4f          mat_proj_view  = Math3d::identity();
        Vec4f            base_color     = Color::WHITE4f;
        float            gamma          = 2.2f;
        float            mix_weights[3] = {0, 0, 0};

        bool compile(GfxCtx* gfx_ctx) override;
        bool configure(GfxCtx* gfx_ctx) override;

        Strid        get_pass_name() override;
        HgfxPassType get_pass_type() override;

    private:
        Ref<GfxPipeline>      m_pipeline;
        Ref<GfxDescSet>       m_desc_set;
        GfxUniformBufferSetup m_buffer_setup;
    };

}// namespace wmoge

#endif//WMOGE_HGFX_PASS_BASE_HPP
