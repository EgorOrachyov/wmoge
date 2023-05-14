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

#ifndef WMOGE_RENDER_PARTICLES_2D_HPP
#define WMOGE_RENDER_PARTICLES_2D_HPP

#include "pfx/pfx_emitter.hpp"
#include "render/objects/render_canvas_item.hpp"

namespace wmoge {

    /**
     * @class RenderParticles2d
     * @brief Render system object responsible for rendering of 2d particles with sprites
     */
    class RenderParticles2d : public RenderCanvasItem {
    public:
        ~RenderParticles2d() override = default;

        void initialize(const Ref<PfxEmitter>& emitter);

        void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) override;
        void on_scene_enter() override;
        bool need_update() const override;
        bool need_render_dynamic() const override;

        const Ref<GfxUniformBuffer>& get_draw_params_no_transform() const { return m_draw_params_no_transform; }

    private:
        Ref<PfxEmitter>       m_emitter;
        Ref<GfxUniformBuffer> m_draw_params_no_transform;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_PARTICLES_2D_HPP
