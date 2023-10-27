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

#ifndef WMOGE_GFX_CTX_WRAPPER_HPP
#define WMOGE_GFX_CTX_WRAPPER_HPP

#include "core/callback_stream.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/threaded/gfx_ctx_threaded.hpp"

namespace wmoge {

    /**
     * @class GfxCtxWrapper
     * @brief Thread-safe wrapper for gfx context to be used from any thread
     *
     * Wraps GfxCtx interface. It uses commands serialization to send it
     * to a separate gfx thread, responsible for GPU communication. Simple
     * commands sent with no wait. Commands requiring immediate feedback
     * require to wait until gfx thread process them.
     *
     * @see GfxCtx
     * @see GfxCtxThreaded
     */
    class GfxCtxWrapper : public GfxCtx {
    public:
        explicit GfxCtxWrapper(GfxCtxThreaded* ctx);

        ~GfxCtxWrapper() override = default;

        void update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data) override;
        void update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data) override;
        void update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) override;
        void update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data) override;

        void* map_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void* map_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void* map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void* map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void begin_render_pass(const GfxRenderPassDesc& pass_desc, const StringId& name) override;
        void bind_target(const Ref<Window>& window) override;
        void bind_color_target(const Ref<GfxTexture>& texture, int target, int mip, int slice) override;
        void bind_depth_target(const Ref<GfxTexture>& texture, int mip, int slice) override;
        void viewport(const Rect2i& viewport) override;
        void clear(int target, const Vec4f& color) override;
        void clear(float depth, int stencil) override;
        bool bind_pipeline(const Ref<GfxPipeline>& pipeline) override;
        void bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset) override;
        void bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) override;
        void bind_desc_set(const Ref<GfxDescSet>& set, int index) override;
        void bind_desc_sets(const ArrayView<GfxDescSet*>& sets, int offset) override;
        void draw(int vertex_count, int base_vertex, int instance_count) override;
        void draw_indexed(int index_count, int base_vertex, int instance_count) override;
        void end_render_pass() override;

        void execute(const std::function<void(GfxCtx* thread_ctx)>& functor) override;
        void shutdown() override;

        void begin_frame() override;
        void end_frame() override;

        [[nodiscard]] const Mat4x4f& clip_matrix() const override;
        [[nodiscard]] GfxCtxType     ctx_type() const override;

    private:
        GfxCtxThreaded* m_ctx    = nullptr;
        CallbackStream* m_stream = nullptr;
        Mat4x4f         m_clip_matrix;
        GfxCtxType      m_ctx_type;
    };

}// namespace wmoge

#endif//WMOGE_GFX_CTX_WRAPPER_HPP
