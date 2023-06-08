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

#include "vk_cmd_manager.hpp"

#include "debug/profiler.hpp"
#include "gfx/vulkan/vk_driver.hpp"

namespace wmoge {

    VKCmdManager::VKCmdManager(class VKDriver& driver) : m_driver(driver) {
    }
    VKCmdManager::~VKCmdManager() {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::~VKCmdManager");

        auto release_allocation = [&](Allocation allocation) {
            if (allocation.buffer) {
                vkFreeCommandBuffers(m_driver.device(), allocation.pool, 1, &allocation.buffer);
            }
            if (allocation.pool) {
                vkDestroyCommandPool(m_driver.device(), allocation.pool, nullptr);
            }
        };

        for (auto& allocations : m_used_allocations) {
            for (auto allocation : allocations) {
                release_allocation(allocation);
            }
        }
        for (auto& allocation : m_free_allocations) {
            release_allocation(allocation);
        }
        release_allocation(m_allocation);
    }

    void VKCmdManager::update() {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::update");

        m_index = (m_index + 1) % GfxLimits::FRAMES_IN_FLIGHT;

        for (auto& allocation : m_used_allocations[m_index]) {
            WG_VK_CHECK(vkResetCommandPool(m_driver.device(), allocation.pool, 0));

            m_free_allocations.push_back(allocation);
        }

        m_used_allocations[m_index].clear();
    }

    VkCommandBuffer VKCmdManager::begin_buffer() {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::begin_buffer");

        assert(m_allocation.buffer == VK_NULL_HANDLE);

        if (m_free_allocations.empty()) {
            Allocation allocation{};

            VkCommandPoolCreateInfo pool_info{};
            pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            pool_info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            pool_info.queueFamilyIndex = m_driver.queues()->gfx_queue_family();

            WG_VK_CHECK(vkCreateCommandPool(m_driver.device(), &pool_info, nullptr, &allocation.pool));
            WG_VK_NAME(m_driver.device(), allocation.pool, VK_OBJECT_TYPE_COMMAND_POOL, "pool@" + std::to_string(m_next_id));

            VkCommandBufferAllocateInfo buffer_info{};
            buffer_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            buffer_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            buffer_info.commandBufferCount = 1;
            buffer_info.commandPool        = allocation.pool;

            WG_VK_CHECK(vkAllocateCommandBuffers(m_driver.device(), &buffer_info, &allocation.buffer));
            WG_VK_NAME(m_driver.device(), allocation.buffer, VK_OBJECT_TYPE_COMMAND_BUFFER, "cmd@" + std::to_string(m_next_id));

            m_next_id += 1;

            m_free_allocations.push_back(allocation);
        }

        m_allocation = m_free_allocations.back();
        m_free_allocations.pop_back();

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;

        WG_VK_CHECK(vkBeginCommandBuffer(m_allocation.buffer, &begin_info));

        return m_allocation.buffer;
    }

    VkCommandBuffer VKCmdManager::end_buffer() {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::end_buffer");

        assert(m_allocation.buffer != VK_NULL_HANDLE);
        assert(m_allocation.pool != VK_NULL_HANDLE);

        WG_VK_CHECK(vkEndCommandBuffer(m_allocation.buffer));
        m_used_allocations[m_index].push_back(m_allocation);

        VkCommandBuffer ended_buffer = m_allocation.buffer;
        m_allocation                 = Allocation{};

        return ended_buffer;
    }

    VkCommandBuffer VKCmdManager::current_buffer() const {
        assert(m_allocation.buffer != VK_NULL_HANDLE);

        return m_allocation.buffer;
    }

}// namespace wmoge