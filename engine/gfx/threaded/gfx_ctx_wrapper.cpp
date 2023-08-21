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

#include "gfx_ctx_wrapper.hpp"

#include "debug/profiler.hpp"

#include <cassert>

namespace wmoge {

    GfxCtxWrapper::GfxCtxWrapper(GfxCtxThreaded* ctx) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::GfxCtxWrapper");

        assert(ctx);

        m_ctx         = ctx;
        m_stream      = ctx->cmd_stream();
        m_clip_matrix = ctx->clip_matrix();
        m_ctx_type    = ctx->ctx_type();
    }

    void GfxCtxWrapper::update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_vert_buffer");

        m_stream->push([=]() { m_ctx->update_vert_buffer(buffer, offset, range, data); });
    }
    void GfxCtxWrapper::update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_index_buffer");

        m_stream->push([=]() { m_ctx->update_index_buffer(buffer, offset, range, data); });
    }
    void GfxCtxWrapper::update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_uniform_buffer");

        m_stream->push([=]() { m_ctx->update_uniform_buffer(buffer, offset, range, data); });
    }
    void GfxCtxWrapper::update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_storage_buffer");

        m_stream->push([=]() { m_ctx->update_storage_buffer(buffer, offset, range, data); });
    }
    void GfxCtxWrapper::update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_texture_2d");

        m_stream->push([=]() { m_ctx->update_texture_2d(texture, mip, region, data); });
    }
    void GfxCtxWrapper::update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_texture_2d_array");

        m_stream->push([=]() { m_ctx->update_texture_2d_array(texture, mip, slice, region, data); });
    }
    void GfxCtxWrapper::update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::update_texture_cube");

        m_stream->push([=]() { m_ctx->update_texture_cube(texture, mip, face, region, data); });
    }

    void* GfxCtxWrapper::map_vert_buffer(const Ref<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::map_vert_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_ctx->map_vert_buffer(buffer); });
        return mapped_ptr;
    }
    void* GfxCtxWrapper::map_index_buffer(const Ref<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::map_index_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_ctx->map_index_buffer(buffer); });
        return mapped_ptr;
    }
    void* GfxCtxWrapper::map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::map_uniform_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_ctx->map_uniform_buffer(buffer); });
        return mapped_ptr;
    }
    void* GfxCtxWrapper::map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::map_storage_buffer");

        void* mapped_ptr = nullptr;
        m_stream->push_and_wait([&]() { mapped_ptr = m_ctx->map_storage_buffer(buffer); });
        return mapped_ptr;
    }
    void GfxCtxWrapper::unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::unmap_vert_buffer");

        m_stream->push([=]() { m_ctx->unmap_vert_buffer(buffer); });
    }
    void GfxCtxWrapper::unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::unmap_index_buffer");

        m_stream->push([=]() { m_ctx->unmap_index_buffer(buffer); });
    }
    void GfxCtxWrapper::unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::unmap_uniform_buffer");

        m_stream->push([=]() { m_ctx->unmap_uniform_buffer(buffer); });
    }
    void GfxCtxWrapper::unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::unmap_storage_buffer");

        m_stream->push([=]() { m_ctx->unmap_storage_buffer(buffer); });
    }

    void GfxCtxWrapper::begin_render_pass(const GfxRenderPassDesc& pass_desc, const StringId& name) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::begin_render_pass");

        m_stream->push([=]() { m_ctx->begin_render_pass(pass_desc, name); });
    }
    void GfxCtxWrapper::bind_target(const Ref<Window>& window) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_target");

        m_stream->push([=]() { m_ctx->bind_target(window); });
    }
    void GfxCtxWrapper::bind_color_target(const Ref<GfxTexture>& texture, int target, int mip, int slice) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_color_target");

        m_stream->push([=]() { m_ctx->bind_color_target(texture, target, mip, slice); });
    }
    void GfxCtxWrapper::bind_depth_target(const Ref<GfxTexture>& texture, int mip, int slice) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_depth_target");

        m_stream->push([=]() { m_ctx->bind_depth_target(texture, mip, slice); });
    }
    void GfxCtxWrapper::viewport(const Rect2i& viewport) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::viewport");

        m_stream->push([=]() { m_ctx->viewport(viewport); });
    }
    void GfxCtxWrapper::clear(int target, const Vec4f& color) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::clear");

        m_stream->push([=]() { m_ctx->clear(target, color); });
    }
    void GfxCtxWrapper::clear(float depth, int stencil) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::clear");

        m_stream->push([=]() { m_ctx->clear(depth, stencil); });
    }
    bool GfxCtxWrapper::bind_pipeline(const Ref<GfxPipeline>& pipeline) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_pipeline");

        bool is_bound = false;
        m_stream->push_and_wait([&]() { is_bound = m_ctx->bind_pipeline(pipeline); });
        return is_bound;
    }
    void GfxCtxWrapper::bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_vert_buffer");

        m_stream->push([=]() { m_ctx->bind_vert_buffer(buffer, index, offset); });
    }
    void GfxCtxWrapper::bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_index_buffer");

        m_stream->push([=]() { m_ctx->bind_index_buffer(buffer, index_type, offset); });
    }
    void GfxCtxWrapper::bind_desc_set(const Ref<GfxDescSet>& set, int index) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_desc_set");

        m_stream->push([=]() { m_ctx->bind_desc_set(set, index); });
    }
    void GfxCtxWrapper::bind_desc_sets(const ArrayView<GfxDescSet*>& sets, int offset) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::bind_desc_sets");

        m_stream->push([=]() { m_ctx->bind_desc_sets(sets, offset); });
    }
    void GfxCtxWrapper::draw(int vertex_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::draw");

        m_stream->push([=]() { m_ctx->draw(vertex_count, base_vertex, instance_count); });
    }
    void GfxCtxWrapper::draw_indexed(int index_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::draw_indexed");

        m_stream->push([=]() { m_ctx->draw_indexed(index_count, base_vertex, instance_count); });
    }
    void GfxCtxWrapper::end_render_pass() {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::end_render_pass");

        m_stream->push([=]() { m_ctx->end_render_pass(); });
    }

    void GfxCtxWrapper::execute(const std::function<void()>& functor) {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::execute");

        m_stream->push_and_wait([&]() { m_ctx->execute(functor); });
    }
    void GfxCtxWrapper::shutdown() {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::shutdown");

        m_stream->push_and_wait([=]() { m_ctx->shutdown(); });
    }

    void GfxCtxWrapper::begin_frame() {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::begin_frame");

        m_stream->push([=]() { m_ctx->begin_frame(); });
    }
    void GfxCtxWrapper::end_frame() {
        WG_AUTO_PROFILE_GFX("GfxCtxWrapper::end_frame");

        m_stream->push([=]() { m_ctx->end_frame(); });
    }

    const Mat4x4f& GfxCtxWrapper::clip_matrix() const {
        return m_clip_matrix;
    }
    GfxCtxType GfxCtxWrapper::ctx_type() const {
        return m_ctx_type;
    }

}// namespace wmoge
