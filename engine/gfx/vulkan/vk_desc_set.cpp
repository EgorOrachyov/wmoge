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

#include "vk_desc_set.hpp"

#include "debug/profiler.hpp"
#include "gfx/vulkan/vk_desc_manager.hpp"
#include "gfx/vulkan/vk_driver.hpp"

#include <cassert>

namespace wmoge {

    VKDescSetLayout::VKDescSetLayout(const GfxDescSetLayoutDesc& desc, const StringId& name, class VKDriver& driver) : VKResource<GfxDescSetLayout>(driver) {
        m_desc = desc;
        m_name = name;

        VkDescriptorSetLayoutBinding bindings[GfxLimits::MAX_DESC_SET_SIZE];
        uint32_t                     bindings_count = 0;

        for (const GfxDescBinging& entry : m_desc) {
            VkDescriptorSetLayoutBinding& vk_binding = bindings[bindings_count++];
            vk_binding.binding                       = entry.binding;
            vk_binding.descriptorCount               = entry.count;
            vk_binding.descriptorType                = VKDefs::get_desc_type(entry.type);
            vk_binding.stageFlags                    = VK_SHADER_STAGE_ALL;
            vk_binding.pImmutableSamplers            = nullptr;
        }

        VkDescriptorSetLayoutCreateInfo desc_set_layout_info{};
        desc_set_layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        desc_set_layout_info.bindingCount = bindings_count;
        desc_set_layout_info.pBindings    = bindings;
        WG_VK_CHECK(vkCreateDescriptorSetLayout(m_driver.device(), &desc_set_layout_info, nullptr, &m_layout));
        WG_VK_NAME(m_driver.device(), m_layout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, "set_layout " + name.str());
    }
    VKDescSetLayout::~VKDescSetLayout() {
        if (m_layout) {
            vkDestroyDescriptorSetLayout(m_driver.device(), m_layout, nullptr);
            m_layout = VK_NULL_HANDLE;
        }
    }

    VKDescSet::VKDescSet(const GfxDescSetResources& resources, const Ref<VKDescSetLayout>& layout, const StringId& name, class VKDriver& driver) : VKResource<GfxDescSet>(driver) {
        m_name      = name;
        m_resources = resources;
        m_layout    = layout;
        m_set       = m_driver.desc_manager()->allocate(m_layout);

        update(resources);
    }
    VKDescSet::~VKDescSet() {
        if (m_set) {
            m_driver.desc_manager()->free(m_layout, m_set);
            m_set = VK_NULL_HANDLE;
        }
    }
    void VKDescSet::copy(const GfxDescSetResources& resources) {
        m_resources = resources;
    }
    void VKDescSet::merge(const GfxDescSetResources& resources) {
        for (const auto& resource : resources) {
            auto& point = resource.first;
            auto  iter  = std::find_if(m_resources.begin(), m_resources.end(), [&](const auto& e) {
                return e.first == point;
            });

            assert(iter != m_resources.end());

            if (iter == m_resources.end()) {
                WG_LOG_ERROR("no such resource in a desc set");
                continue;
            }

            iter->second = resource.second;
        }
    }
    void VKDescSet::update(const GfxDescSetResources& resources) {
        WG_AUTO_PROFILE_VULKAN("VKDescSet::update");

        std::array<VkWriteDescriptorSet, GfxLimits::MAX_DESC_SET_SIZE>   write_infos{};
        std::array<VkDescriptorImageInfo, GfxLimits::MAX_DESC_SET_SIZE>  image_infos{};
        std::array<VkDescriptorBufferInfo, GfxLimits::MAX_DESC_SET_SIZE> buffer_infos{};

        std::uint32_t writes_count = 0;
        std::uint32_t image_count  = 0;
        std::uint32_t buffer_count = 0;

        for (const auto& [point, value] : resources) {
            VkWriteDescriptorSet& write_info = write_infos[writes_count++];
            write_info.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_info.pNext                 = nullptr;
            write_info.dstSet                = m_set;
            write_info.dstArrayElement       = point.array_element;
            write_info.dstBinding            = point.binding;
            write_info.descriptorType        = VKDefs::get_desc_type(point.type);
            write_info.descriptorCount       = 1;

            switch (point.type) {
                case GfxBindingType::SampledTexture: {
                    VkDescriptorImageInfo& image_info = image_infos[image_count++];
                    image_info.sampler                = value.sampler.cast<VKSampler>()->sampler();
                    image_info.imageView              = value.resource.cast<VKTexture>()->view();
                    image_info.imageLayout            = value.resource.cast<VKTexture>()->primary_layout();
                    write_info.pImageInfo             = &image_info;
                    break;
                }
                case GfxBindingType::UniformBuffer: {
                    VkDescriptorBufferInfo& buffer_info = buffer_infos[buffer_count++];
                    buffer_info.buffer                  = value.resource.cast<VKUniformBuffer>()->buffer();
                    buffer_info.offset                  = value.offset;
                    buffer_info.range                   = value.range;
                    write_info.pBufferInfo              = &buffer_info;
                    break;
                }
                case GfxBindingType::StorageBuffer: {
                    VkDescriptorBufferInfo& buffer_info = buffer_infos[buffer_count++];
                    buffer_info.buffer                  = value.resource.cast<VKStorageBuffer>()->buffer();
                    buffer_info.offset                  = value.offset;
                    buffer_info.range                   = value.range;
                    write_info.pBufferInfo              = &buffer_info;
                    break;
                }
                case GfxBindingType::StorageImage: {
                    VkDescriptorImageInfo& image_info = image_infos[image_count++];
                    image_info.imageView              = value.resource.cast<VKTexture>()->view();
                    image_info.imageLayout            = VK_IMAGE_LAYOUT_GENERAL;
                    write_info.pImageInfo             = &image_info;
                    break;
                }
                default:
                    return;
            }
        }

        vkUpdateDescriptorSets(m_driver.device(), writes_count, write_infos.data(), 0, nullptr);
    }

}// namespace wmoge
