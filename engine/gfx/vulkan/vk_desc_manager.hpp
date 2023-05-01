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

#include "gfx/vulkan/vk_buffers.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_sampler.hpp"
#include "gfx/vulkan/vk_shader.hpp"
#include "gfx/vulkan/vk_texture.hpp"

#include <array>
#include <bitset>
#include <functional>
#include <vector>

namespace wmoge {

    /**
     * @class VKPoolKey
     * @brief Key to access a descriptor pool for a particular layout configuration
     */
    struct VKPoolKey {
    public:
        int textures_count   = 0;
        int ub_buffers_count = 0;
        int sb_buffers_count = 0;

        bool operator==(const VKPoolKey& key) const {
            return textures_count == key.textures_count &&
                   ub_buffers_count == key.ub_buffers_count &&
                   sb_buffers_count == key.sb_buffers_count;
        }
    };

    /**
     * @class VKPool
     * @brief Pool to allocate descriptors with particular resource usage
     */
    struct VKPool {
        std::array<std::vector<VkDescriptorPool>, GfxLimits::FRAMES_IN_FLIGHT> pools{};
        int                                                                    current_pool = -1;
        int                                                                    allocated    = 0;
        int                                                                    capacity     = 0;
    };

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::VKPoolKey> {
    public:
        std::size_t operator()(const wmoge::VKPoolKey& key) const {
            return std::hash<int>()(key.textures_count) ^
                   std::hash<int>()(key.ub_buffers_count) ^
                   std::hash<int>()(key.sb_buffers_count);
        }
    };

}// namespace std

namespace wmoge {

    /**
     * @class VKDescManager
     * @brief Manager for binding and per-frame allocation of descriptor sets
     */
    class VKDescManager {
    public:
        static const int MAX_RESOURCES_BINDINGS = 3 * 128;
        static const int MAX_TEXTURES_BINDINGS  = 128;
        static const int MAX_BUFFERS_BINDINGS   = 2 * 128;
        static const int POOL_SIZE_BASE         = 4;
        static const int POOL_SIZE_FACTOR       = 2;

        VKDescManager(class VKDriver& driver);
        ~VKDescManager();

        void update();

        void bind_shader(const ref_ptr<VKShader>& shader);
        void bind_texture(const StringId& name, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler);
        void bind_texture(const GfxLocation& location, int array_element, const ref_ptr<GfxTexture>& texture, const ref_ptr<GfxSampler>& sampler);
        void bind_uniform_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer);
        void bind_uniform_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxUniformBuffer>& buffer);
        void bind_storage_buffer(const StringId& name, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer);
        void bind_storage_buffer(const GfxLocation& location, int offset, int range, const ref_ptr<GfxStorageBuffer>& buffer);
        void reset();

        std::array<VkDescriptorSet, GfxLimits::MAX_DESC_SETS>& sets(int& to_bind);

    private:
        void allocate_set(int idx);
        void prepare_set(int idx);

    private:
        fast_map<VKPoolKey, VKPool>                           m_allocator{};
        std::array<VkDescriptorSet, GfxLimits::MAX_DESC_SETS> m_sets{};
        std::bitset<GfxLimits::MAX_DESC_SETS>                 m_dirty{};
        std::vector<VkWriteDescriptorSet>                     m_write_infos{};
        std::vector<VkDescriptorImageInfo>                    m_image_infos{};
        std::vector<VkDescriptorBufferInfo>                   m_buffer_infos{};
        const GfxShaderReflection*                            m_reflection;
        ref_ptr<VKShader>                                     m_shader;

        int m_frame_index       = 0;
        int m_write_info_count  = 0;
        int m_image_info_count  = 0;
        int m_buffer_info_count = 0;

        class VKDriver& m_driver;
    };

}// namespace wmoge

#endif//WMOGE_VK_DESC_MANAGER_HPP
