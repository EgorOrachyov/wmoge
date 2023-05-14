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

#include "debug/profiler.hpp"
#include "gfx/vulkan/vk_driver.hpp"

namespace wmoge {

    VKDescManager::VKDescManager(class VKDriver& driver) : m_driver(driver) {
        m_write_infos.resize(MAX_RESOURCES_BINDINGS);
        m_image_infos.resize(MAX_TEXTURES_BINDINGS);
        m_buffer_infos.resize(MAX_BUFFERS_BINDINGS);
    }
    VKDescManager::~VKDescManager() {
        auto device = m_driver.device();
        for (auto& entry : m_allocator) {
            auto& pools = entry.second.pools;
            for (auto& list : pools) {
                for (auto pool : list) {
                    vkDestroyDescriptorPool(device, pool, nullptr);
                }
            }
        }
    }
    void VKDescManager::update() {
        WG_AUTO_PROFILE_VULKAN("VKDescManager::update");

        m_frame_index = (m_frame_index + 1) % GfxLimits::FRAMES_IN_FLIGHT;
        auto device   = m_driver.device();
        for (auto& entry : m_allocator) {
            auto& pools        = entry.second;
            pools.current_pool = -1;
            pools.capacity     = 0;
            pools.allocated    = 0;
            auto& list         = pools.pools[m_frame_index];
            for (auto pool : list) {
                WG_VK_CHECK(vkResetDescriptorPool(device, pool, 0));
            }
        }
    }
    void VKDescManager::bind_shader(const Ref<VKShader>& shader) {
        m_shader     = shader;
        m_reflection = shader->reflection();
    }
    void VKDescManager::bind_texture(const StringId& name, int array_element, const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) {
        auto query = m_reflection->textures.find(name);
        if (query == m_reflection->textures.end()) {
            WG_LOG_ERROR("no such texture " << name << " in shader " << m_shader->name());
            return;
        }
        auto& info = query->second;
        bind_texture({info.set, info.binding}, array_element, texture, sampler);
    }
    void VKDescManager::bind_texture(const GfxLocation& location, int array_element, const Ref<GfxTexture>& texture, const Ref<GfxSampler>& sampler) {
        prepare_set(location.set);

        auto& image_info       = m_image_infos[m_image_info_count];
        image_info.sampler     = sampler.cast<VKSampler>()->sampler();
        image_info.imageView   = texture.cast<VKTexture>()->view();
        image_info.imageLayout = texture.cast<VKTexture>()->primary_layout();

        auto& write_info           = m_write_infos[m_write_info_count];
        write_info.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_info.pNext           = nullptr;
        write_info.dstSet          = m_sets[location.set];
        write_info.dstArrayElement = array_element;
        write_info.dstBinding      = location.binding;
        write_info.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write_info.descriptorCount = 1;
        write_info.pImageInfo      = &image_info;

        m_write_info_count += 1;
        m_image_info_count += 1;
    }
    void VKDescManager::bind_uniform_buffer(const StringId& name, int offset, int range, const Ref<GfxUniformBuffer>& buffer) {
        auto query = m_reflection->ub_buffers.find(name);
        if (query == m_reflection->ub_buffers.end()) {
            WG_LOG_ERROR("no such buffer " << name << " in shader " << m_shader->name());
            return;
        }
        auto& info = query->second;
        bind_uniform_buffer({info.set, info.binding}, offset, range, buffer);
    }
    void VKDescManager::bind_uniform_buffer(const GfxLocation& location, int offset, int range, const Ref<GfxUniformBuffer>& buffer) {
        prepare_set(location.set);

        auto& buffer_info  = m_buffer_infos[m_buffer_info_count];
        buffer_info.buffer = buffer.cast<VKUniformBuffer>()->buffer();
        buffer_info.offset = offset;
        buffer_info.range  = range;

        auto& write_info           = m_write_infos[m_write_info_count];
        write_info.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_info.pNext           = nullptr;
        write_info.dstSet          = m_sets[location.set];
        write_info.dstArrayElement = 0;
        write_info.dstBinding      = location.binding;
        write_info.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_info.descriptorCount = 1;
        write_info.pBufferInfo     = &buffer_info;

        m_write_info_count += 1;
        m_buffer_info_count += 1;
    }
    void VKDescManager::bind_storage_buffer(const StringId& name, int offset, int range, const Ref<GfxStorageBuffer>& buffer) {
        auto query = m_reflection->sb_buffers.find(name);
        if (query == m_reflection->sb_buffers.end()) {
            WG_LOG_ERROR("no such buffer " << name << " in shader " << m_shader->name());
            return;
        }
        auto& info = query->second;
        bind_storage_buffer({info.set, info.binding}, offset, range, buffer);
    }
    void VKDescManager::bind_storage_buffer(const GfxLocation& location, int offset, int range, const Ref<GfxStorageBuffer>& buffer) {
        prepare_set(location.set);

        auto& buffer_info  = m_buffer_infos[m_buffer_info_count];
        buffer_info.buffer = buffer.cast<VKStorageBuffer>()->buffer();
        buffer_info.offset = offset;
        buffer_info.range  = range;

        auto& write_info           = m_write_infos[m_write_info_count];
        write_info.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_info.pNext           = nullptr;
        write_info.dstSet          = m_sets[location.set];
        write_info.dstArrayElement = 0;
        write_info.dstBinding      = location.binding;
        write_info.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        write_info.descriptorCount = 1;
        write_info.pBufferInfo     = &buffer_info;

        m_write_info_count += 1;
        m_buffer_info_count += 1;
    }
    void VKDescManager::reset() {
        m_sets.fill(VK_NULL_HANDLE);
        m_write_info_count  = 0;
        m_image_info_count  = 0;
        m_buffer_info_count = 0;
        m_dirty.reset();
    }
    std::array<VkDescriptorSet, GfxLimits::MAX_DESC_SETS>& VKDescManager::sets(int& to_bind) {
        if (m_dirty.any()) {
            vkUpdateDescriptorSets(m_driver.device(), m_write_info_count, m_write_infos.data(), 0, nullptr);
            m_dirty.reset();
            m_write_info_count  = 0;
            m_image_info_count  = 0;
            m_buffer_info_count = 0;
        }

        to_bind = 0;
        while (to_bind < GfxLimits::MAX_DESC_SETS && m_sets[to_bind]) to_bind += 1;

        return m_sets;
    }
    void VKDescManager::allocate_set(int idx) {
        WG_AUTO_PROFILE_VULKAN("VKDescManager::allocate_set");

        VKPoolKey pool_key{};
        pool_key.textures_count   = m_reflection->textures_per_desc[idx];
        pool_key.ub_buffers_count = m_reflection->ub_buffers_per_desc[idx];
        pool_key.sb_buffers_count = m_reflection->sb_buffers_per_desc[idx];

        assert(pool_key.textures_count || pool_key.ub_buffers_count || pool_key.sb_buffers_count);

        auto& pool = m_allocator[pool_key];

        if (pool.allocated == pool.capacity) {
            pool.allocated = 0;
            pool.current_pool += 1;
            pool.capacity = pool.capacity > 0 ? pool.capacity * POOL_SIZE_FACTOR : POOL_SIZE_BASE;

            if (pool.current_pool >= pool.pools[m_frame_index].size()) {
                VkDescriptorPoolSize sizes[3];
                int                  sizes_count = 0;

                if (m_reflection->textures_per_desc[idx] > 0) {
                    sizes[sizes_count].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    sizes[sizes_count].descriptorCount = pool.capacity * m_reflection->textures_per_desc[idx];
                    sizes_count += 1;
                }
                if (m_reflection->ub_buffers_per_desc[idx] > 0) {
                    sizes[sizes_count].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    sizes[sizes_count].descriptorCount = pool.capacity * m_reflection->ub_buffers_per_desc[idx];
                    sizes_count += 1;
                }
                if (m_reflection->sb_buffers_per_desc[idx] > 0) {
                    sizes[sizes_count].type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    sizes[sizes_count].descriptorCount = pool.capacity * m_reflection->sb_buffers_per_desc[idx];
                    sizes_count += 1;
                }

                VkDescriptorPoolCreateInfo create_info{};
                create_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                create_info.poolSizeCount = sizes_count;
                create_info.pPoolSizes    = sizes;
                create_info.maxSets       = pool.capacity;

                VkDescriptorPool descriptor_pool;

                std::stringstream pool_name;
                pool_name << "pool@" << std::to_string(pool.capacity) << "x"
                          << std::to_string(pool_key.textures_count) << ":"
                          << std::to_string(pool_key.ub_buffers_count) << ":"
                          << std::to_string(pool_key.sb_buffers_count);

                WG_VK_CHECK(vkCreateDescriptorPool(m_driver.device(), &create_info, nullptr, &descriptor_pool));
                WG_VK_NAME(m_driver.device(), descriptor_pool, VK_OBJECT_TYPE_DESCRIPTOR_POOL,
                           pool_name.str());

                pool.pools[m_frame_index].push_back(descriptor_pool);
            }
        }

        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool     = pool.pools[m_frame_index][pool.current_pool];
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts        = &m_shader->set_layouts()[idx];
        WG_VK_CHECK(vkAllocateDescriptorSets(m_driver.device(), &alloc_info, &m_sets[idx]));

        pool.allocated += 1;
    }
    void VKDescManager::prepare_set(int idx) {
        if (!m_dirty[idx]) {
            allocate_set(idx);
            m_dirty[idx] = true;
        }
    }

}// namespace wmoge