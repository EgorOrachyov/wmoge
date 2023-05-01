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

#ifndef WMOGE_VK_MEM_MANAGER_HPP
#define WMOGE_VK_MEM_MANAGER_HPP

#include "gfx/gfx_driver.hpp"
#include "gfx/vulkan/vk_defs.hpp"

#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class VKMemManager
     * @brief Manages memory allocation in vulkan and staging of buffer updates
     */
    class VKMemManager {
    public:
        VKMemManager(class VKDriver& driver);
        ~VKMemManager();

        void update();

        void allocate(VkBufferCreateInfo& buff_info, GfxMemUsage usage, VkBuffer& bufffer, VmaAllocation& allocation);
        void allocate(VkImageCreateInfo& image_info, GfxMemUsage usage, VkImage& image, VmaAllocation& allocation);
        void deallocate(VkBuffer buffer, VmaAllocation allocation);
        void deallocate(VkImage image, VmaAllocation allocation);

        void  staging_allocate(VkDeviceSize size, VkBuffer& buffer, VmaAllocation& allocation);
        void* staging_map(VmaAllocation allocation);
        void  staging_unmap(VmaAllocation allocation);

        void* map(VmaAllocation allocation);
        void  unmap(VmaAllocation allocation);

    private:
        std::vector<std::pair<VkBuffer, VmaAllocation>> m_release_buffs[GfxLimits::FRAMES_IN_FLIGHT];
        std::vector<std::pair<VkImage, VmaAllocation>>  m_release_images[GfxLimits::FRAMES_IN_FLIGHT];
        std::vector<std::pair<VkBuffer, VmaAllocation>> m_release_staging[GfxLimits::FRAMES_IN_FLIGHT];

        VmaPool      m_staging[GfxLimits::FRAMES_IN_FLIGHT];
        VmaAllocator m_vma = VK_NULL_HANDLE;

        int             m_current = 0;
        class VKDriver& m_driver;

        std::mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_VK_MEM_MANAGER_HPP
