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

#define VMA_IMPLEMENTATION
#include "vk_mem_manager.hpp"

#include "gfx/vulkan/vk_driver.hpp"

namespace wmoge {

    VKMemManager::VKMemManager(class VKDriver& driver) : m_driver(driver) {
        VmaVulkanFunctions vk_funcs{};
        vk_funcs.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vk_funcs.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo create_info{};
        create_info.flags            = VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
        create_info.instance         = m_driver.instance();
        create_info.device           = m_driver.device();
        create_info.physicalDevice   = m_driver.phys_device();
        create_info.vulkanApiVersion = VULKAN_VERSION;
        create_info.pVulkanFunctions = &vk_funcs;
        WG_VK_CHECK(vmaCreateAllocator(&create_info, &m_vma));

        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            // Staging buffers used as transfer source only
            VkBufferCreateInfo buff_info{};
            buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buff_info.size  = 1;
            buff_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

            // Staging buffers are host resident (memory for PCIe transfer CPU -> GPU)
            VmaAllocationCreateInfo alloc_info{};
            alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
            alloc_info.flags =
                    VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
                    VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;

            uint32_t mem_type_index;
            vmaFindMemoryTypeIndexForBufferInfo(m_vma, &buff_info, &alloc_info, &mem_type_index);

            VmaPoolCreateInfo pool_info{};
            pool_info.memoryTypeIndex = mem_type_index;
            pool_info.flags           = VMA_POOL_CREATE_LINEAR_ALGORITHM_BIT;
            pool_info.blockSize       = 0;
            pool_info.minBlockCount   = 0;
            pool_info.maxBlockCount   = 0;

            WG_VK_CHECK(vmaCreatePool(m_vma, &pool_info, &m_staging[i]));
        }
    }

    VKMemManager::~VKMemManager() {
        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            update(i);// release next
        }
        for (int i = 0; i < GfxLimits::FRAMES_IN_FLIGHT; i++) {
            vmaDestroyPool(m_vma, m_staging[i]);
        }
        vmaDestroyAllocator(m_vma);
    }

    void VKMemManager::update(std::size_t frame_id) {
        std::lock_guard guard(m_mutex);

        m_frame_id = frame_id;
        m_index    = m_frame_id % GfxLimits::FRAMES_IN_FLIGHT;

        for (auto& to_release : m_release_buffs[m_index])
            vmaDestroyBuffer(m_vma, to_release.first, to_release.second);
        for (auto& to_release : m_release_images[m_index])
            vmaDestroyImage(m_vma, to_release.first, to_release.second);
        for (auto& to_release : m_release_staging[m_index])
            vmaDestroyBuffer(m_vma, to_release.first, to_release.second);

        m_release_buffs[m_index].clear();
        m_release_images[m_index].clear();
        m_release_staging[m_index].clear();
    }

    void VKMemManager::allocate(VkBufferCreateInfo& buff_info, GfxMemUsage usage, VkBuffer& buffer, VmaAllocation& allocation) {
        VmaAllocationCreateInfo alloc_info{};
        alloc_info.requiredFlags = VKDefs::get_memory_properties(usage);
        alloc_info.usage         = VKDefs::get_memory_usage(usage);
        alloc_info.flags         = VKDefs::get_allocation_flags(usage);
        WG_VK_CHECK(vmaCreateBuffer(m_vma, &buff_info, &alloc_info, &buffer, &allocation, nullptr));
    }

    void VKMemManager::allocate(VkImageCreateInfo& image_info, GfxMemUsage usage, VkImage& image, VmaAllocation& allocation) {
        VmaAllocationCreateInfo alloc_info{};
        alloc_info.requiredFlags = VKDefs::get_memory_properties(usage);
        alloc_info.usage         = VKDefs::get_memory_usage(usage);
        alloc_info.flags         = VKDefs::get_allocation_flags(usage);
        WG_VK_CHECK(vmaCreateImage(m_vma, &image_info, &alloc_info, &image, &allocation, nullptr));
    }

    void VKMemManager::deallocate(VkBuffer buffer, VmaAllocation allocation) {
        m_release_buffs[m_index].emplace_back(buffer, allocation);
    }

    void VKMemManager::deallocate(VkImage image, VmaAllocation allocation) {
        m_release_images[m_index].emplace_back(image, allocation);
    }

    void VKMemManager::staging_allocate(VkDeviceSize size, VkBuffer& buffer, VmaAllocation& allocation) {
        std::lock_guard guard(m_mutex);

        VkBufferCreateInfo buff_info{};
        buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buff_info.size  = size;
        buff_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.pool = m_staging[m_index];

        WG_VK_CHECK(vmaCreateBuffer(m_vma, &buff_info, &alloc_info, &buffer, &allocation, nullptr));
        m_release_staging[m_index].emplace_back(buffer, allocation);
    }

    void* VKMemManager::staging_map(VmaAllocation allocation) {
        void* ptr;
        WG_VK_CHECK(vmaMapMemory(m_vma, allocation, &ptr));
        return ptr;
    }

    void VKMemManager::staging_unmap(VmaAllocation allocation) {
        vmaUnmapMemory(m_vma, allocation);
    }

    void* VKMemManager::map(VmaAllocation allocation) {
        void* ptr;
        WG_VK_CHECK(vmaMapMemory(m_vma, allocation, &ptr));
        return ptr;
    }

    void VKMemManager::unmap(VmaAllocation allocation) {
        vmaUnmapMemory(m_vma, allocation);
    }

}// namespace wmoge
