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

#ifndef WMOGE_VK_DESC_MANAGER_HPP
#define WMOGE_VK_DESC_MANAGER_HPP

#include "core/fast_map.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_desc_set.hpp"

#include <array>
#include <bitset>
#include <functional>
#include <memory>
#include <vector>

namespace wmoge {

    /**
     * @class VKDescPoolConfig
     * @brief Config to allocate pool for all engine descriptors
     */
    struct VKDescPoolConfig {
        int max_images = 8 * 1024;
        int max_ub     = 4 * 1024;
        int max_sb     = 2 * 1024;
        int max_sets   = 1 * 1024;
    };

    /**
     * @class VKDescManager
     * @brief Manager allocation of descriptor sets
     */
    class VKDescManager {
    public:
        VKDescManager(const VKDescPoolConfig& config, class VKDriver& driver);
        ~VKDescManager();

        VkDescriptorSet allocate(const Ref<VKDescSetLayout>& layout);
        void            free(const Ref<VKDescSetLayout>& layout, VkDescriptorSet set);

    private:
        fast_map<Ref<VKDescSetLayout>, std::vector<VkDescriptorSet>> m_buckets;
        VkDescriptorPool                                             m_pool = VK_NULL_HANDLE;

        class VKDriver& m_driver;
    };

}// namespace wmoge

#endif//WMOGE_VK_DESC_MANAGER_HPP
