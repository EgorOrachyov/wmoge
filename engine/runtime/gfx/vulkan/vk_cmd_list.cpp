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

#include "vk_cmd_list.hpp"

#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler.hpp"

#include <cassert>

namespace wmoge {

    VKCmdList::VKCmdList(VkCommandBuffer cmd_buffer, GfxQueueType queue_type, VKDriver& driver) : VKResource<GfxCmdList>(driver) {
        m_cmd_buffer = cmd_buffer;
        m_queue_type = queue_type;
    }

    VKCmdList::~VKCmdList() {
        // nothing to do: command buffer returned on submit to pool
    }

    void VKCmdList::update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_vert_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKVertBuffer*>(buffer.get())->update(m_cmd_buffer, offset, range, data);
    }
    void VKCmdList::update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_index_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKIndexBuffer*>(buffer.get())->update(m_cmd_buffer, offset, range, data);
    }
    void VKCmdList::update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_uniform_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKUniformBuffer*>(buffer.get())->update(m_cmd_buffer, offset, range, data);
    }
    void VKCmdList::update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_storage_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKStorageBuffer*>(buffer.get())->update(m_cmd_buffer, offset, range, data);
    }
    void VKCmdList::update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_texture_2d");

        assert(texture);
        assert(!m_in_render_pass);

        dynamic_cast<VKTexture*>(texture.get())->update_2d(m_cmd_buffer, mip, region, data);
    }
    void VKCmdList::update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_texture_2d_array");

        assert(texture);
        assert(!m_in_render_pass);

        dynamic_cast<VKTexture*>(texture.get())->update_2d_array(m_cmd_buffer, mip, slice, region, data);
    }
    void VKCmdList::update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::update_texture_cube");

        assert(texture);
        assert(!m_in_render_pass);

        dynamic_cast<VKTexture*>(texture.get())->update_cube(m_cmd_buffer, mip, face, region, data);
    }

    void* VKCmdList::map_vert_buffer(const Ref<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::map_vert_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKVertBuffer*>(buffer.get()))->map();
    }
    void* VKCmdList::map_index_buffer(const Ref<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::map_index_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKIndexBuffer*>(buffer.get()))->map();
    }
    void* VKCmdList::map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::map_uniform_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKUniformBuffer*>(buffer.get()))->map();
    }
    void* VKCmdList::map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::map_storage_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        return (dynamic_cast<VKStorageBuffer*>(buffer.get()))->map();
    }
    void VKCmdList::unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::unmap_vert_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKVertBuffer*>(buffer.get())->unmap(m_cmd_buffer);
    }
    void VKCmdList::unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::unmap_index_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKIndexBuffer*>(buffer.get())->unmap(m_cmd_buffer);
    }
    void VKCmdList::unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::unmap_uniform_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKUniformBuffer*>(buffer.get())->unmap(m_cmd_buffer);
    }
    void VKCmdList::unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::unmap_storage_buffer");

        assert(buffer);
        assert(!m_in_render_pass);

        dynamic_cast<VKStorageBuffer*>(buffer.get())->unmap(m_cmd_buffer);
    }

    void VKCmdList::barrier_image(const Ref<GfxTexture>& texture, GfxTexBarrierType barrier_type) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::barrier_image");

        dynamic_cast<VKTexture*>(texture.get())->transition_layout(m_cmd_buffer, barrier_type);
    }
    void VKCmdList::barrier_buffer(const Ref<GfxStorageBuffer>& buffer) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::barrier_buffer");

        dynamic_cast<VKStorageBuffer*>(buffer.get())->barrier(m_cmd_buffer);
    }

    void VKCmdList::begin_render_pass(const GfxRenderPassBeginInfo& pass_desc) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::begin_render_pass");

        assert(!m_in_render_pass);

        m_in_render_pass         = true;
        m_pipeline_bound_compute = false;

        m_current_fbo  = pass_desc.frame_buffer.cast<VKFrameBuffer>();
        m_current_pass = m_current_fbo->desc().render_pass.cast<VKRenderPass>();

        std::array<VkClearValue, GfxLimits::MAX_COLOR_TARGETS + 1> clear_values{};
        int                                                        clear_value_count = 0;

        for (int i = 0; i < m_current_pass->color_targets_count(); i++) {
            VkClearColorValue value;
            value.float32[0]                        = pass_desc.clear_color[i].x();
            value.float32[1]                        = pass_desc.clear_color[i].y();
            value.float32[2]                        = pass_desc.clear_color[i].z();
            value.float32[3]                        = pass_desc.clear_color[i].w();
            clear_values[clear_value_count++].color = value;
        }
        if (m_current_pass->has_depth_stencil()) {
            VkClearDepthStencilValue value;
            value.depth                                    = pass_desc.clear_depth;
            value.stencil                                  = pass_desc.clear_stencil;
            clear_values[clear_value_count++].depthStencil = value;
        }

        VkRect2D render_area{};
        render_area.offset.x      = 0;
        render_area.offset.y      = 0;
        render_area.extent.width  = static_cast<uint32_t>(m_current_pass->get_size().x());
        render_area.extent.height = static_cast<uint32_t>(m_current_pass->get_size().y());

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass      = m_current_pass->render_pass();
        render_pass_info.framebuffer     = m_current_fbo->framebuffer();
        render_pass_info.renderArea      = render_area;
        render_pass_info.clearValueCount = clear_value_count;
        render_pass_info.pClearValues    = clear_values.data();

        vkCmdBeginRenderPass(m_cmd_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
    }
    void VKCmdList::peek_render_pass(GfxRenderPassRef& rp) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::peek_render_pass");

        assert(m_in_render_pass);

        rp = m_current_pass;
    }
    void VKCmdList::viewport(const Rect2i& viewport) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::viewport");

        assert(m_in_render_pass);

        m_viewport = viewport;

        VkViewport vk_viewport;
        vk_viewport.x        = static_cast<float>(m_viewport.x());
        vk_viewport.y        = static_cast<float>(m_viewport.y());
        vk_viewport.width    = static_cast<float>(m_viewport.z());
        vk_viewport.height   = static_cast<float>(m_viewport.w());
        vk_viewport.minDepth = 0.0f;
        vk_viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_cmd_buffer, 0, 1, &vk_viewport);

        VkRect2D vk_scissor;
        vk_scissor.offset.x      = m_viewport.x();
        vk_scissor.offset.y      = m_viewport.y();
        vk_scissor.extent.width  = m_viewport.z();
        vk_scissor.extent.height = m_viewport.w();
        vkCmdSetScissor(m_cmd_buffer, 0, 1, &vk_scissor);
    }
    void VKCmdList::bind_pso(const Ref<GfxPsoGraphics>& pipeline) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::bind_pso");

        assert(m_in_render_pass);
        assert(pipeline);

        Ref<VKPsoGraphics> new_pipeline = pipeline.cast<VKPsoGraphics>();

        vkCmdBindPipeline(m_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, new_pipeline->pipeline());
        m_current_pso_graphics    = std::move(new_pipeline);
        m_current_pso_layout      = m_current_pso_graphics->layout();
        m_pipeline_bound_graphics = true;
    }
    void VKCmdList::bind_pso(const Ref<GfxPsoCompute>& pipeline) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::bind_pso");

        assert(!m_in_render_pass);
        assert(pipeline);

        Ref<VKPsoCompute> new_pipeline = pipeline.cast<VKPsoCompute>();

        vkCmdBindPipeline(m_cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, new_pipeline->pipeline());
        m_current_pso_compute    = std::move(new_pipeline);
        m_current_pso_layout     = m_current_pso_compute->layout();
        m_pipeline_bound_compute = true;
    }
    void VKCmdList::bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::bind_vert_buffer");

        assert(m_pipeline_bound_graphics);
        assert(buffer);

        m_current_vert_buffers[index]         = buffer.cast<VKVertBuffer>();
        m_current_vert_buffers_offsets[index] = offset;

        VkBuffer     vk_vert_buffer        = m_current_vert_buffers[index]->buffer();
        VkDeviceSize vk_vert_buffer_offset = offset;

        vkCmdBindVertexBuffers(m_cmd_buffer, index, 1, &vk_vert_buffer, &vk_vert_buffer_offset);
    }
    void VKCmdList::bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::bind_index_buffer");

        assert(m_pipeline_bound_graphics);
        assert(buffer);

        m_current_index_buffer = buffer.cast<VKIndexBuffer>();
        vkCmdBindIndexBuffer(m_cmd_buffer, m_current_index_buffer->buffer(), offset, VKDefs::get_index_type(index_type));
    }
    void VKCmdList::bind_desc_set(const Ref<GfxDescSet>& set, int index) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::bind_desc_set");

        assert(m_pipeline_bound_graphics || m_pipeline_bound_compute);
        assert(set);

        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        if (m_pipeline_bound_graphics) {
            bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
        if (m_pipeline_bound_compute) {
            bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
        }

        m_desc_sets[index] = set.cast<VKDescSet>()->set();
        vkCmdBindDescriptorSets(m_cmd_buffer, bind_point, m_current_pso_layout->layout(), index, 1, &m_desc_sets[index], 0, nullptr);
    }
    void VKCmdList::bind_desc_sets(const array_view<GfxDescSet*>& sets, int offset) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::bind_desc_sets");

        assert(m_pipeline_bound_graphics || m_pipeline_bound_compute);
        assert(!sets.empty());

        const int count = int(sets.size());

        for (int i = 0; i < count; i++) {
            assert(i + offset < GfxLimits::MAX_DESC_SETS);
            m_desc_sets[i + offset] = dynamic_cast<VKDescSet*>(sets[i])->set();
        }

        VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_MAX_ENUM;

        if (m_pipeline_bound_graphics) {
            bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
        }
        if (m_pipeline_bound_compute) {
            bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
        }

        vkCmdBindDescriptorSets(m_cmd_buffer, bind_point, m_current_pso_layout->layout(), offset, count, &m_desc_sets[offset], 0, nullptr);
    }
    void VKCmdList::draw(int vertex_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::draw");

        assert(m_pipeline_bound_graphics);

        vkCmdDraw(m_cmd_buffer, vertex_count, instance_count, base_vertex, 0);
    }
    void VKCmdList::draw_indexed(int index_count, int base_vertex, int instance_count) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::draw_indexed");

        assert(m_pipeline_bound_graphics);

        vkCmdDrawIndexed(m_cmd_buffer, index_count, instance_count, 0, base_vertex, 0);
    }
    void VKCmdList::dispatch(Vec3i group_count) {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::dispatch");

        assert(m_pipeline_bound_compute);

        vkCmdDispatch(m_cmd_buffer, std::uint32_t(group_count.x()), std::uint32_t(group_count.y()), std::uint32_t(group_count.z()));
    }
    void VKCmdList::end_render_pass() {
        WG_AUTO_PROFILE_VULKAN("VKCmdList::end_render_pass");

        assert(m_in_render_pass);

        vkCmdEndRenderPass(m_cmd_buffer);

        reset_state();
    }

    void VKCmdList::begin_label(const Strid& label) {
        assert(!m_in_render_pass);

        WG_VK_BEGIN_LABEL(m_cmd_buffer, label);
    }

    void VKCmdList::end_label() {
        assert(!m_in_render_pass);

        WG_VK_END_LABEL(m_cmd_buffer);
    }

    void VKCmdList::reset_state() {
        m_current_pass.reset();
        m_current_fbo.reset();
        m_current_window.reset();
        m_current_pso_graphics.reset();
        m_current_pso_compute.reset();
        m_current_pso_layout.reset();
        m_current_index_buffer.reset();
        m_current_vert_buffers.fill(nullptr);
        m_current_vert_buffers_offsets.fill(0);
        m_viewport = Rect2i(0, 0, 0, 0);

        m_in_render_pass          = false;
        m_pipeline_bound_graphics = false;
        m_pipeline_bound_compute  = false;
    }

}// namespace wmoge