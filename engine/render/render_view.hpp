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

#ifndef WMOGE_RENDER_VIEW_HPP
#define WMOGE_RENDER_VIEW_HPP

#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "math/color.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "memory/mem_linear.hpp"
#include "render/draw_cmd.hpp"
#include "render/draw_primitive.hpp"
#include "render/draw_queue.hpp"
#include "render/render_camera.hpp"

#include <bitset>
#include <memory>

namespace wmoge {

    /**
     * @class RenderViewMask
     * @brief Bit set using to filter primitive in some views (support only up to 32 views)
     */
    using RenderViewMask = std::bitset<32>;

    /**
     * @class RenderViewList
     * @brief List of views ptrs to render
     */
    using RenderViewList = fast_vector<class RenderView*, 32>;

    /**
     * @class RenderView
     * @brief All data needed to draw a single view to final screen
     */
    class RenderView final : public RefCnt {
    public:
        ~RenderView() override = default;

        void reset();

        void add_primitive(DrawPrimitive* primitive);
        void add_cmd(DrawCmdSortingKey key, DrawCmd* cmd, DrawPass pass);

        void set_display(const Ref<Window>& window);
        void set_viewport_rect(const Vec4f& rect);
        void set_screen_size(const Vec2f& get_screen_size);
        void set_screen_camera_mat_inv(const Mat3x3f& get_screen_camera_mat_inv);
        void set_screen_camera_mat_inv_prev(const Mat3x3f& get_screen_camera_mat_inv_prev);
        void set_clear_color(Color4f get_clear_color);
        void set_clear_depth(float get_clear_depth);
        void set_clear_stencil(int get_clear_stencil);
        void set_color_view(bool is_color_view);
        void set_overlay_view(bool is_overlay_view);
        void set_depth_view(bool is_depth_view);

        DrawCmdQueue&      get_draw_cmds(int pass_idx);
        DrawCmdQueue&      get_draw_cmds(DrawPass pass);
        const Ref<Window>& get_display() const { return m_display; }
        const Vec4f&       get_viewport_rect() const { return m_viewport_rect; }
        const Vec2f&       get_screen_size() const { return m_screen_size; }
        const Mat3x3f&     get_screen_camera_mat_inv() const { return m_screen_camera_mat_inv; }
        const Mat3x3f&     get_screen_camera_mat_inv_prev() const { return m_screen_camera_mat_inv_prev; }
        Color4f            get_clear_color() const { return m_clear_color; }
        float              get_clear_depth() const { return m_clear_depth; }
        int                get_clear_stencil() const { return m_clear_stencil; }
        bool               is_color_view() const { return m_is_color_view; }
        bool               is_overlay_view() const { return m_is_overlay_view; }
        bool               is_depth_view() const { return m_is_depth_view; }

    private:
        friend class PipelineStandard;

        DrawPrimitiveCollector m_primitive_collector;           /* Collection of dynamic primitives to be drawm */
        DrawCmdQueue           m_draw_cmds[int(DrawPass::Max)]; /* Queues with commands to draw per pass */

        /* General setup */
        Ref<Window> m_display;
        Vec4f       m_viewport_rect{0, 0, 1, 1};

        /* Setup for cameras which allows 2d rendering too in overlay */
        Vec2f   m_screen_size{};
        Mat3x3f m_screen_camera_mat_inv{};
        Mat3x3f m_screen_camera_mat_inv_prev{};

        /* Clear target of camera before a draw */
        Color4f m_clear_color   = Color::BLACK4f;
        float   m_clear_depth   = 1.0f;
        int     m_clear_stencil = 0;

        bool m_is_color_view   = false; /* Is camera for normal color rendering */
        bool m_is_overlay_view = false; /* Is camera for color rendering in overlay layer too */
        bool m_is_depth_view   = false; /* Is camera for normal depth (shadow) rendering */
    };

}// namespace wmoge

#endif//WMOGE_RENDER_VIEW_HPP
