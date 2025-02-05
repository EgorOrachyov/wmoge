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

#include "vk_texture.hpp"

#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    VKTexture::VKTexture(class VKDriver& driver) : VKResource<GfxTexture>(driver) {
    }
    VKTexture::~VKTexture() {
        WG_PROFILE_CPU_VULKAN("VKTexture::~VKTexture");

        for (auto view : m_rt_views) {
            vkDestroyImageView(m_driver.device(), view, nullptr);
        }
        if (m_view) {
            vkDestroyImageView(m_driver.device(), m_view, nullptr);
        }
        if (m_image && m_allocation) {
            m_driver.mem_manager()->deallocate(m_image, m_allocation);
        }
    }
    void VKTexture::create(const GfxTextureDesc& desc, const Strid& name) {
        m_desc = desc;
        m_name = name;

        init_image();
        init_view();
        init_rt_views();
    }
    void VKTexture::create_2d(int width, int height, VkImage image, VkFormat format, const Strid& name) {
        WG_PROFILE_CPU_VULKAN("VKTexture::create_2d");

        m_desc.tex_type     = GfxTex::Tex2d;
        m_desc.width        = width;
        m_desc.height       = height;
        m_desc.depth        = 1;
        m_desc.array_slices = 1;
        m_desc.mips_count   = 1;
        m_desc.usages.set(GfxTexUsageFlag::ColorTarget);
        m_desc.mem_usage = GfxMemUsage::GpuLocal;

        m_image          = image;
        m_name           = name;
        m_usage_flags    = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        m_primary_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        switch (format) {
            case VK_FORMAT_R8G8B8A8_SRGB:
                m_desc.format = GfxFormat::SRGB8_ALPHA8;
                break;
            case VK_FORMAT_B8G8R8A8_SRGB:
                m_desc.format = GfxFormat::SBGR8_ALPHA8;
                break;
            default:
                WG_LOG_ERROR("unsupported vk format for surface color target");
                break;
        }

        init_view();
    }
    void VKTexture::create_2d(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, GfxTexSwizz swizz, const Strid& name) {
        WG_PROFILE_CPU_VULKAN("VKTexture::create_2d");

        GfxTextureDesc desc;
        desc.tex_type     = GfxTex::Tex2d;
        desc.width        = width;
        desc.height       = height;
        desc.depth        = 1;
        desc.array_slices = 1;
        desc.mips_count   = mips;
        desc.format       = format;
        desc.swizz        = swizz;
        desc.usages       = usages;
        desc.mem_usage    = mem_usage;

        create(desc, name);
    }
    void VKTexture::create_2d_array(int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) {
        WG_PROFILE_CPU_VULKAN("VKTexture::create_2d_array");

        GfxTextureDesc desc;
        desc.tex_type     = GfxTex::Tex2dArray;
        desc.width        = width;
        desc.height       = height;
        desc.depth        = 1;
        desc.array_slices = slices;
        desc.mips_count   = mips;
        desc.format       = format;
        desc.usages       = usages;
        desc.mem_usage    = mem_usage;

        create(desc, name);
    }
    void VKTexture::create_cube(int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const Strid& name) {
        WG_PROFILE_CPU_VULKAN("VKTexture::create_cube");

        GfxTextureDesc desc;
        desc.tex_type     = GfxTex::TexCube;
        desc.width        = width;
        desc.height       = height;
        desc.depth        = 1;
        desc.array_slices = GfxLimits::MAX_CUBE_FACES;
        desc.mips_count   = mips;
        desc.format       = format;
        desc.usages       = usages;
        desc.mem_usage    = mem_usage;

        create(desc, name);
    }
    void VKTexture::update_2d(VkCommandBuffer cmd, int mip, const Rect2i& region, array_view<const std::uint8_t> data) {
        WG_PROFILE_CPU_VULKAN("VKTexture::update_2d");

        update(cmd, mip, 0, region, data);
    }
    void VKTexture::update_2d_array(VkCommandBuffer cmd, int mip, int slice, const Rect2i& region, array_view<const std::uint8_t> data) {
        WG_PROFILE_CPU_VULKAN("VKTexture::update_2d_array");

        update(cmd, mip, slice, region, data);
    }
    void VKTexture::update_cube(VkCommandBuffer cmd, int mip, int face, const Rect2i& region, array_view<const std::uint8_t> data) {
        WG_PROFILE_CPU_VULKAN("VKTexture::update_cube");

        update(cmd, mip, face, region, data);
    }

    VkImageLayout VKTexture::get_layout(GfxTexBarrierType barrier_type) const {
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        if (barrier_type == GfxTexBarrierType::Storage) {
            assert(m_desc.usages.get(GfxTexUsageFlag::Storage));
            layout = VK_IMAGE_LAYOUT_GENERAL;
        }
        if (barrier_type == GfxTexBarrierType::Sampling) {
            assert(m_desc.usages.get(GfxTexUsageFlag::Sampling));
            layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        if (barrier_type == GfxTexBarrierType::RenderTarget) {
            assert(m_desc.usages.get(GfxTexUsageFlag::ColorTarget) ||
                   m_desc.usages.get(GfxTexUsageFlag::DepthTarget) ||
                   m_desc.usages.get(GfxTexUsageFlag::DepthStencilTarget));

            if (m_desc.usages.get(GfxTexUsageFlag::ColorTarget)) {
                layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            }
            if (m_desc.usages.get(GfxTexUsageFlag::DepthTarget) ||
                m_desc.usages.get(GfxTexUsageFlag::DepthStencilTarget)) {
                layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
        }
        if (barrier_type == GfxTexBarrierType::CopySource) {
            layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        }
        if (barrier_type == GfxTexBarrierType::CopyDestination) {
            layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        }
        if (barrier_type == GfxTexBarrierType::Presentation) {
            layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        if (barrier_type == GfxTexBarrierType::Undefined) {
            layout = VK_IMAGE_LAYOUT_UNDEFINED;
        }

        return layout;
    }

    void VKTexture::init_image() {
        WG_PROFILE_CPU_VULKAN("VKTexture::init_image");

        m_usage_flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (m_desc.usages.get(GfxTexUsageFlag::ColorTarget)) {
            m_primary_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            assert(!m_desc.usages.get(GfxTexUsageFlag::DepthStencilTarget));
            assert(!m_desc.usages.get(GfxTexUsageFlag::DepthTarget));
        }
        if (m_desc.usages.get(GfxTexUsageFlag::DepthStencilTarget)) {
            m_primary_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            assert(!m_desc.usages.get(GfxTexUsageFlag::ColorTarget));
            assert(!m_desc.usages.get(GfxTexUsageFlag::DepthTarget));
        }
        if (m_desc.usages.get(GfxTexUsageFlag::DepthTarget)) {
            m_primary_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            assert(!m_desc.usages.get(GfxTexUsageFlag::ColorTarget));
            assert(!m_desc.usages.get(GfxTexUsageFlag::DepthStencilTarget));
        }
        if (m_desc.usages.get(GfxTexUsageFlag::Storage)) {
            m_primary_layout = VK_IMAGE_LAYOUT_GENERAL;
            m_usage_flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (m_desc.usages.get(GfxTexUsageFlag::Sampling)) {
            m_primary_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        auto queues  = m_driver.queues();
        auto mem_man = m_driver.mem_manager();

        VkImageCreateInfo image_info{};
        image_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.pNext                 = nullptr;
        image_info.imageType             = VKDefs::get_image_type(m_desc.tex_type);
        image_info.extent.width          = m_desc.width;
        image_info.extent.height         = m_desc.height;
        image_info.extent.depth          = m_desc.depth;
        image_info.mipLevels             = m_desc.mips_count;
        image_info.arrayLayers           = m_desc.array_slices;
        image_info.format                = VKDefs::get_format(m_desc.format);
        image_info.tiling                = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage                 = m_usage_flags;
        image_info.sharingMode           = queues->mode();
        image_info.queueFamilyIndexCount = static_cast<uint32_t>(queues->unique_families().size());
        image_info.pQueueFamilyIndices   = queues->unique_families().data();
        image_info.samples               = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags                 = m_desc.tex_type == GfxTex::TexCube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

        mem_man->allocate(image_info, m_desc.mem_usage, m_image, m_allocation);
        WG_VK_NAME(m_driver.device(), m_image, VK_OBJECT_TYPE_IMAGE, name().str());
    }
    void VKTexture::init_view() {
        WG_PROFILE_CPU_VULKAN("VKTexture::init_view");

        VkImageViewCreateInfo view_info{};
        view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image                           = m_image;
        view_info.viewType                        = VKDefs::get_view_type(m_desc.tex_type);
        view_info.format                          = VKDefs::get_format(m_desc.format);
        view_info.subresourceRange.aspectMask     = VKDefs::get_aspect_flags(m_desc.format);
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = m_desc.mips_count;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = m_desc.array_slices;

        if (m_desc.swizz == GfxTexSwizz::None) {
            view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        }
        if (m_desc.swizz == GfxTexSwizz::RRRRtoRGBA) {
            view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            view_info.components.g = VK_COMPONENT_SWIZZLE_R;
            view_info.components.b = VK_COMPONENT_SWIZZLE_R;
            view_info.components.a = VK_COMPONENT_SWIZZLE_R;
        }

        WG_VK_CHECK(vkCreateImageView(m_driver.device(), &view_info, nullptr, &m_view));
        WG_VK_NAME(m_driver.device(), m_view, VK_OBJECT_TYPE_IMAGE_VIEW, name().str());
    }
    void VKTexture::init_rt_views() {
        WG_PROFILE_CPU_VULKAN("VKTexture::init_rt_views");

        if (!m_desc.usages.get(GfxTexUsageFlag::ColorTarget) &&
            !m_desc.usages.get(GfxTexUsageFlag::DepthTarget) &&
            !m_desc.usages.get(GfxTexUsageFlag::DepthStencilTarget))
            return;

        m_rt_views.resize(m_desc.array_slices * m_desc.mips_count, VK_NULL_HANDLE);

        for (int slice = 0; slice < m_desc.array_slices; slice++) {
            for (int mip = 0; mip < m_desc.mips_count; mip++) {
                VkImageViewCreateInfo view_info{};
                view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view_info.image                           = m_image;
                view_info.viewType                        = VKDefs::get_view_type(m_desc.tex_type);
                view_info.format                          = VKDefs::get_format(m_desc.format);
                view_info.subresourceRange.aspectMask     = VKDefs::get_aspect_flags(m_desc.format);
                view_info.subresourceRange.baseMipLevel   = mip;
                view_info.subresourceRange.levelCount     = 1;
                view_info.subresourceRange.baseArrayLayer = slice;
                view_info.subresourceRange.layerCount     = 1;
                view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

                VkImageView& view     = m_rt_views[slice * m_desc.mips_count + mip];
                std::string  dbg_name = "rt_view " + name().str() + " slice=" + std::to_string(slice) + " mip=" + std::to_string(mip);

                WG_VK_CHECK(vkCreateImageView(m_driver.device(), &view_info, nullptr, &view));
                WG_VK_NAME(m_driver.device(), view, VK_OBJECT_TYPE_IMAGE_VIEW, dbg_name);
            }
        }
    }

    void VKTexture::update(VkCommandBuffer cmd, int mip, int slice, const Rect2i& region, array_view<const std::uint8_t> data) {
        WG_PROFILE_CPU_VULKAN("VKTexture::update");

        assert(mip < m_desc.mips_count);
        assert(slice < m_desc.array_slices);

        auto mem_man = m_driver.mem_manager();

        VkBuffer      staging_buffer;
        VmaAllocation staging_allocation;

        mem_man->staging_allocate(data.size(), staging_buffer, staging_allocation);
        void* staging_ptr = mem_man->staging_map(staging_allocation);
        std::memcpy(staging_ptr, data.data(), data.size());
        mem_man->staging_unmap(staging_allocation);

        VkImageSubresourceRange subresource{};
        subresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource.baseMipLevel   = mip;
        subresource.levelCount     = 1;
        subresource.baseArrayLayer = slice;
        subresource.layerCount     = 1;

        VkBufferImageCopy copy_region{};
        copy_region.bufferOffset                    = 0;
        copy_region.bufferRowLength                 = 0;
        copy_region.bufferImageHeight               = 0;
        copy_region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        copy_region.imageSubresource.mipLevel       = mip;
        copy_region.imageSubresource.baseArrayLayer = slice;
        copy_region.imageSubresource.layerCount     = 1;
        copy_region.imageOffset                     = {region.x(), region.y(), 0};
        copy_region.imageExtent                     = {static_cast<uint32_t>(region.z()), static_cast<uint32_t>(region.w()), 1};

        vkCmdCopyBufferToImage(cmd, staging_buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
    }
}// namespace wmoge
