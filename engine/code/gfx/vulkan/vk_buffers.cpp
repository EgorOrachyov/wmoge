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

    VKBuffer::VKBuffer(const GfxBufferDesc& desc, const Strid& name, VKDriver& driver) : VKResource<GfxBuffer>(driver) {
        m_name  = name;
        m_size  = desc.size;
        m_usage = desc.usage;
        m_type  = desc.type;

        auto queues = m_driver.queues();

        VkBufferUsageFlags flags = 0;

        VkBufferCreateInfo buff_info{};
        buff_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buff_info.size                  = m_size;
        buff_info.usage                 = VKDefs::get_buffer_usage(m_type);
        buff_info.sharingMode           = queues->mode();
        buff_info.queueFamilyIndexCount = static_cast<uint32_t>(queues->unique_families().size());
        buff_info.pQueueFamilyIndices   = queues->unique_families().data();

        m_driver.mem_manager()->allocate(buff_info, m_usage, m_buffer, m_allocation);
        WG_VK_NAME(m_driver.device(), m_buffer, VK_OBJECT_TYPE_BUFFER, m_name);
    }

    VKBuffer::~VKBuffer() {
        if (m_buffer) {
            m_driver.mem_manager()->deallocate(m_buffer, m_allocation);
            m_buffer     = VK_NULL_HANDLE;
            m_allocation = VK_NULL_HANDLE;
        }
    }

    void VKBuffer::update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, array_view<const std::uint8_t> data) {
        assert(size > 0);
        assert(size <= data.size());
        assert(offset + size <= m_size);

        auto mem_man = m_driver.mem_manager();

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

}// namespace wmoge