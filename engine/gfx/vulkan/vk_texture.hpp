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

#ifndef WMOGE_VK_TEXTURE_HPP
#define WMOGE_VK_TEXTURE_HPP

#include "core/data.hpp"
#include "gfx/gfx_texture.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"

namespace wmoge {

    /**
     * @class VKTexture
     * @brief Vulkan gfx texture implementation for resources and render targets
     */
    class VKTexture final : public VKResource<GfxTexture> {
    public:
        VKTexture(class VKDriver& driver);
        ~VKTexture() override;

        void create_2d(VkCommandBuffer cmd, int width, int height, VkImage image, VkFormat format, const StringId& name);
        void create_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name);
        void create_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name);
        void create_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name);

        void update_2d(VkCommandBuffer cmd, int mip, Rect2i region, const ref_ptr<Data>& data);
        void update_2d_array(VkCommandBuffer cmd, int mip, int slice, Rect2i region, const ref_ptr<Data>& data);
        void update_cube(VkCommandBuffer cmd, int mip, int face, Rect2i region, const ref_ptr<Data>& data);

        void transition_layout(VkCommandBuffer cmd, VkImageLayout destination);
        void transition_layout(VkCommandBuffer cmd, VkImageLayout destination, const VkImageSubresourceRange& range);

        VkImage       image() const { return m_image; }
        VkImageView   view() const { return m_view; }
        VkImageView   rt_view(int slice, int mip) const { return m_rt_views[slice * m_mips_count + mip]; }
        VkImageLayout primary_layout() const { return m_primary_layout; }
        VkImageLayout current_layout() const { return m_current_layout; }

    private:
        void init_image();
        void init_view();
        void init_rt_views();
        void init_layout();
        void update(VkCommandBuffer cmd, int mip, int slice, const Rect2i& region, const ref_ptr<Data>& data);

    private:
        VkImage                  m_image          = VK_NULL_HANDLE;
        VkImageView              m_view           = VK_NULL_HANDLE;
        VkImageLayout            m_primary_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageLayout            m_current_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageUsageFlags        m_usage_flags    = 0;
        VmaAllocation            m_allocation     = VK_NULL_HANDLE;
        std::vector<VkImageView> m_rt_views;
    };

}// namespace wmoge

#endif//WMOGE_VK_TEXTURE_HPP
