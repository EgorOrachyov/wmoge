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

#ifndef WMOGE_VK_CMD_POOL_HPP
#define WMOGE_VK_CMD_POOL_HPP

#include "core/fast_vector.hpp"
#include "gfx/vulkan/vk_defs.hpp"

#include <array>
#include <vector>

namespace wmoge {

    /**
     * @class VKCmdManager
     * @brief Manages creation and recycling of vulkan cmd buffers
     */
    class VKCmdManager {
    public:
        explicit VKCmdManager(class VKDriver& driver);
        ~VKCmdManager();

        void update();

        VkCommandBuffer begin_buffer();
        VkCommandBuffer end_buffer();
        VkCommandBuffer current_buffer() const;

    private:
        struct Allocation {
            VkCommandPool   pool   = VK_NULL_HANDLE;
            VkCommandBuffer buffer = VK_NULL_HANDLE;
        };

        std::array<fast_vector<Allocation>, GfxLimits::FRAMES_IN_FLIGHT> m_used_allocations{};
        fast_vector<Allocation>                                          m_free_allocations{};

        Allocation  m_allocation{};
        std::size_t m_index   = 0 % GfxLimits::FRAMES_IN_FLIGHT;
        int         m_next_id = 0;

        class VKDriver& m_driver;
    };

}// namespace wmoge

#endif//WMOGE_VK_CMD_POOL_HPP
