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

#include "vk_desc_manager.hpp"

#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    VKDescManager::VKDescManager(const VKDescPoolConfig& config, class VKDriver& driver) : m_driver(driver) {
        VkDescriptorPoolSize sizes[3];
        sizes[0].type            = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        sizes[0].descriptorCount = config.max_images;
        sizes[1].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        sizes[1].descriptorCount = config.max_ub;
        sizes[2].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        sizes[2].descriptorCount = config.max_sb;

        VkDescriptorPoolCreateInfo create_info{};
        create_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        create_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        create_info.poolSizeCount = 3;
        create_info.pPoolSizes    = sizes;
        create_info.maxSets       = config.max_sets;

        std::stringstream pool_name;
        pool_name << "desc_pool "
                  << "sets=" << std::to_string(config.max_sets) << " "
                  << "images=" << std::to_string(config.max_images) << " "
                  << "ubs=" << std::to_string(config.max_ub) << " "
                  << "sbs=" << std::to_string(config.max_sb);

        WG_VK_CHECK(vkCreateDescriptorPool(m_driver.device(), &create_info, nullptr, &m_pool));
        WG_VK_NAME(m_driver.device(), m_pool, VK_OBJECT_TYPE_DESCRIPTOR_POOL, pool_name.str());
    }
    VKDescManager::~VKDescManager() {
        for (auto& bucket : m_buckets) {
            vkFreeDescriptorSets(m_driver.device(), m_pool, std::uint32_t(bucket.second.size()), bucket.second.data());
        }

        vkDestroyDescriptorPool(m_driver.device(), m_pool, nullptr);
    }

    VkDescriptorSet VKDescManager::allocate(const Ref<VKDescSetLayout>& layout) {
        auto& bucket = m_buckets[layout];

        if (bucket.empty()) {
            VkDescriptorSetLayout layouts[1] = {layout->layout()};
            VkDescriptorSet       sets[1]    = {VK_NULL_HANDLE};

            VkDescriptorSetAllocateInfo alloc_info{};
            alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc_info.descriptorPool     = m_pool;
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts        = layouts;
            WG_VK_CHECK(vkAllocateDescriptorSets(m_driver.device(), &alloc_info, sets));

            bucket.push_back(sets[0]);
        }

        assert(!bucket.empty());

        VkDescriptorSet set = bucket.back();
        bucket.pop_back();

        return set;
    }

    void VKDescManager::free(const Ref<VKDescSetLayout>& layout, VkDescriptorSet set) {
        m_buckets[layout].push_back(set);
    }

}// namespace wmoge