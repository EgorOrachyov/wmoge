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

#ifndef WMOGE_VK_DESC_SET_HPP
#define WMOGE_VK_DESC_SET_HPP

#include "core/array_view.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"

namespace wmoge {

    /**
     * @class VKDescSetLayout
     * @brief Vulkan descriptor set layout implementation
     */
    class VKDescSetLayout final : public VKResource<GfxDescSetLayout> {
    public:
        VKDescSetLayout(const GfxDescSetLayoutDesc& desc, const StringId& name, class VKDriver& driver);
        ~VKDescSetLayout() override;

        [[nodiscard]] VkDescriptorSetLayout layout() const { return m_layout; }

    private:
        VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
        VkDescriptorPoolSize  m_sizes[3]{};
    };

    /**
     * @class VKDescSet
     * @brief Vulkan descriptor set implementation
     */
    class VKDescSet final : public VKResource<GfxDescSet> {
    public:
        VKDescSet(const GfxDescSetResources& resources, const Ref<VKDescSetLayout>& layout, const StringId& name, class VKDriver& driver);
        ~VKDescSet() override;

        void merge(const GfxDescSetResources& resources);
        void update(const GfxDescSetResources& resources);

        [[nodiscard]] VkDescriptorSet             set() const { return m_set; }
        [[nodiscard]] const Ref<VKDescSetLayout>& layout() const { return m_layout; }

    private:
        VkDescriptorSet      m_set = VK_NULL_HANDLE;
        Ref<VKDescSetLayout> m_layout;
    };

}// namespace wmoge

#endif//WMOGE_VK_DESC_SET_HPP
