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
#include "core/flat_map.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/vulkan/vk_defs.hpp"

namespace wmoge {

    /**
     * @class VKQueues
     * @brief Manages device queues for commands execution
     */
    class VKQueues {
    public:
        static const uint32_t INVALID_QUEUE_INDEX = 0xffffffff;
        VKQueues(VkPhysicalDevice device, VkSurfaceKHR surface);

        bool     is_complete() const;
        void     init_queues(VkDevice device);
        VkQueue  get_queue(GfxQueueType queue_type);
        uint32_t get_family_index(GfxQueueType queue_type);

        [[nodiscard]] const buffered_vector<uint32_t>& unique_families() const { return m_families; }
        [[nodiscard]] uint32_t                         gfx_queue_family() const { return m_gfx_queue_family; }
        [[nodiscard]] uint32_t                         cmp_queue_family() const { return m_cmp_queue_family; }
        [[nodiscard]] uint32_t                         tsf_queue_family() const { return m_tsf_queue_family; }
        [[nodiscard]] uint32_t                         prs_queue_family() const { return m_prs_queue_family; }
        [[nodiscard]] VkQueue                          gfx_queue() const { return m_gfx_queue; }
        [[nodiscard]] VkQueue                          cmp_queue() const { return m_cmp_queue; }
        [[nodiscard]] VkQueue                          tsf_queue() const { return m_tsf_queue; }
        [[nodiscard]] VkQueue                          prs_queue() const { return m_prs_queue; }
        [[nodiscard]] VkSharingMode                    mode() const { return m_mode; }

    private:
        buffered_vector<uint32_t>                m_families;
        buffered_vector<VkQueueFamilyProperties> m_props;

        VkQueue m_gfx_queue = VK_NULL_HANDLE;
        VkQueue m_cmp_queue = VK_NULL_HANDLE;
        VkQueue m_tsf_queue = VK_NULL_HANDLE;
        VkQueue m_prs_queue = VK_NULL_HANDLE;

        uint32_t m_gfx_queue_family = INVALID_QUEUE_INDEX;
        uint32_t m_cmp_queue_family = INVALID_QUEUE_INDEX;
        uint32_t m_tsf_queue_family = INVALID_QUEUE_INDEX;
        uint32_t m_prs_queue_family = INVALID_QUEUE_INDEX;

        VkSharingMode m_mode;
    };

}// namespace wmoge