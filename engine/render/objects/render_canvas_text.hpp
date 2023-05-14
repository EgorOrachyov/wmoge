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

#ifndef WMOGE_RENDER_CANVAS_TEXT_HPP
#define WMOGE_RENDER_CANVAS_TEXT_HPP

#include "render/objects/render_canvas_item.hpp"
#include "resource/font.hpp"

#include <string>

namespace wmoge {

    /**
     * @class RenderCanvasText
     * @brief Render-system implementation of 2d text label
     */
    class RenderCanvasText final : public RenderCanvasItem {
    public:
        static constexpr const int N_VERTICES_PER_GLYPH = 4;
        static constexpr const int N_INDICES_PER_GLYPH  = 6;

        ~RenderCanvasText() override = default;

        void initialize(std::string text, Ref<Font> font, float font_size, FontTextAlignment alignment);
        void set_text(std::string text);
        void set_font(Ref<Font> font);
        void set_font_size(float font_size);
        void set_alignment(FontTextAlignment alignment);

        void on_scene_enter() override;
        void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) override;
        bool need_update() const override;
        bool need_render_dynamic() const override;

    private:
        void rebuild_geom();
        void mark_dirty_geom();

    private:
        std::string       m_text;
        Ref<Font>         m_font;
        float             m_font_size  = 1.0f;
        FontTextAlignment m_alignment  = FontTextAlignment::Center;
        bool              m_dirty_geom = true;
        int               m_n_indices  = 0;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_CANVAS_TEXT_HPP
