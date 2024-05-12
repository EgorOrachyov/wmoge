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

#include "vk_queues.hpp"

namespace wmoge {

    VKQueues::VKQueues(VkPhysicalDevice device, VkSurfaceKHR surface) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        m_props.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, m_props.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            auto& family = m_props[i];

            VkBool32 presentationSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupported);

            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 &&
                (family.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0 &&
                presentationSupported) {
                m_gfx_queue_family = i;
            }
            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
                (family.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) {
                m_tsf_queue_family = i;
            }
            if ((family.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
                (family.queueFlags & VK_QUEUE_TRANSFER_BIT) == 0 &&
                presentationSupported) {
                m_prs_queue_family = i;
            }
        }
        assert(m_gfx_queue_family != INVALID_QUEUE_INDEX);

        // Fallback
        if (m_tsf_queue_family == INVALID_QUEUE_INDEX) {
            m_tsf_queue_family = m_gfx_queue_family;
        }
        if (m_prs_queue_family == INVALID_QUEUE_INDEX) {
            m_prs_queue_family = m_gfx_queue_family;
        }

        // Compute set of unique families
        m_families.push_back(m_gfx_queue_family);
        if (m_tsf_queue_family != m_gfx_queue_family) m_families.push_back(m_tsf_queue_family);
        if (m_prs_queue_family != m_gfx_queue_family && m_prs_queue_family != m_tsf_queue_family) m_families.push_back(m_prs_queue_family);

        // Sharing mode
        m_mode = m_families.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
    }
    bool VKQueues::is_complete() const {
        return m_gfx_queue_family != INVALID_QUEUE_INDEX && m_tsf_queue_family != INVALID_QUEUE_INDEX && m_prs_queue_family != INVALID_QUEUE_INDEX;
    }
    void VKQueues::init_queues(VkDevice device) {
        vkGetDeviceQueue(device, m_gfx_queue_family, 0, &m_gfx_queue);
        vkGetDeviceQueue(device, m_tsf_queue_family, 0, &m_tsf_queue);
        vkGetDeviceQueue(device, m_prs_queue_family, 0, &m_prs_queue);
    }

}// namespace wmoge
