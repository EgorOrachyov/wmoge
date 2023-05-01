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

#ifndef WMOGE_RENDER_CAMERA_2D_HPP
#define WMOGE_RENDER_CAMERA_2D_HPP

#include "render/objects/render_canvas_item.hpp"

namespace wmoge {

    /**
     * @class RenderCamera2d
     * @brief Render-system camera 2d for overlay rendering of canvas items
     */
    class RenderCamera2d : public RenderCanvasItem {
    public:
        void set_background(const Color4f& background) { m_background = background; }
        void set_screen_space(const Vec2f& screen_space) { m_screen_space = screen_space; }
        void set_viewport_rect(const Vec4f& viewport_rect) { m_viewport_rect = viewport_rect; }
        void set_window(const ref_ptr<Window>& window) { m_window = window; }

        void on_view_setup(RenderView* view) override;
        bool need_update() const override;
        bool need_render_dynamic() const override;
        bool need_view() const override;

    private:
        Color4f         m_background    = Color::BLACK4f;
        Vec2f           m_screen_space  = Vec2f(1280, 720);
        Vec4f           m_viewport_rect = Vec4f(0, 0, 1, 1);
        ref_ptr<Window> m_window;
        bool            m_is_active = true;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_CAMERA_2D_HPP
