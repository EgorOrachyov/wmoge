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

#pragma once

#include "core/array_view.hpp"
#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "core/mask.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_vector.hpp"
#include "math/color.hpp"
#include "math/mat.hpp"
#include "math/math_utils.hpp"
#include "math/math_utils2d.hpp"
#include "math/transform.hpp"
#include "math/vec.hpp"
#include "render/font.hpp"
#include "render/texture.hpp"

#include "shaders/generated/auto_canvas_reflection.hpp"

#include <cinttypes>
#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @brief Canvas drawing primitive flags
     */
    enum class CanvasFlag {
        None            = 0,//< Default
        Rounded         = 1,//< All sides rounded
        RoundedCornerLU = 2,//< Round left upper corner
        RoundedCornerRU = 3,//< Round right upper corner
        RoundedCornerLB = 4,//< Round left bottom corner
        RoundedCornerRB = 5,//< Round right bottom corner
    };

    /**
     * @brief Canvas flags set
     */
    using CanvasFlags = Mask<CanvasFlag>;

    /**
     * @class CanvasVert
     * @brief Canvas vert attributes for drawing on gpu
     */
    struct CanvasVert {
        Vec2f pos;
        Vec4f color;
        Vec2f uv;
    };

    /**
     * @class CanvasDrawCmd
     * @brief Canvas single draw cmd to dispatch gfx draw call
     */
    struct CanvasDrawCmd {
        Mat3x3f transform;
        Vec4f   clip_rect;
        int     texture_idx = -1;
        int     vtx_offset  = 0;
        int     idx_offset  = 0;
        int     elements    = 0;
    };

    /**
     * @class CanvasSharedData
     * @brief Shared data for canvas primitives rendering
    */
    struct CanvasSharedData {
        static constexpr int MAX_CANVAS_IMAGES = int(ShaderCanvas::MAX_CANVAS_IMAGES);

        CanvasSharedData();
        CanvasSharedData(const CanvasSharedData&) = delete;
        CanvasSharedData(CanvasSharedData&&)      = delete;
        ~CanvasSharedData()                       = default;

        void compile();
        void clear();

        std::vector<Ref<Texture2d>>      tex_buffer;     //< Texures referenced in cmds, store separately to pack batched into descriptor sets
        flat_map<Ref<Texture2d>, int>    tex_map;        //< Map used textures to ids (to batch used textures together)
        buffered_vector<Ref<GfxDescSet>> tex_sets;       //< Descriptor sets with textures to bind
        Ref<GfxPipeline>                 pipeline_srgb;  //< Cached pso to draw all primitives
        Ref<GfxPipeline>                 pipeline_linear;//< Cached pso to draw all primitives
    };

    /**
     * @brief Canvas struct to pack draw cmd data for gpu
     */
    using GPUCanvasDrawCmd = ShaderCanvas::DrawCmdData;

    /**
     * @class Canvas
     * @brief Low-level primitives for 2d drawing using lines, polygones, text and images
     */
    class Canvas {
    public:
        static constexpr int INLINE_STACK_SIZE   = 16;
        static constexpr int NUM_SEGMENTS_PER_PI = 32;
        static constexpr int MAX_CANVAS_IMAGES   = int(ShaderCanvas::MAX_CANVAS_IMAGES);

        Canvas();
        Canvas(std::shared_ptr<CanvasSharedData> shared);
        Canvas(const Canvas&) = delete;
        Canvas(Canvas&&)      = delete;
        ~Canvas()             = default;

        void           push_clip_rect(const Vec4f& clip_rect);
        void           pop_clip_rect();
        const Vec4f&   peek_clip_rect();
        void           push_transform(const Vec2f& translate, float rotate_rad, const Vec2f& scale);
        void           push_transform(const Vec2f& translate, float rotate_rad);
        void           push_transform(const Transform2d& transform);
        void           push_transform(const Mat3x3f& matr);
        void           pop_transform();
        const Mat3x3f& peek_transform();
        void           add_line(const Vec2f& p1, const Vec2f& p2, const Color4f& color, float thickness = 1.0f);
        void           add_rect(const Vec2f& p_min, const Vec2f& p_max, const Color4f& color, float rounding = 0.0f, CanvasFlags flags = {}, float thickness = 1.0f);
        void           add_rect_filled(const Vec2f& p_min, const Vec2f& p_max, const Color4f& color, float rounding = 0.0f, CanvasFlags flags = {});
        void           add_triangle(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3, const Color4f& color, float thickness = 1.0f);
        void           add_triangle_filled(const Vec2f& p1, const Vec2f& p2, const Vec2f& p3, const Color4f& color);
        void           add_polyline(const array_view<Vec2f>& points, const Color4f& color, float thickness = 1.0f);
        void           add_polygone(const array_view<Vec2f>& points, const Color4f& color);
        void           add_text(const std::string& text, const Ref<Font>& font, float font_size, const Vec2f& p, const Color4f& color);
        void           add_image(const Ref<Texture2d>& image, const Vec2f& p_min, const Vec2f& p_max, const Vec2f& uv_min, const Vec2f& uv_max, const Color4f& color);
        void           path_line_to(const Vec2f& p);
        void           path_arc_to(const Vec2f& p, float r, float start_rad, float end_rad, int num_segments = 0);
        void           path_stroke(const Color4f& color, float thickness);
        void           path_stroke_filled(const Color4f& color);
        void           path_clear();

        void clear(bool clear_shared_data = false);
        void compile(bool compile_shared_data = false);
        void render(const Ref<Window>& window, const Rect2i& viewport, const Vec4f& area, float gamma);

    private:
        void set_texture();
        void set_texture(const Ref<Texture2d>& tex);
        void commit_draw_cmd();
        void reserve(int num_vtx, int num_idx);
        void write_vtx(const Vec2f& pos, const Vec2f& uv, const Vec4f& color);
        void write_idx(int i1, int i2, int i3);
        int  tessellate(float angle, int num_segments);
        bool need_rounding(float rounding, CanvasFlags flags);

    private:
        std::vector<CanvasDrawCmd>               m_cmd_buffer;//< Cmds to draw, map to 1 draw call to Gfx driver
        GfxVector<CanvasVert, GfxVertBuffer>     m_vtx_buffer;//< Gpu vertex data for drawing
        GfxVector<std::uint32_t, GfxIndexBuffer> m_idx_buffer;//< Gpu index data for drawing
        GfxVector<int, GfxVertBuffer>            m_prx_buffer;//< Gpu per-instance cmd id buffer to fetch cmd data for drawing

        int m_vxt_current = 0;//< Current vertex to write
        int m_idx_current = 0;//< Current index to write

        std::vector<Vec2f>                            m_path;           //< Internal tmp buffer to draw path
        std::shared_ptr<CanvasSharedData>             m_shared;         //< Shared state amond all canvas classes for cmds rendering
        buffered_vector<Vec4f, INLINE_STACK_SIZE>     m_clip_rect_stack;//< Context stack
        buffered_vector<Mat3x3f, INLINE_STACK_SIZE>   m_transform_stack;//< Context stack
        GfxVector<GPUCanvasDrawCmd, GfxStorageBuffer> m_gpu_cmd_buffer; //< Packed cmd data for gpu
        Ref<GfxUniformBuffer>                         m_params;         //< Cached ubo to fill with const params
        Ref<GfxDescSet>                               m_params_set;     //< Cached ubo to fill with const params and draw cmds buffer
    };

}// namespace wmoge