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

#include "core/buffered_vector.hpp"
#include "core/data.hpp"
#include "core/flat_map.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_cmd_manager.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_mem_manager.hpp"
#include "gfx/vulkan/vk_pipeline.hpp"
#include "gfx/vulkan/vk_queues.hpp"
#include "gfx/vulkan/vk_render_pass.hpp"
#include "gfx/vulkan/vk_resource.hpp"
#include "gfx/vulkan/vk_vert_format.hpp"
#include "gfx/vulkan/vk_window.hpp"

#include <array>
#include <mutex>
#include <unordered_set>
#include <vector>

namespace wmoge {

    /**
     * @class VKCmdList
     * @brief Vulkan command list object implementation
    */
    class VKCmdList : public VKResource<GfxCmdList> {
    public:
        VKCmdList(VkCommandBuffer cmd_buffer, GfxQueueType queue_type, class VKDriver& driver);
        ~VKCmdList() override;

        void update_vert_buffer(const Ref<GfxVertBuffer>& buffer, int offset, int range, array_view<const std::uint8_t> data) override;
        void update_index_buffer(const Ref<GfxIndexBuffer>& buffer, int offset, int range, array_view<const std::uint8_t> data) override;
        void update_uniform_buffer(const Ref<GfxUniformBuffer>& buffer, int offset, int range, array_view<const std::uint8_t> data) override;
        void update_storage_buffer(const Ref<GfxStorageBuffer>& buffer, int offset, int range, array_view<const std::uint8_t> data) override;
        void update_texture_2d(const Ref<GfxTexture>& texture, int mip, Rect2i region, array_view<const std::uint8_t> data) override;
        void update_texture_2d_array(const Ref<GfxTexture>& texture, int mip, int slice, Rect2i region, array_view<const std::uint8_t> data) override;
        void update_texture_cube(const Ref<GfxTexture>& texture, int mip, int face, Rect2i region, array_view<const std::uint8_t> data) override;

        void* map_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void* map_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void* map_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void* map_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void unmap_vert_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void unmap_index_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void unmap_uniform_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void unmap_storage_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void barrier_image(const Ref<GfxTexture>& texture, GfxTexBarrierType src, GfxTexBarrierType dst) override;
        void barrier_buffer(const Ref<GfxVertBuffer>& buffer) override;
        void barrier_buffer(const Ref<GfxIndexBuffer>& buffer) override;
        void barrier_buffer(const Ref<GfxUniformBuffer>& buffer) override;
        void barrier_buffer(const Ref<GfxStorageBuffer>& buffer) override;

        void barrier_images(array_view<GfxTexture*> textures, GfxTexBarrierType src, GfxTexBarrierType dst) override;
        void barrier_buffers(array_view<GfxBuffer*> buffers) override;

        void begin_render_pass(const GfxRenderPassBeginInfo& pass_desc) override;
        void peek_render_pass(GfxRenderPassRef& rp) override;
        void viewport(const Rect2i& viewport) override;
        void bind_pso(const Ref<GfxPsoGraphics>& pipeline) override;
        void bind_pso(const Ref<GfxPsoCompute>& pipeline) override;
        void bind_vert_buffer(const Ref<GfxVertBuffer>& buffer, int index, int offset = 0) override;
        void bind_index_buffer(const Ref<GfxIndexBuffer>& buffer, GfxIndexType index_type, int offset = 0) override;
        void bind_desc_set(const Ref<GfxDescSet>& set, int index) override;
        void bind_desc_sets(const array_view<GfxDescSet*>& sets, int offset = 0) override;
        void draw(int vertex_count, int base_vertex, int instance_count) override;
        void draw_indexed(int index_count, int base_vertex, int instance_count) override;
        void dispatch(Vec3i group_count) override;
        void end_render_pass() override;

        void begin_label(const Strid& label) override;
        void end_label() override;

        GfxQueueType    get_queue_type() const override { return m_queue_type; }
        VkCommandBuffer get_handle() const { return m_cmd_buffer; }

        void barrier(VKBuffer* buffer);
        void barrier(VKBuffer* buffer, VkDeviceSize offset, VkDeviceSize size);
        void barrier(VKTexture* texture, GfxTexBarrierType src, GfxTexBarrierType dst);
        void barrier(VKTexture* texture, VkImageLayout src, VkImageLayout dst);
        void barrier(VKTexture* texture, VkImageLayout src, VkImageLayout dst, const VkImageSubresourceRange& range);
        void flush_barriers();

    private:
        void reset_state();

    private:
        VkCommandBuffer m_cmd_buffer = VK_NULL_HANDLE;
        GfxQueueType    m_queue_type = GfxQueueType::None;

        Ref<VKRenderPass>                                          m_current_pass;
        Ref<VKFrameBuffer>                                         m_current_fbo;
        Ref<VKWindow>                                              m_current_window;
        Ref<VKPsoGraphics>                                         m_current_pso_graphics;
        Ref<VKPsoCompute>                                          m_current_pso_compute;
        Ref<VKPsoLayout>                                           m_current_pso_layout;
        Ref<VKIndexBuffer>                                         m_current_index_buffer;
        std::array<Ref<VKVertBuffer>, GfxLimits::MAX_VERT_BUFFERS> m_current_vert_buffers{};
        std::array<int, GfxLimits::MAX_VERT_BUFFERS>               m_current_vert_buffers_offsets{};
        std::array<VkDescriptorSet, GfxLimits::MAX_DESC_SETS>      m_desc_sets{};
        Rect2i                                                     m_viewport;

        buffered_vector<VkBufferMemoryBarrier, GfxLimits::NUM_INLINE_BARRIERS> m_barriers_buffer;
        buffered_vector<VkImageMemoryBarrier, GfxLimits::NUM_INLINE_BARRIERS>  m_barriers_image;

        bool m_is_started              = false;
        bool m_in_render_pass          = false;
        bool m_pipeline_bound_graphics = false;
        bool m_pipeline_bound_compute  = false;
    };

}// namespace wmoge