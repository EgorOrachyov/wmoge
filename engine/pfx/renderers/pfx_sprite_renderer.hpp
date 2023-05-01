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

#ifndef WMOGE_PFX_SPRITE_RENDERER_HPP
#define WMOGE_PFX_SPRITE_RENDERER_HPP

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "pfx/pfx_renderer.hpp"
#include "render/draw_cmd.hpp"
#include "resource/material.hpp"

namespace wmoge {

    /**
     * @class PfxSpriteRenderer
     * @brief Render particles animated sprites on a canvas
     */
    class PfxSpriteRenderer : public PfxRenderer {
    public:
        PfxSpriteRenderer(ref_ptr<Material> material, int vert_buffer_size, int index_buffer_size, const StringId& name);
        ~PfxSpriteRenderer() override = default;

        void on_update_data(const PfxComponentRenderData& data) override;
        void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask, RenderObject* object) override;
        bool need_render_dynamic() const override;

    private:
        PfxComponentRenderData  m_data{};
        ref_ptr<Material>       m_material;
        ref_ptr<GfxVertBuffer>  m_vert_buffer;
        ref_ptr<GfxIndexBuffer> m_index_buffer;
        StringId                m_name;
        DrawCmd                 m_draw_cmd;
        DrawCmdSortingKey       m_cmd_key;
        bool                    m_cmd_compiled = false;
    };

}// namespace wmoge

#endif//WMOGE_PFX_SPRITE_RENDERER_HPP
