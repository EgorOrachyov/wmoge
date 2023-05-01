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

#ifndef WMOGE_RENDER_CANVAS_ITEM_HPP
#define WMOGE_RENDER_CANVAS_ITEM_HPP

#include "core/fast_vector.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/color.hpp"
#include "math/mat.hpp"
#include "math/math_utils2d.hpp"
#include "render/draw_cmd.hpp"
#include "render/render_object.hpp"
#include "resource/material.hpp"

namespace wmoge {

    /**
     * @class RenderItem2d
     * @brief Base class for any item which can be drawn into 2d overlay color pass
     */
    class RenderCanvasItem : public RenderObject {
    public:
        virtual ~RenderCanvasItem();

        void set_material(const ref_ptr<Material>& material);
        void set_layer_id(int layer_id);
        void set_transform(const Mat3x3f& new_model, const Mat3x3f& new_model_inv);
        void set_tint(const Color4f& tint);

        void on_render_dynamic(RenderViewList& views, const RenderViewMask& mask) override;

        const ref_ptr<Material>&                 get_material() { return m_material; }
        const ref_ptr<GfxUniformBuffer>&         get_draw_params() { return m_draw_params; }
        const ref_ptr<GfxIndexBuffer>&           get_index_buffer() { return m_index_buffer; }
        const ref_ptr<GfxVertBuffer>&            get_vert_buffer() { return m_vert_buffer; }
        const ref_ptr<GfxVertFormat>&            get_vert_format() { return m_vert_format; }
        const fast_vector<DrawCmd*, 1>&          get_cached_cmds() { return m_cached_cmds; }
        const fast_vector<DrawCmdSortingKey, 1>& get_cached_keys() { return m_cached_keys; }

        const Mat3x3f& get_model() { return m_model; }
        const Mat3x3f& get_model_prev() { return m_model_prev; }
        const Mat3x3f& get_model_inv() { return m_model_inv; }
        const Mat3x3f& get_model_inv_prev() { return m_model_inv_prev; }
        const Color4f& get_tint() { return m_tint; }
        int            get_layer_id() { return m_layer_id; }

    protected:
        void create_draw_params();
        void create_vert_format();
        void update_draw_params();
        void mark_dirty_params();
        void compile_draw_cmds(int index_count);

    protected:
        ref_ptr<Material>                 m_material;
        ref_ptr<GfxUniformBuffer>         m_draw_params;
        ref_ptr<GfxIndexBuffer>           m_index_buffer;
        ref_ptr<GfxVertBuffer>            m_vert_buffer;
        ref_ptr<GfxVertFormat>            m_vert_format;
        fast_vector<DrawCmd*, 1>          m_cached_cmds;
        fast_vector<DrawCmdSortingKey, 1> m_cached_keys;

        Mat3x3f m_model          = Math2d::identity3x3();
        Mat3x3f m_model_prev     = Math2d::identity3x3();
        Mat3x3f m_model_inv      = Math2d::identity3x3();
        Mat3x3f m_model_inv_prev = Math2d::identity3x3();
        Color4f m_tint           = Color::WHITE4f;
        int     m_layer_id       = 0;

        bool m_dirty_params = true;
    };

}// namespace wmoge

#endif//WMOGE_RENDER_CANVAS_ITEM_HPP
