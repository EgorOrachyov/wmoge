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

#include "vk_semaphore.hpp"

#include "gfx/vulkan/vk_driver.hpp"

namespace wmoge {

    VKSemaphorePool::VKSemaphorePool(VKDriver& driver) : m_driver(driver) {
    }

    VKSemaphorePool::~VKSemaphorePool() {
        clear();
    }

    void VKSemaphorePool::clear() {
        for (auto& list : m_used) {
            for (auto semaphore : list) {
                vkDestroySemaphore(m_driver.device(), semaphore, nullptr);
            }
            list.clear();
        }

        for (auto semaphore : m_free) {
            vkDestroySemaphore(m_driver.device(), semaphore, nullptr);
        }
        m_free.clear();
    }

    void VKSemaphorePool::update(std::size_t frame_id) {
        m_frame_id = frame_id;
        m_index    = m_frame_id % GfxLimits::FRAMES_IN_FLIGHT;

        for (auto semaphore : m_used[m_index]) {
            m_free.push_back(semaphore);
        }

        m_used[m_index].clear();
    }

    VkSemaphore VKSemaphorePool::allocate() {
        if (m_free.empty()) {
            VkSemaphore semaphore;

            VkSemaphoreCreateInfo semaphore_info{};
            semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            WG_VK_CHECK(vkCreateSemaphore(m_driver.device(), &semaphore_info, nullptr, &semaphore));
            WG_VK_NAME(m_driver.device(), semaphore, VK_OBJECT_TYPE_SEMAPHORE, "id=" + std::to_string(m_next_id));

            m_free.push_back(semaphore);
            m_next_id++;
        }

        VkSemaphore semaphore = m_free.back();
        m_free.pop_back();

        m_used[m_index].push_back(semaphore);

        return semaphore;
    }

}// namespace wmoge