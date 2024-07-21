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
#include "core/simple_id.hpp"
#include "gfx/vulkan/vk_defs.hpp"

#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class VKSemaphorePool
     * @brief Vulkan semaphores pool with reuse
    */
    class VKSemaphorePool {
    public:
        VKSemaphorePool(class VKDriver& driver);
        ~VKSemaphorePool();

        void clear();
        void update(std::size_t frame_id);

        VkSemaphore allocate();

    private:
        std::vector<VkSemaphore> m_used[GfxLimits::FRAMES_IN_FLIGHT];
        std::vector<VkSemaphore> m_free;

        std::size_t m_index    = 0 % GfxLimits::FRAMES_IN_FLIGHT;
        std::size_t m_frame_id = 0;

        int m_next_id = 0;

        class VKDriver& m_driver;
    };

}// namespace wmoge