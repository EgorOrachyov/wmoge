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

#include "gfx/vulkan/vk_driver.hpp"
#include "gfx/vulkan/vk_queues.hpp"
#include "profiler/profiler.hpp"

#include <algorithm>
#include <cassert>

namespace wmoge {

    VKCmdManager::VKCmdManager(VKDriver& driver) : m_driver(driver) {
        // Init pools and submit queues using queue info from driver

        VKQueues*    queues        = m_driver.queues();
        GfxQueueType queue_types[] = {GfxQueueType::Graphics, GfxQueueType::Compute, GfxQueueType::Copy};

        for (auto queue_type : queue_types) {
            CmdBufferPool& pool     = m_pools.emplace_back();
            pool.queue_type         = queue_type;
            pool.queue_family_index = queues->get_family_index(queue_type);

            CmdBufferQueue& queue = m_queues.emplace_back();
            queue.queue_type      = queue_type;
            queue.queue           = queues->get_queue(queue_type);
        }
    }

    VKCmdManager::~VKCmdManager() {
        clear();
    }

    void VKCmdManager::update(std::size_t frame_id) {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::update");

        m_frame_id = frame_id;
        m_index    = m_frame_id % GfxLimits::FRAMES_IN_FLIGHT;

        for (CmdBufferPool& pool : m_pools) {
            for (CmdBuffer& buffer : pool.used[m_index]) {
                WG_VK_CHECK(vkResetCommandPool(m_driver.device(), buffer.pool, 0));
                pool.free.push_back(buffer);
            }
            pool.used[m_index].clear();
        }
    }

    VkCommandBuffer VKCmdManager::allocate(GfxQueueType queue_type) {
        CmdBufferPool& pool = m_pools[static_cast<int>(queue_type)];

        if (pool.free.empty()) {
            CmdBuffer allocation{};

            VkCommandPoolCreateInfo pool_info{};
            pool_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            pool_info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
            pool_info.queueFamilyIndex = pool.queue_family_index;

            WG_VK_CHECK(vkCreateCommandPool(m_driver.device(), &pool_info, nullptr, &allocation.pool));
            WG_VK_NAME(m_driver.device(), allocation.pool, VK_OBJECT_TYPE_COMMAND_POOL, "cmd_pool id=" + std::to_string(m_next_dbg_id));

            VkCommandBufferAllocateInfo buffer_info{};
            buffer_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            buffer_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            buffer_info.commandBufferCount = 1;
            buffer_info.commandPool        = allocation.pool;

            WG_VK_CHECK(vkAllocateCommandBuffers(m_driver.device(), &buffer_info, &allocation.buffer));
            WG_VK_NAME(m_driver.device(), allocation.buffer, VK_OBJECT_TYPE_COMMAND_BUFFER, "cmd_buff id=" + std::to_string(m_next_dbg_id));

            pool.free.push_back(allocation);

            ++m_next_dbg_id;
        }

        CmdBuffer allocation = pool.free.back();
        pool.free.pop_back();
        pool.used[m_index].push_back(allocation);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;

        WG_VK_CHECK(vkBeginCommandBuffer(allocation.buffer, &begin_info));

        return allocation.buffer;
    }

    void VKCmdManager::submit(GfxQueueType queue_type, VkCommandBuffer buffer) {
        submit(queue_type, buffer, {}, {}, VK_NULL_HANDLE);
    }

    void VKCmdManager::submit(GfxQueueType queue_type, VkCommandBuffer buffer, array_view<VkSemaphore> wait, array_view<VkSemaphore> signal, VkFence fence) {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::submit");

        assert(buffer);

        WG_VK_CHECK(vkEndCommandBuffer(buffer));

        CmdBufferPool&  pool  = m_pools[static_cast<int>(queue_type)];
        CmdBufferQueue& queue = m_queues[static_cast<int>(queue_type)];

        assert(pool.used[m_index].end() != std::find_if(pool.used[m_index].begin(), pool.used[m_index].end(), [&](const CmdBuffer& other) {
                   return other.buffer == buffer;
               }));

        const auto id         = queue.submits.size();
        const bool need_chain = !queue.submits.empty();

        CmdBufferSubmitInfo& submit = queue.submits.emplace_back();
        submit.buffers.push_back(buffer);
        submit.fence = fence;

        for (auto semaphore : signal) {
            submit.signal.push_back(semaphore);
        }
        for (auto semaphore : wait) {
            submit.wait.push_back(semaphore);
        }

        if (need_chain) {
            CmdBufferSubmitInfo& prev          = queue.submits[id - 1];
            VkSemaphore          sync_semphore = m_driver.semaphore_pool()->allocate();

            prev.signal.push_back(sync_semphore);
            submit.wait.push_back(sync_semphore);
        }
    }

    void VKCmdManager::clear() {
        auto release_allocation = [&](CmdBuffer allocation) {
            vkFreeCommandBuffers(m_driver.device(), allocation.pool, 1, &allocation.buffer);
            vkDestroyCommandPool(m_driver.device(), allocation.pool, nullptr);
        };

        for (CmdBufferPool& pool : m_pools) {
            for (auto& used : pool.used) {
                for (CmdBuffer& buffer : used) {
                    release_allocation(buffer);
                }
                used.clear();
            }
            for (CmdBuffer& buffer : pool.free) {
                release_allocation(buffer);
            }
            pool.free.clear();
        }

        for (CmdBufferQueue& queue : m_queues) {
            queue.submits.clear();
        }
    }

    void VKCmdManager::flush(array_view<VkSemaphore> wait, array_view<VkSemaphore> signal) {
        WG_AUTO_PROFILE_VULKAN("VKCmdManager::flush");

        buffered_vector<VkPipelineStageFlags> wait_flags;

        for (CmdBufferQueue& queue : m_queues) {
            std::size_t num_commands = queue.submits.size();

            const bool is_graphics_queue = queue.queue_type == GfxQueueType::Graphics;

            for (std::size_t i = 0; i < num_commands; i++) {
                CmdBufferSubmitInfo& submit = queue.submits[i];

                if (is_graphics_queue) {
                    const bool is_first = i == 0;
                    const bool is_last  = (i + 1) == num_commands;

                    if (is_first) {
                        for (auto semaphore : wait) {
                            submit.wait.push_back(semaphore);
                        }
                    }
                    if (is_last) {
                        for (auto semaphore : signal) {
                            submit.signal.push_back(semaphore);
                        }
                    }
                }

                wait_flags.clear();
                wait_flags.resize(submit.wait.size(), VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

                VkSubmitInfo submit_info{};
                submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit_info.waitSemaphoreCount   = static_cast<uint32_t>(submit.wait.size());
                submit_info.pWaitSemaphores      = submit.wait.data();
                submit_info.pWaitDstStageMask    = wait_flags.data();
                submit_info.commandBufferCount   = static_cast<uint32_t>(submit.buffers.size());
                submit_info.pCommandBuffers      = submit.buffers.data();
                submit_info.signalSemaphoreCount = static_cast<uint32_t>(submit.signal.size());
                submit_info.pSignalSemaphores    = submit.signal.data();

                WG_VK_CHECK(vkQueueSubmit(queue.queue, 1, &submit_info, submit.fence));
            }

            queue.submits.clear();
        }
    }

}// namespace wmoge