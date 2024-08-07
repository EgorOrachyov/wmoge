/**********************************************************************************/
/* Game engine tutorial                                                           */
/* Available at github https://github.com/EgorOrachyov/game-engine-tutorial       */
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

#include "vk_buffers.hpp"

#include "gfx/vulkan/vk_cmd_list.hpp"
#include "gfx/vulkan/vk_driver.hpp"

namespace wmoge {

    void VKBuffer::init(VkDeviceSize size, VkBufferUsageFlags flags, GfxMemUsage usage) {
        auto queues = driver().queues();

        VkBufferCreateInfo buff_info{};
        buff_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buff_info.size                  = size;
        buff_info.usage                 = flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        buff_info.sharingMode           = queues->mode();
        buff_info.queueFamilyIndexCount = static_cast<uint32_t>(queues->unique_families().size());
        buff_info.pQueueFamilyIndices   = queues->unique_families().data();

        m_size = size;
        driver().mem_manager()->allocate(buff_info, usage, m_buffer, m_allocation);
    }
    void VKBuffer::release() {
        if (m_buffer) {
            assert(m_staging_buffer == VK_NULL_HANDLE);
            assert(m_staging_allocation == VK_NULL_HANDLE);

            driver().mem_manager()->deallocate(m_buffer, m_allocation);
            m_buffer     = VK_NULL_HANDLE;
            m_allocation = VK_NULL_HANDLE;
            m_size       = 0;
        }
    }
    void* VKBuffer::map() {
        assert(m_staging_buffer == VK_NULL_HANDLE);

        auto mem_man = driver().mem_manager();

        // allocate staging buffer of required size, map it preserving buffer
        mem_man->staging_allocate(m_size, m_staging_buffer, m_staging_allocation);
        return mem_man->staging_map(m_staging_allocation);
    }
    void VKBuffer::unmap(VKCmdList* cmd) {
        assert(m_staging_buffer != VK_NULL_HANDLE);

        auto mem_man = driver().mem_manager();
        mem_man->staging_unmap(m_staging_allocation);

        // copy staging mapped buffer into our buffer
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size      = m_size;

        vkCmdCopyBuffer(cmd->get_handle(), m_staging_buffer, m_buffer, 1, &copy_region);
        cmd->barrier(this);

        m_staging_buffer     = VK_NULL_HANDLE;
        m_staging_allocation = VK_NULL_HANDLE;
    }
    void VKBuffer::update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, array_view<const std::uint8_t> data) {
        assert(size > 0);
        assert(size <= data.size());
        assert(offset + size <= m_size);

        auto mem_man = driver().mem_manager();

        VkBuffer      staging_buffer;
        VmaAllocation staging_allocation;

        // allocate staging buffer of required size, map it and update
        mem_man->staging_allocate(size, staging_buffer, staging_allocation);
        void* mapped = mem_man->staging_map(staging_allocation);
        std::memcpy(mapped, data.data(), size);
        mem_man->staging_unmap(staging_allocation);

        // copy staging buffer into our buffer (remember about offset)
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = offset;
        copy_region.size      = size;

        vkCmdCopyBuffer(cmd, staging_buffer, m_buffer, 1, &copy_region);
    }

    VKVertBuffer::VKVertBuffer(VKDriver& driver) : VKResource<GfxVertBuffer>(driver) {
    }
    VKVertBuffer::~VKVertBuffer() {
        release();
    }
    void VKVertBuffer::create(int size, GfxMemUsage usage, const Strid& name) {
        GfxBuffer::m_size   = size;
        GfxBuffer::m_usage  = usage;
        GfxResource::m_name = name;
        VKBuffer::init(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, usage);
        WG_VK_NAME(m_driver.device(), m_buffer, VK_OBJECT_TYPE_BUFFER, name);
    }

    VKIndexBuffer::VKIndexBuffer(VKDriver& driver) : VKResource<GfxIndexBuffer>(driver) {
    }
    VKIndexBuffer::~VKIndexBuffer() {
        release();
    }
    void VKIndexBuffer::create(int size, GfxMemUsage usage, const Strid& name) {
        GfxBuffer::m_size   = size;
        GfxBuffer::m_usage  = usage;
        GfxResource::m_name = name;
        VKBuffer::init(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, usage);
        WG_VK_NAME(m_driver.device(), m_buffer, VK_OBJECT_TYPE_BUFFER, name);
    }

    VKUniformBuffer::VKUniformBuffer(VKDriver& driver) : VKResource<GfxUniformBuffer>(driver) {
    }
    VKUniformBuffer::~VKUniformBuffer() {
        release();
    }
    void VKUniformBuffer::create(int size, GfxMemUsage usage, const Strid& name) {
        GfxBuffer::m_size   = size;
        GfxBuffer::m_usage  = usage;
        GfxResource::m_name = name;
        VKBuffer::init(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, usage);
        WG_VK_NAME(m_driver.device(), m_buffer, VK_OBJECT_TYPE_BUFFER, name);
    }

    VKStorageBuffer::VKStorageBuffer(VKDriver& driver) : VKResource<GfxStorageBuffer>(driver) {
    }
    VKStorageBuffer::~VKStorageBuffer() {
        release();
    }
    void VKStorageBuffer::create(int size, GfxMemUsage usage, const Strid& name) {
        GfxBuffer::m_size   = size;
        GfxBuffer::m_usage  = usage;
        GfxResource::m_name = name;
        VKBuffer::init(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, usage);
        WG_VK_NAME(m_driver.device(), m_buffer, VK_OBJECT_TYPE_BUFFER, name);
    }

}// namespace wmoge