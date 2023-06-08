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
#include "vk_driver.hpp"

#include "debug/profiler.hpp"

namespace wmoge {

    VKTexture::VKTexture(class VKDriver& driver) : VKResource<GfxTexture>(driver) {
    }
    VKTexture::~VKTexture() {
        WG_AUTO_PROFILE_VULKAN("VKTexture::~VKTexture");

        for (auto view : m_rt_views)
            vkDestroyImageView(m_driver.device(), view, nullptr);
        if (m_view)
            vkDestroyImageView(m_driver.device(), m_view, nullptr);
        if (m_image && m_allocation)
            m_driver.mem_manager()->deallocate(m_image, m_allocation);
    }
    void VKTexture::create_2d(VkCommandBuffer cmd, int width, int height, VkImage image, VkFormat format, const StringId& name) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::create_2d");

        m_tex_type     = GfxTex::Tex2d;
        m_width        = width;
        m_height       = height;
        m_depth        = 1;
        m_array_slices = 1;
        m_mips_count   = 1;
        m_image        = image;
        m_usages.set(GfxTexUsageFlag::ColorTarget);
        m_mem_usage      = GfxMemUsage::GpuLocal;
        m_name           = name;
        m_usage_flags    = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        m_primary_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_current_layout = VK_IMAGE_LAYOUT_UNDEFINED;

        switch (format) {
            case VK_FORMAT_R8G8B8A8_SRGB:
                m_format = GfxFormat::SRGB8_ALPHA8;
                break;
            case VK_FORMAT_B8G8R8A8_SRGB:
                m_format = GfxFormat::SBGR8_ALPHA8;
                break;
            default:
                WG_LOG_ERROR("unsupported vk format for surface color target");
                break;
        }

