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

#include "vk_ctx.hpp"

#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    VKCtx::VKCtx(class VKDriver& driver) : m_driver(driver) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::VKCtx");

        m_cmd_manager        = std::make_unique<VKCmdManager>(driver);
        m_render_pass_binder = std::make_unique<VKRenderPassBinder>(driver);

        m_clip_matrix = driver.clip_matrix();

        cmd_begin();
    }

    VKCtx::~VKCtx() {
        WG_AUTO_PROFILE_VULKAN("VKCtx::~VKCtx");

        shutdown();

        WG_LOG_INFO("shutdown vulkan gfx context");
    }

    void VKCtx::update_desc_set(const Ref<GfxDescSet>& set, const GfxDescSetResources& resources) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_desc_set");

        assert(check_thread_valid());
        assert(set);
        assert(!m_in_render_pass);

        auto vk_desc_set = dynamic_cast<VKDescSet*>(set.get());

        vk_desc_set->update(resources);
        vk_desc_set->merge(resources);
    }

    void VKCtx::update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_vert_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKVertBuffer*>(buffer.get())->update(cmd_current(), offset, range, data);
    }
    void VKCtx::update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_index_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKIndexBuffer*>(buffer.get())->update(cmd_current(), offset, range, data);
    }
    void VKCtx::update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_uniform_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKUniformBuffer*>(buffer.get())->update(cmd_current(), offset, range, data);
    }
    void VKCtx::update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_storage_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKStorageBuffer*>(buffer.get())->update(cmd_current(), offset, range, data);
    }
    void VKCtx::update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_texture_2d");

        assert(check_thread_valid());
        assert(texture);
        assert(!m_in_render_pass);

        dynamic_cast<VKTexture*>(texture.get())->update_2d(cmd_current(), mip, region, data);
    }
    void VKCtx::update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_texture_2d_array");

        assert(check_thread_valid());
        assert(texture);
        assert(!m_in_render_pass);

        dynamic_cast<VKTexture*>(texture.get())->update_2d_array(cmd_current(), mip, slice, region, data);
    }
    void VKCtx::update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::update_texture_cube");

        assert(check_thread_valid());
        assert(texture);
        assert(!m_in_render_pass);

        dynamic_cast<VKTexture*>(texture.get())->update_cube(cmd_current(), mip, face, region, data);
    }

    void* VKCtx::map_vert_buffer(const Ref<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::map_vert_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKVertBuffer*>(buffer.get()))->map();
    }
    void* VKCtx::map_index_buffer(const Ref<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::map_index_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKIndexBuffer*>(buffer.get()))->map();
    }
    void* VKCtx::map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::map_uniform_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKUniformBuffer*>(buffer.get()))->map();
    }
    void* VKCtx::map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::map_storage_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKStorageBuffer*>(buffer.get()))->map();
    }
    void VKCtx::unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::unmap_vert_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKVertBuffer*>(buffer.get())->unmap(cmd_current());
    }
    void VKCtx::unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::unmap_index_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKIndexBuffer*>(buffer.get())->unmap(cmd_current());
    }
    void VKCtx::unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::unmap_uniform_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKUniformBuffer*>(buffer.get())->unmap(cmd_current());
    }
    void VKCtx::unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::unmap_storage_buffer");

        assert(check_thread_valid());
        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKStorageBuffer*>(buffer.get())->unmap(cmd_current());
    }

    void VKCtx::barrier_image(const Ref<GfxTexture>& texture, GfxTexBarrierType barrier_type) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::barrier_image");

        dynamic_cast<VKTexture*>(texture.get())->transition_layout(cmd_current(), barrier_type);
    }
    void VKCtx::barrier_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::barrier_buffer");

        dynamic_cast<VKStorageBuffer*>(buffer.get())->barrier(cmd_current());
    }

    void VKCtx::begin_render_pass(const GfxRenderPassDesc& pass_desc, const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::begin_render_pass");

        assert(check_thread_valid());
        assert(!m_in_render_pass);
        assert(pass_desc == GfxRenderPassDesc{});// not supported pass desc yet

        m_render_pass_binder->start(name);
        m_in_render_pass      = true;
        m_comp_pipeline_bound = false;
        m_render_pass_name    = name;
    }
    void VKCtx::bind_target(const Ref<Window>& window) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_target");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(window);

        m_render_pass_binder->bind_target(m_driver.window_manager()->get_or_create(window));
        m_target_bound = true;
    }
    void VKCtx::bind_color_target(const Ref<GfxTexture>& texture, int target, int mip, int slice) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_color_target");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(texture);

        m_render_pass_binder->bind_color_target(texture.cast<VKTexture>(), target, mip, slice);
        m_target_bound = true;
    }
    void VKCtx::bind_depth_target(const Ref<GfxTexture>& texture, int mip, int slice) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_depth_target");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(texture);

        m_render_pass_binder->bind_depth_target(texture.cast<VKTexture>(), mip, slice);
        m_target_bound = true;
    }
    void VKCtx::viewport(const Rect2i& viewport) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::viewport");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(m_target_bound);

        m_viewport = viewport;
    }
    void VKCtx::clear(int target, const Vec4f& color) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::clear");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(m_target_bound);

        m_clear_color[target] = color;
        m_render_pass_binder->clear_color(target);
    }
    void VKCtx::clear(float depth, int stencil) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::clear");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(m_target_bound);

        m_clear_depth   = depth;
        m_clear_stencil = stencil;
        m_render_pass_binder->clear_depth();
        m_render_pass_binder->clear_stencil();
    }
    bool VKCtx::bind_pipeline(const Ref<GfxPsoGraphics>& pipeline) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_pipeline");

        assert(check_thread_valid());
        assert(m_in_render_pass);
        assert(m_target_bound);
        assert(pipeline);

        prepare_render_pass();

        Ref<VKPsoGraphics> new_pipeline = pipeline.cast<VKPsoGraphics>();

        if (new_pipeline == m_current_pso_graphics) {
            return true;
        }
        if (!new_pipeline->validate(m_current_pass)) {
            return false;
        }

        vkCmdBindPipeline(cmd_current(), VK_PIPELINE_BIND_POINT_GRAPHICS, new_pipeline->pipeline());
        m_current_pso_graphics = std::move(new_pipeline);
        m_current_pso_layout   = m_current_pso_graphics->state().layout.cast<VKPsoLayout>();
        m_pipeline_bound       = true;

        return true;
    }
    bool VKCtx::bind_comp_pipeline(const Ref<GfxPsoCompute>& pipeline) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_pipeline");

        assert(check_thread_valid());
        assert(!m_in_render_pass);
        assert(pipeline);

        Ref<VKPsoCompute> new_pipeline = pipeline.cast<VKPsoCompute>();

        if (new_pipeline == m_current_pso_compute) {
            return true;
        }
        if (!new_pipeline->validate()) {
            return false;
        }

        vkCmdBindPipeline(cmd_current(), VK_PIPELINE_BIND_POINT_COMPUTE, new_pipeline->pipeline());
        m_current_pso_compute = std::move(new_pipeline);
        m_current_pso_layout  = m_current_pso_compute->state().layout.cast<VKPsoLayout>();
        m_comp_pipeline_bound = true;

        return true;
    }
    void VKCtx::bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_vert_buffer");

        assert(check_thread_valid());
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);

        m_current_vert_buffers[index]         = buffer.cast<VKVertBuffer>();
        m_current_vert_buffers_offsets[index] = offset;

        VkBuffer     vk_vert_buffer        = m_current_vert_buffers[index]->buffer();
        VkDeviceSize vk_vert_buffer_offset = offset;

        vkCmdBindVertexBuffers(cmd_current(), index, 1, &vk_vert_buffer, &vk_vert_buffer_offset);
    }
    void VKCtx::bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_index_buffer");

        assert(check_thread_valid());
        assert(m_pipeline_bound);
        assert(m_target_bound);
        assert(buffer);

        m_current_index_buffer = buffer.cast<VKIndexBuffer>();
        vkCmdBindIndexBuffer(cmd_current(), m_current_index_buffer->buffer(), offset, VKDefs::get_index_type(index_type));
    }
    void VKCtx::bind_desc_set(const Ref<GfxDescSet>& set, int index) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_desc_set");

        assert(check_thread_valid());
        assert(m_pipeline_bound || m_comp_pipeline_bound);
        assert(set);

        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        if (m_pipeline_bound) {
            bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
        if (m_comp_pipeline_bound) {
            bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
        }

        m_desc_sets[index] = set.cast<VKDescSet>()->set();
        vkCmdBindDescriptorSets(cmd_current(), bind_point, m_current_pso_layout->layout(), index, 1, &m_desc_sets[index], 0, nullptr);
    }
    void VKCtx::bind_desc_sets(const array_view<GfxDescSet*>& sets, int offset) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::bind_desc_sets");

        assert(check_thread_valid());
        assert(m_pipeline_bound || m_comp_pipeline_bound);
        assert(!sets.empty());

        const int count = int(sets.size());

        for (int i = 0; i < count; i++) {
            assert(i + offset < GfxLimits::MAX_DESC_SETS);
            m_desc_sets[i + offset] = dynamic_cast<VKDescSet*>(sets[i])->set();
        }

        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        if (m_pipeline_bound) {
            bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
        if (m_comp_pipeline_bound) {
            bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
        }

        vkCmdBindDescriptorSets(cmd_current(), bind_point, m_current_pso_layout->layout(), offset, count, &m_desc_sets[offset], 0, nullptr);
    }
    void VKCtx::draw(int vertex_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::draw");

        assert(check_thread_valid());
        assert(m_pipeline_bound);
        assert(m_target_bound);

        vkCmdDraw(cmd_current(), vertex_count, instance_count, base_vertex, 0);
    }
    void VKCtx::draw_indexed(int index_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::draw_indexed");

        assert(check_thread_valid());
        assert(m_pipeline_bound);
        assert(m_target_bound);

        vkCmdDrawIndexed(cmd_current(), index_count, instance_count, 0, base_vertex, 0);
    }
    void VKCtx::dispatch(Vec3i group_count) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::dispatch");

        assert(check_thread_valid());
        assert(m_comp_pipeline_bound);

        vkCmdDispatch(cmd_current(), std::uint32_t(group_count.x()), std::uint32_t(group_count.y()), std::uint32_t(group_count.z()));
    }
    void VKCtx::end_render_pass() {
        WG_AUTO_PROFILE_VULKAN("VKCtx::end_render_pass");

        assert(check_thread_valid());
        assert(m_in_render_pass);

        if (!m_render_pass_started) {
            prepare_render_pass();
        }

        assert(m_render_pass_started);

        vkCmdEndRenderPass(cmd_current());
        WG_VK_END_LABEL(cmd_current());

        m_render_pass_binder->finish(cmd_current());
        m_current_pass.reset();
        m_current_pso_graphics.reset();
        m_current_pso_layout.reset();
        m_current_index_buffer.reset();
        m_current_vert_buffers.fill(nullptr);
        m_current_vert_buffers_offsets.fill(0);
        m_clear_color.fill(Vec4f());
        m_clear_depth   = 1.0f;
        m_clear_stencil = 0;
        m_viewport      = Rect2i(0, 0, 0, 0);

        m_in_render_pass      = false;
        m_render_pass_started = false;
        m_pipeline_bound      = false;
        m_target_bound        = false;
    }

    void VKCtx::execute(const std::function<void(GfxCtx* thread_ctx)>& functor) {
        WG_AUTO_PROFILE_VULKAN("VKCtx::execute");

        functor(this);
    }
    void VKCtx::shutdown() {
        WG_AUTO_PROFILE_VULKAN("VKCtx::shutdown");

        m_render_pass_binder.reset();
        m_cmd_manager.reset();
    }

    void VKCtx::begin_frame() {
        WG_AUTO_PROFILE_VULKAN("VKCtx::begin_frame");
    }
    void VKCtx::end_frame() {
        WG_AUTO_PROFILE_VULKAN("VKCtx::end_frame");

        assert(!m_render_pass_started);

        m_cmd_manager->update();

        m_current_pso_graphics.reset();
        m_current_pso_compute.reset();
        m_current_pso_layout.reset();

        m_target_bound        = false;
        m_pipeline_bound      = false;
        m_comp_pipeline_bound = false;
        m_render_pass_started = false;
    }

    void VKCtx::begin_label(const Strid& label) {
        assert(!m_in_render_pass);

        WG_VK_BEGIN_LABEL(cmd_current(), label);
    }
    void VKCtx::end_label() {
        assert(!m_in_render_pass);

        WG_VK_END_LABEL(cmd_current());
    }

    const Mat4x4f& VKCtx::clip_matrix() const {
        return m_clip_matrix;
    }
    GfxCtxType VKCtx::ctx_type() const {
        return m_ctx_type;
    }
    CallbackStream* VKCtx::cmd_stream() {
        return m_driver.cmd_stream();
    }
    bool VKCtx::check_thread_valid() {
        return m_ctx_type != GfxCtxType::Immediate || m_driver.on_gfx_thread();
    }

    void VKCtx::prepare_render_pass() {
        WG_AUTO_PROFILE_VULKAN("VKCtx::prepare_render_pass");

        if (!m_render_pass_started) {
            // Potentially recreate make pass and framebuffer
            m_render_pass_binder->validate(cmd_current());
            m_current_pass = m_render_pass_binder->render_pass();

            std::array<VkClearValue, GfxLimits::MAX_COLOR_TARGETS + 1> clear_values{};
            int                                                        clear_value_count = 0;

            for (int i = 0; i < m_current_pass->color_targets_count(); i++) {
                VkClearColorValue value;
                value.float32[0]                        = m_clear_color[i].x();
                value.float32[1]                        = m_clear_color[i].y();
                value.float32[2]                        = m_clear_color[i].z();
                value.float32[3]                        = m_clear_color[i].w();
                clear_values[clear_value_count++].color = value;
            }
            if (m_current_pass->has_depth_stencil()) {
                VkClearDepthStencilValue value;
                value.depth                                    = m_clear_depth;
                value.stencil                                  = m_clear_stencil;
                clear_values[clear_value_count++].depthStencil = value;
            }

            VkRect2D render_area{};
            render_area.offset.x      = 0;
            render_area.offset.y      = 0;
            render_area.extent.width  = m_render_pass_binder->width();
            render_area.extent.height = m_render_pass_binder->height();

            VkRenderPassBeginInfo render_pass_info{};
            render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            render_pass_info.renderPass      = m_render_pass_binder->render_pass()->render_pass();
            render_pass_info.framebuffer     = m_render_pass_binder->framebuffer()->framebuffer();
            render_pass_info.renderArea      = render_area;
            render_pass_info.clearValueCount = clear_value_count;
            render_pass_info.pClearValues    = clear_values.data();

            WG_VK_BEGIN_LABEL(cmd_current(), m_render_pass_name);
            vkCmdBeginRenderPass(cmd_current(), &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport;
            viewport.x        = static_cast<float>(m_viewport.x());
            viewport.y        = static_cast<float>(m_viewport.y());
            viewport.width    = static_cast<float>(m_viewport.z());
            viewport.height   = static_cast<float>(m_viewport.w());
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            vkCmdSetViewport(cmd_current(), 0, 1, &viewport);

            VkRect2D scissor;
            scissor.offset.x      = m_viewport.x();
            scissor.offset.y      = m_viewport.y();
            scissor.extent.width  = m_viewport.z();
            scissor.extent.height = m_viewport.w();
            vkCmdSetScissor(cmd_current(), 0, 1, &scissor);

            m_render_pass_started = true;
        }
    }

}// namespace wmoge