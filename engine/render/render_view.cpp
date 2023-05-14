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

#include "render_view.hpp"

#include <cassert>

namespace wmoge {

    void RenderView::reset() {
        for (auto& queue : m_draw_cmds) {
            queue.clear();
        }
        m_primitive_collector.clear();
    }

    void RenderView::add_primitive(DrawPrimitive* primitive) {
        assert(primitive);
        m_primitive_collector.push(primitive);
    }
    void RenderView::add_cmd(DrawCmdSortingKey key, DrawCmd* cmd, DrawPass pass) {
        assert(cmd);
        get_draw_cmds(pass).push(key, cmd);
    }

    void RenderView::set_display(const Ref<Window>& window) {
        m_display = window;
    }
    void RenderView::set_viewport_rect(const Vec4f& rect) {
        m_viewport_rect = rect;
    }
    void RenderView::set_screen_size(const Vec2f& screen_size) {
        m_screen_size = screen_size;
    }
    void RenderView::set_screen_camera_mat_inv(const Mat3x3f& screen_camera_mat_inv) {
        m_screen_camera_mat_inv = screen_camera_mat_inv;
    }
    void RenderView::set_screen_camera_mat_inv_prev(const Mat3x3f& screen_camera_mat_inv_prev) {
        m_screen_camera_mat_inv_prev;
    }
    void RenderView::set_clear_color(Color4f clear_color) {
        m_clear_color = clear_color;
    }
    void RenderView::set_clear_depth(float clear_depth) {
        m_clear_depth = clear_depth;
    }
    void RenderView::set_clear_stencil(int clear_stencil) {
        m_clear_stencil = clear_stencil;
    }
    void RenderView::set_color_view(bool is_color_view) {
        m_is_color_view = is_color_view;
    }
    void RenderView::set_overlay_view(bool is_overlay_view) {
        m_is_overlay_view = is_overlay_view;
    }
    void RenderView::set_depth_view(bool is_depth_view) {
        m_is_depth_view = is_depth_view;
    }

    DrawCmdQueue& RenderView::get_draw_cmds(int pass_idx) {
        assert(pass_idx < int(DrawPass::Max));
        return m_draw_cmds[pass_idx];
    }
    DrawCmdQueue& RenderView::get_draw_cmds(DrawPass pass) {
        return get_draw_cmds(int(pass));
    }

}// namespace wmoge
