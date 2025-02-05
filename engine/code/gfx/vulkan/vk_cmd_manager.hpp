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

#include "core/array_view.hpp"
#include "core/buffered_vector.hpp"
#include "gfx/vulkan/vk_defs.hpp"

#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class VKCmdManager
     * @brief Manages allocation and submit of command buffers for execution on GPU
     */
    class VKCmdManager {
    public:
        explicit VKCmdManager(class VKDriver& driver);
        ~VKCmdManager();

        void update(std::size_t frame_id);

        VkCommandBuffer allocate(GfxQueueType queue_type);
        void            submit(GfxQueueType queue_type, VkCommandBuffer buffer);
        void            submit(GfxQueueType queue_type, VkCommandBuffer buffer, array_view<VkSemaphore> wait, array_view<VkSemaphore> signal, VkFence fence);
        void            clear();
        void            flush(array_view<VkSemaphore> wait, array_view<VkSemaphore> signal);

    private:
        // Single buffer allocation
        struct CmdBuffer {
            VkCommandPool   pool   = VK_NULL_HANDLE;
            VkCommandBuffer buffer = VK_NULL_HANDLE;
        };

        // Pool of command buffer allocations for a specific queue
        struct CmdBufferPool {
            std::vector<CmdBuffer> used[GfxLimits::FRAMES_IN_FLIGHT];
            std::vector<CmdBuffer> free;
            GfxQueueType           queue_type;
            uint32_t               queue_family_index;
        };

        // Submit info to submit a batch of command buffers to a single queue
        struct CmdBufferSubmitInfo {
            buffered_vector<VkSemaphore>     wait;
            buffered_vector<VkSemaphore>     signal;
            buffered_vector<VkCommandBuffer> buffers;
            VkFence                          fence = VK_NULL_HANDLE;
        };

        // Queue to submit commnad buffers to a single gpu queue
        struct CmdBufferQueue {
            std::vector<CmdBufferSubmitInfo> submits;
            GfxQueueType                     queue_type;
            VkQueue                          queue = VK_NULL_HANDLE;
        };

        buffered_vector<CmdBufferPool>  m_pools;
        buffered_vector<CmdBufferQueue> m_queues;

        std::size_t m_index    = 0 % GfxLimits::FRAMES_IN_FLIGHT;
        std::size_t m_frame_id = 0;

        std::size_t m_next_dbg_id = 0;

        class VKDriver& m_driver;
    };

}// namespace wmoge