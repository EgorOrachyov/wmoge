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
#include "core/callback_queue.hpp"
#include "core/data.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/gfx_resource.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_texture.hpp"
#include "gfx/gfx_vert_format.hpp"
#include "math/mat.hpp"
#include "math/vec.hpp"
#include "platform/window.hpp"

namespace wmoge {

    /**
     * @class GfxCmdList
     * @brief List to record gfx commands for execution on device
    */
    class GfxCmdList : public GfxResource {
    public:
        ~GfxCmdList() override = default;

        virtual void update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data)                = 0;
        virtual void update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data)              = 0;
        virtual void update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data)          = 0;
        virtual void update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data)          = 0;
        virtual void update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data)                  = 0;
        virtual void update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) = 0;
        virtual void update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data)      = 0;

        virtual void* map_vert_buffer(const Ref<GfxVertBuffer>& buffer)       = 0;
        virtual void* map_index_buffer(const Ref<GfxIndexBuffer>& buffer)     = 0;
        virtual void* map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) = 0;
        virtual void* map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) = 0;

        virtual void unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer)       = 0;
        virtual void unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer)     = 0;
        virtual void unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) = 0;
        virtual void unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) = 0;

        virtual void barrier_image(const Ref<GfxTexture>& texture, GfxTexBarrierType barrier_type) = 0;
        virtual void barrier_buffer(const Ref<GfxStorageBuffer>& buffer)                           = 0;

        virtual void begin_render_pass(const GfxRenderPassBeginInfo& pass_desc)                                    = 0;
        virtual void peek_render_pass(GfxRenderPassRef& rp)                                                        = 0;
        virtual void viewport(const Rect2i& viewport)                                                              = 0;
        virtual void bind_pso(const Ref<GfxPsoGraphics>& pipeline)                                                 = 0;
        virtual void bind_pso(const Ref<GfxPsoCompute>& pipeline)                                                  = 0;
        virtual void bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset = 0)                 = 0;
        virtual void bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset = 0) = 0;
        virtual void bind_desc_set(const Ref<GfxDescSet>& set, int index)                                          = 0;
        virtual void bind_desc_sets(const array_view<GfxDescSet*>& sets, int offset = 0)                           = 0;
        virtual void draw(int vertex_count, int base_vertex, int instance_count)                                   = 0;
        virtual void draw_indexed(int index_count, int base_vertex, int instance_count)                            = 0;
        virtual void dispatch(Vec3i group_count)                                                                   = 0;
        virtual void end_render_pass()                                                                             = 0;

        virtual void begin_label(const Strid& label) = 0;
        virtual void end_label()                     = 0;

        virtual GfxQueueType get_queue_type() const = 0;
    };

    using GfxCmdListRef = Ref<GfxCmdList>;

}// namespace wmoge