        init_view();
        init_layout(cmd);
    }
    void VKTexture::create_2d(VkCommandBuffer cmd, int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::create_2d");

        m_tex_type     = GfxTex::Tex2d;
        m_width        = width;
        m_height       = height;
        m_depth        = 1;
        m_array_slices = 1;
        m_mips_count   = mips;
        m_format       = format;
        m_usages       = usages;
        m_name         = name;
        m_mem_usage    = mem_usage;

        init_image();
        init_view();
        init_rt_views();
        init_layout(cmd);
    }
    void VKTexture::create_2d_array(VkCommandBuffer cmd, int width, int height, int mips, int slices, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::create_2d_array");

        m_tex_type     = GfxTex::Tex2dArray;
        m_width        = width;
        m_height       = height;
        m_depth        = 1;
        m_array_slices = slices;
        m_mips_count   = mips;
        m_format       = format;
        m_usages       = usages;
        m_name         = name;
        m_mem_usage    = mem_usage;

        init_image();
        init_view();
        init_rt_views();
        init_layout(cmd);
    }
    void VKTexture::create_cube(VkCommandBuffer cmd, int width, int height, int mips, GfxFormat format, GfxTexUsages usages, GfxMemUsage mem_usage, const StringId& name) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::create_cube");

        m_tex_type     = GfxTex::TexCube;
        m_width        = width;
        m_height       = height;
        m_depth        = 1;
        m_array_slices = GfxLimits::MAX_CUBE_FACES;
        m_mips_count   = mips;
        m_format       = format;
        m_usages       = usages;
        m_name         = name;
        m_mem_usage    = mem_usage;

        init_image();
        init_view();
        init_rt_views();
        init_layout(cmd);
    }
    void VKTexture::update_2d(VkCommandBuffer cmd, int mip, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::update_2d");

        update(cmd, mip, 0, region, data);
    }
    void VKTexture::update_2d_array(VkCommandBuffer cmd, int mip, int slice, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::update_2d_array");

        update(cmd, mip, slice, region, data);
    }
    void VKTexture::update_cube(VkCommandBuffer cmd, int mip, int face, Rect2i region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::update_cube");

        update(cmd, mip, face, region, data);
    }
    void VKTexture::transition_layout(VkCommandBuffer cmd, VkImageLayout destination) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::transition_layout");

        VkImageSubresourceRange subresource{};
        subresource.aspectMask     = VKDefs::get_aspect_flags(m_format);
        subresource.baseMipLevel   = 0;
        subresource.levelCount     = m_mips_count;
        subresource.baseArrayLayer = 0;
        subresource.layerCount     = m_array_slices;
        transition_layout(cmd, destination, subresource);
    }
    void VKTexture::transition_layout(VkCommandBuffer cmd, VkImageLayout destination, const VkImageSubresourceRange& range) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::transition_layout");

        if (destination == m_current_layout)
            return;

        VkImageMemoryBarrier barrier{};
        barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout           = m_current_layout;
        barrier.newLayout           = destination;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image               = m_image;
        barrier.subresourceRange    = range;

        VkPipelineStageFlags src_stage_flags;
        VkPipelineStageFlags dst_stage_flags;

        auto get_dst_layout_settings = [](VkImageLayout layout, VkAccessFlags& access, VkPipelineStageFlags& stage) {
            switch (layout) {
                case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                    access = VK_ACCESS_TRANSFER_WRITE_BIT;
                    stage  = VK_PIPELINE_STAGE_TRANSFER_BIT;
                    break;
                case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                    access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    stage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                    break;
                case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
                case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                    access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    stage  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                    break;
                case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                    access = VK_ACCESS_SHADER_READ_BIT;
                    stage  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                    break;
                case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                    access = 0;
                    stage  = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                    break;
                default:
                    WG_LOG_ERROR("unsupported dst image layout");
                    return;
            }
        };

        if (m_current_layout == VK_IMAGE_LAYOUT_UNDEFINED) {
            barrier.srcAccessMask = 0;
            src_stage_flags       = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else if (m_current_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            src_stage_flags       = VK_PIPELINE_STAGE_TRANSFER_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else if (m_current_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            src_stage_flags       = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else if (m_current_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            src_stage_flags       = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else if (m_current_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            src_stage_flags       = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else if (m_current_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            src_stage_flags       = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else if (m_current_layout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
            barrier.srcAccessMask = 0;
            src_stage_flags       = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            get_dst_layout_settings(destination, barrier.dstAccessMask, dst_stage_flags);
        } else {
            WG_LOG_ERROR("unsupported src image layout");
            return;
        }

        m_current_layout = destination;

        vkCmdPipelineBarrier(cmd, src_stage_flags, dst_stage_flags, 0,
                             0, nullptr, 0, nullptr, 1, &barrier);
    }
    void VKTexture::init_image() {
        WG_AUTO_PROFILE_VULKAN("VKTexture::init_image");

        m_usage_flags = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        if (m_usages.get(GfxTexUsageFlag::ColorTarget)) {
            m_primary_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            assert(!m_usages.get(GfxTexUsageFlag::DepthStencilTarget));
            assert(!m_usages.get(GfxTexUsageFlag::DepthTarget));
        }
        if (m_usages.get(GfxTexUsageFlag::DepthStencilTarget)) {
            m_primary_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            assert(!m_usages.get(GfxTexUsageFlag::ColorTarget));
            assert(!m_usages.get(GfxTexUsageFlag::DepthTarget));
        }
        if (m_usages.get(GfxTexUsageFlag::DepthTarget)) {
            m_primary_layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            assert(!m_usages.get(GfxTexUsageFlag::ColorTarget));
            assert(!m_usages.get(GfxTexUsageFlag::DepthStencilTarget));
        }
        if (m_usages.get(GfxTexUsageFlag::Sampling)) {
            m_primary_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            m_usage_flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        auto queues  = m_driver.queues();
        auto mem_man = m_driver.mem_manager();

        VkImageCreateInfo image_info{};
        image_info.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.pNext                 = nullptr;
        image_info.imageType             = VKDefs::get_image_type(m_tex_type);
        image_info.extent.width          = m_width;
        image_info.extent.height         = m_height;
        image_info.extent.depth          = m_depth;
        image_info.mipLevels             = m_mips_count;
        image_info.arrayLayers           = m_array_slices;
        image_info.format                = VKDefs::get_format(m_format);
        image_info.tiling                = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage                 = m_usage_flags;
        image_info.sharingMode           = queues->mode();
        image_info.queueFamilyIndexCount = static_cast<uint32_t>(queues->unique_families().size());
        image_info.pQueueFamilyIndices   = queues->unique_families().data();
        image_info.samples               = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags                 = m_tex_type == GfxTex::TexCube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

        mem_man->allocate(image_info, m_mem_usage, m_image, m_allocation);
        WG_VK_NAME(m_driver.device(), m_image, VK_OBJECT_TYPE_IMAGE, "image@" + name().str());
    }
    void VKTexture::init_view() {
        WG_AUTO_PROFILE_VULKAN("VKTexture::init_view");

        VkImageViewCreateInfo view_info{};
        view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image                           = m_image;
        view_info.viewType                        = VKDefs::get_view_type(m_tex_type);
        view_info.format                          = VKDefs::get_format(m_format);
        view_info.subresourceRange.aspectMask     = VKDefs::get_aspect_flags(m_format);
        view_info.subresourceRange.baseMipLevel   = 0;
        view_info.subresourceRange.levelCount     = m_mips_count;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount     = m_array_slices;
        view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

        WG_VK_CHECK(vkCreateImageView(m_driver.device(), &view_info, nullptr, &m_view));
        WG_VK_NAME(m_driver.device(), m_view, VK_OBJECT_TYPE_IMAGE_VIEW, "view@ " + name().str());
    }
    void VKTexture::init_rt_views() {
        WG_AUTO_PROFILE_VULKAN("VKTexture::init_rt_views");

        if (!m_usages.get(GfxTexUsageFlag::ColorTarget) &&
            !m_usages.get(GfxTexUsageFlag::DepthTarget) &&
            !m_usages.get(GfxTexUsageFlag::DepthStencilTarget))
            return;

        m_rt_views.resize(m_array_slices * m_mips_count, VK_NULL_HANDLE);

        for (int slice = 0; slice < m_array_slices; slice++) {
            for (int mip = 0; mip < m_mips_count; mip++) {
                VkImageViewCreateInfo view_info{};
                view_info.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view_info.image                           = m_image;
                view_info.viewType                        = VKDefs::get_view_type(m_tex_type);
                view_info.format                          = VKDefs::get_format(m_format);
                view_info.subresourceRange.aspectMask     = VKDefs::get_aspect_flags(m_format);
                view_info.subresourceRange.baseMipLevel   = mip;
                view_info.subresourceRange.levelCount     = 1;
                view_info.subresourceRange.baseArrayLayer = slice;
                view_info.subresourceRange.layerCount     = 1;
                view_info.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
                view_info.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;

                VkImageView& view     = m_rt_views[slice * m_mips_count + mip];
                std::string  dbg_name = "rt_view@" + name().str() + "[" + std::to_string(slice) + "," + std::to_string(mip) + "]";

                WG_VK_CHECK(vkCreateImageView(m_driver.device(), &view_info, nullptr, &view));
                WG_VK_NAME(m_driver.device(), view, VK_OBJECT_TYPE_IMAGE_VIEW, dbg_name);
            }
        }
    }
    void VKTexture::init_layout(VkCommandBuffer cmd) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::init_layout");

        assert(m_driver.on_gfx_thread());
        assert(cmd != VK_NULL_HANDLE);

        transition_layout(cmd, m_primary_layout);
    }

    void VKTexture::update(VkCommandBuffer cmd, int mip, int slice, const Rect2i& region, const Ref<Data>& data) {
        WG_AUTO_PROFILE_VULKAN("VKTexture::update");

        assert(data);
        assert(mip < m_mips_count);
        assert(slice < m_array_slices);

        auto mem_man = m_driver.mem_manager();

        VkBuffer      staging_buffer;
        VmaAllocation staging_allocation;

        mem_man->staging_allocate(data->size(), staging_buffer, staging_allocation);
        void* staging_ptr = mem_man->staging_map(staging_allocation);
        std::memcpy(staging_ptr, data->buffer(), data->size());
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

        transition_layout(cmd, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, subresource);
        vkCmdCopyBufferToImage(cmd, staging_buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);
        transition_layout(cmd, m_primary_layout, subresource);
    }
}// namespace wmoge
