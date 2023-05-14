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

#include "vk_render_pass.hpp"
#include "vk_driver.hpp"

#include "debug/profiler.hpp"

namespace wmoge {

    VKRenderPass::VKRenderPass(GfxRenderPassType pass, const StringId& name, VKDriver& driver) : VKResource<GfxRenderPass>(driver) {
        m_name = name;
        m_type = pass;
        m_color_op.fill(VK_ATTACHMENT_LOAD_OP_LOAD);
        m_depth_stencil_op = VK_ATTACHMENT_LOAD_OP_LOAD;
    }
    VKRenderPass::~VKRenderPass() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPass::~VKRenderPass");

        release_framebuffer();
        release_render_pass();
    }
    void VKRenderPass::bind_target(const Ref<VKWindow>& window) {
        m_is_fb_dirty = m_window != window || m_fb_version < window->version();
        m_window      = window;
        // bind to get properties for render pass creation
        bind_color_target(m_window->color()[0], 0, 0, 0);
        bind_depth_target(m_window->depth_stencil(), 0, 0);
    }
    void VKRenderPass::bind_color_target(const Ref<VKTexture>& texture, int target, int mip, int slice) {
        auto& info    = m_color_targets[target];
        auto& format  = m_color_formats[target];
        m_is_fb_dirty = m_is_fb_dirty ||
                        info.texture != texture ||
                        info.mip != mip ||
                        info.slice != slice;
        m_is_pass_dirty = m_is_pass_dirty ||
                          format != VKDefs::get_format(texture->format());
        info.texture = texture;
        info.mip     = mip;
        info.slice   = slice;
        format       = VKDefs::get_format(texture->format());
    }
    void VKRenderPass::bind_depth_target(const Ref<VKTexture>& texture, int mip, int slice) {
        m_is_fb_dirty = m_is_fb_dirty ||
                        m_depth_stencil_target.texture != texture ||
                        m_depth_stencil_target.mip != mip ||
                        m_depth_stencil_target.slice != slice;
        m_is_pass_dirty = m_is_pass_dirty ||
                          m_depth_stencil_format != VKDefs::get_format(texture->format());
        m_depth_stencil_target.texture = texture;
        m_depth_stencil_target.mip     = mip;
        m_depth_stencil_target.slice   = slice;
        m_depth_stencil_format         = VKDefs::get_format(texture->format());
    }
    void VKRenderPass::clear_color(int target) {
        m_is_pass_dirty    = m_is_pass_dirty || (m_color_op[target] != VK_ATTACHMENT_LOAD_OP_CLEAR);
        m_color_op[target] = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    void VKRenderPass::clear_depth_stencil() {
        m_is_pass_dirty    = m_is_pass_dirty || (m_depth_stencil_op != VK_ATTACHMENT_LOAD_OP_CLEAR);
        m_depth_stencil_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
    void VKRenderPass::validate() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPass::validate");

        if (m_is_pass_dirty) {
            release_framebuffer();
            release_render_pass();
            init_render_pass();
        }
        if (m_is_fb_dirty) {
            release_framebuffer();
            init_framebuffer();
        }
    }
    void VKRenderPass::init_render_pass() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPass::init_render_pass");

        int attachments_count      = 0;
        int color_references_count = 0;

        std::array<VkAttachmentDescription, GfxLimits::MAX_COLOR_TARGETS + 1> attachments{};
        std::array<VkAttachmentReference, GfxLimits::MAX_COLOR_TARGETS>       color_references{};
        VkAttachmentReference                                                 depth_stencil_reference{};

        for (int i = 0; i < GfxLimits::MAX_COLOR_TARGETS; i++) {
            if (!m_color_targets[i].texture)
                break;

            attachments[attachments_count].format        = VKDefs::get_format(m_color_targets[i].texture->format());
            attachments[attachments_count].samples       = VK_SAMPLE_COUNT_1_BIT;
            attachments[attachments_count].loadOp        = m_color_op[i];
            attachments[attachments_count].storeOp       = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[attachments_count].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments[attachments_count].finalLayout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            color_references[color_references_count].attachment = attachments_count;
            color_references[color_references_count].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            attachments_count += 1;
            color_references_count += 1;
        }
        if (m_depth_stencil_target.texture) {
            auto layout = m_depth_stencil_target.texture->usages().get(GfxTexUsageFlag::DepthStencilTarget) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;

            attachments[attachments_count].format         = VKDefs::get_format(m_depth_stencil_target.texture->format());
            attachments[attachments_count].samples        = VK_SAMPLE_COUNT_1_BIT;
            attachments[attachments_count].loadOp         = m_depth_stencil_op;
            attachments[attachments_count].storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[attachments_count].stencilLoadOp  = m_depth_stencil_op;
            attachments[attachments_count].stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachments[attachments_count].initialLayout  = layout;
            attachments[attachments_count].finalLayout    = layout;

            depth_stencil_reference.attachment = attachments_count;
            depth_stencil_reference.layout     = layout;

            attachments_count += 1;
        }

        VkSubpassDescription sub_pass{};
        sub_pass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sub_pass.inputAttachmentCount    = 0;
        sub_pass.pInputAttachments       = nullptr;
        sub_pass.colorAttachmentCount    = color_references_count;
        sub_pass.pColorAttachments       = color_references.data();
        sub_pass.pResolveAttachments     = nullptr;
        sub_pass.pDepthStencilAttachment = m_depth_stencil_target.texture ? &depth_stencil_reference : nullptr;
        sub_pass.preserveAttachmentCount = 0;
        sub_pass.pPreserveAttachments    = nullptr;

        VkRenderPassCreateInfo create_info{};
        create_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        create_info.pAttachments    = attachments.data();
        create_info.attachmentCount = attachments_count;
        create_info.subpassCount    = 1;
        create_info.pSubpasses      = &sub_pass;
        create_info.dependencyCount = 0;
        create_info.pDependencies   = nullptr;

        VkRenderPass render_pass;

        WG_VK_CHECK(vkCreateRenderPass(m_driver.device(), &create_info, nullptr, &render_pass));
        WG_VK_NAME(m_driver.device(), render_pass, VK_OBJECT_TYPE_RENDER_PASS, "rp@" + name().str());
        m_render_pass = make_ref<VKRenderPassHnd>(render_pass, color_references_count, m_depth_stencil_target.texture, m_driver);

        m_version += 1;
        m_is_pass_dirty = false;
        m_is_fb_dirty   = true;
    }
    void VKRenderPass::init_framebuffer() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPass::init_framebuffer");

        VkFramebufferCreateInfo create_info{};
        create_info.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = m_render_pass->render_pass();

        if (!m_window) {
            std::array<VkImageView, GfxLimits::MAX_COLOR_TARGETS + 1> views;
            int                                                       views_count = 0;

            for (int i = 0; i < GfxLimits::MAX_COLOR_TARGETS; i++) {
                if (!m_color_targets[i].texture) {
                    break;
                }
                views[views_count++] = m_color_targets[i].texture->rt_view(m_color_targets[i].slice, m_color_targets[i].mip);
            }
            if (m_depth_stencil_target.texture) {
                views[views_count++] = m_depth_stencil_target.texture->rt_view(m_depth_stencil_target.slice, m_depth_stencil_target.mip);
            }

            Ref<VKTexture> ref = m_depth_stencil_target.texture ? m_depth_stencil_target.texture : m_color_targets[0].texture;
            m_size.width       = ref->width();
            m_size.height      = ref->height();

            create_info.attachmentCount = views_count;
            create_info.pAttachments    = views.data();
            create_info.width           = ref->width();
            create_info.height          = ref->height();
            create_info.layers          = 1;

            VkFramebuffer framebuffer;

            WG_VK_CHECK(vkCreateFramebuffer(m_driver.device(), &create_info, nullptr, &framebuffer));
            WG_VK_NAME(m_driver.device(), framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, "fb@" + name().str());
            m_framebuffer = make_ref<VKFramebufferHnd>(framebuffer, m_driver);
        } else {
            auto& colors = m_window->color();
            m_window_framebuffers.resize(colors.size());
            m_size.width  = m_window->width();
            m_size.height = m_window->height();

            for (int i = 0; i < colors.size(); i++) {
                std::array<VkImageView, 2> views;
                views[0] = colors[i]->view();
                views[1] = m_window->depth_stencil()->view();

                create_info.attachmentCount = 2;
                create_info.pAttachments    = views.data();
                create_info.width           = m_window->width();
                create_info.height          = m_window->height();
                create_info.layers          = 1;

                VkFramebuffer framebuffer;

                WG_VK_CHECK(vkCreateFramebuffer(m_driver.device(), &create_info, nullptr, &framebuffer));
                WG_VK_NAME(m_driver.device(), framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, "fb@" + name().str() + std::to_string(i));
                m_window_framebuffers[i] = make_ref<VKFramebufferHnd>(framebuffer, m_driver);
            }

            m_fb_version = m_window->version();
        }
        m_is_fb_dirty = false;
    }
    void VKRenderPass::release_render_pass() {
        m_render_pass.reset();
        m_is_pass_dirty = true;
    }
    void VKRenderPass::release_framebuffer() {
        m_framebuffer.reset();
        m_window_framebuffers.clear();
        m_is_fb_dirty = true;
    }

    VKRenderPassHnd::VKRenderPassHnd(VkRenderPass render_pass, int color_targets, bool has_depth_stencil, VKDriver& driver)
        : VKResource<GfxResource>(driver),
          m_render_pass(render_pass),
          m_color_targets_count(color_targets),
          m_has_depth_stencil(has_depth_stencil) {
    }
    VKRenderPassHnd::~VKRenderPassHnd() {
        if (m_render_pass) {
            vkDestroyRenderPass(m_driver.device(), m_render_pass, nullptr);
        }
    }
    VKFramebufferHnd::VKFramebufferHnd(VkFramebuffer framebuffer, class VKDriver& driver)
        : VKResource<GfxResource>(driver),
          m_framebuffer(framebuffer) {
    }
    VKFramebufferHnd::~VKFramebufferHnd() {
        if (m_framebuffer) {
            vkDestroyFramebuffer(m_driver.device(), m_framebuffer, nullptr);
        }
    }

}// namespace wmoge
