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

#include "core/crc32.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    VKRenderPass::VKRenderPass(const GfxRenderPassDesc& pass_desc, const Strid& name, class VKDriver& driver) : VKResource<GfxRenderPass>(driver), m_pass_desc(pass_desc) {
        m_name = name;

        int  attachments_count      = 0;
        int  color_references_count = 0;
        bool has_depth_stencil      = false;

        std::array<VkAttachmentDescription, GfxLimits::MAX_COLOR_TARGETS + 1> attachments{};
        std::array<VkAttachmentReference, GfxLimits::MAX_COLOR_TARGETS>       color_references{};
        VkAttachmentReference                                                 depth_stencil_reference{};

        for (int i = 0; i < GfxLimits::MAX_COLOR_TARGETS; i++) {
            if (m_pass_desc.color_target_fmts[i] == GfxFormat::Unknown)
                break;

            attachments[attachments_count].format        = VKDefs::get_format(m_pass_desc.color_target_fmts[i]);
            attachments[attachments_count].samples       = VK_SAMPLE_COUNT_1_BIT;
            attachments[attachments_count].loadOp        = VKDefs::load_op(m_pass_desc.color_target_ops[i]);
            attachments[attachments_count].storeOp       = VKDefs::store_op(m_pass_desc.color_target_ops[i]);
            attachments[attachments_count].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            attachments[attachments_count].finalLayout   = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            color_references[color_references_count].attachment = attachments_count;
            color_references[color_references_count].layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            attachments_count += 1;
            color_references_count += 1;
        }
        if (m_pass_desc.depth_stencil_fmt != GfxFormat::Unknown) {
            auto layout = VKDefs::rt_layout_from_fmt(m_pass_desc.depth_stencil_fmt);

            attachments[attachments_count].format         = VKDefs::get_format(m_pass_desc.depth_stencil_fmt);
            attachments[attachments_count].samples        = VK_SAMPLE_COUNT_1_BIT;
            attachments[attachments_count].loadOp         = VKDefs::load_op(m_pass_desc.depth_op);
            attachments[attachments_count].storeOp        = VKDefs::store_op(m_pass_desc.depth_op);
            attachments[attachments_count].stencilLoadOp  = VKDefs::load_op(m_pass_desc.stencil_op);
            attachments[attachments_count].stencilStoreOp = VKDefs::store_op(m_pass_desc.stencil_op);
            attachments[attachments_count].initialLayout  = layout;
            attachments[attachments_count].finalLayout    = layout;

            depth_stencil_reference.attachment = attachments_count;
            depth_stencil_reference.layout     = layout;

            attachments_count += 1;
            has_depth_stencil = true;
        }

        VkSubpassDescription sub_pass{};
        sub_pass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        sub_pass.inputAttachmentCount    = 0;
        sub_pass.pInputAttachments       = nullptr;
        sub_pass.colorAttachmentCount    = color_references_count;
        sub_pass.pColorAttachments       = color_references.data();
        sub_pass.pResolveAttachments     = nullptr;
        sub_pass.pDepthStencilAttachment = has_depth_stencil ? &depth_stencil_reference : nullptr;
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

        WG_VK_CHECK(vkCreateRenderPass(m_driver.device(), &create_info, nullptr, &m_render_pass));
        WG_VK_NAME(m_driver.device(), m_render_pass, VK_OBJECT_TYPE_RENDER_PASS, m_name.str());

        m_color_targets_count = color_references_count;
        m_has_depth_stencil   = has_depth_stencil;
    }
    VKRenderPass::~VKRenderPass() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPass::~VKRenderPass");

        if (m_render_pass) {
            vkDestroyRenderPass(m_driver.device(), m_render_pass, nullptr);
        }
    }

    bool VKFrameBufferDesc::operator==(const VKFrameBufferDesc& other) const {
        return std::memcmp(this, &other, sizeof(VKFrameBufferDesc)) == 0;
    }
    std::size_t VKFrameBufferDesc::hash() const {
        return static_cast<std::size_t>(Crc32Util::hash(this, sizeof(VKFrameBufferDesc)));
    }

    VKFramebufferObject::VKFramebufferObject(const VKFrameBufferDesc& desc, const Strid& name, class VKDriver& driver)
        : VKResource<GfxResource>(driver) {
        m_desc = desc;
        m_name = name;

        VkFramebufferCreateInfo create_info{};
        create_info.sType      = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        create_info.renderPass = desc.render_pass->render_pass();

        std::array<VkImageView, GfxLimits::MAX_COLOR_TARGETS + 1> views;
        int                                                       views_count = 0;

        for (int i = 0; i < GfxLimits::MAX_COLOR_TARGETS; i++) {
            if (!desc.color_targets[i].texture) {
                break;
            }

            auto& texture        = desc.color_targets[i].texture;
            views[views_count++] = texture->has_rt_views() ? texture->rt_view(desc.color_targets[i].slice, desc.color_targets[i].mip) : texture->view();
        }
        if (desc.depth_stencil_target.texture) {
            auto& texture        = desc.depth_stencil_target.texture;
            views[views_count++] = texture->rt_view(desc.depth_stencil_target.slice, desc.depth_stencil_target.mip);
        }

        Ref<VKTexture> ref = desc.depth_stencil_target.texture ? desc.depth_stencil_target.texture : desc.color_targets[0].texture;
        m_size[0]          = ref->width();
        m_size[1]          = ref->height();

        create_info.attachmentCount = views_count;
        create_info.pAttachments    = views.data();
        create_info.width           = ref->width();
        create_info.height          = ref->height();
        create_info.layers          = 1;

        WG_VK_CHECK(vkCreateFramebuffer(m_driver.device(), &create_info, nullptr, &m_framebuffer));
        WG_VK_NAME(m_driver.device(), m_framebuffer, VK_OBJECT_TYPE_FRAMEBUFFER, m_name.str());
    }
    VKFramebufferObject::~VKFramebufferObject() {
        WG_AUTO_PROFILE_VULKAN("VKFramebufferObject::~VKFramebufferObject");

        if (m_framebuffer) {
            vkDestroyFramebuffer(m_driver.device(), m_framebuffer, nullptr);
        }
    }

    VKRenderPassBinder::VKRenderPassBinder(VKDriver& driver) : m_driver(driver) {
    }
    void VKRenderPassBinder::bind_target(const Ref<VKWindow>& window) {
        m_window = window;
        bind_color_target(m_window->color()[m_window->current()], 0, 0, 0);
        bind_depth_target(m_window->depth_stencil(), 0, 0);
    }
    void VKRenderPassBinder::bind_color_target(const Ref<VKTexture>& texture, int target, int mip, int slice) {
        m_current_pass_desc.color_target_fmts[target] = texture->format();
        VKTargetInfo& info                            = m_color_targets[target];
        info.texture                                  = texture;
        info.mip                                      = mip;
        info.slice                                    = slice;
    }
    void VKRenderPassBinder::bind_depth_target(const Ref<VKTexture>& texture, int mip, int slice) {
        m_current_pass_desc.depth_stencil_fmt = texture->format();
        m_depth_stencil_target.texture        = texture;
        m_depth_stencil_target.mip            = mip;
        m_depth_stencil_target.slice          = slice;
    }
    void VKRenderPassBinder::clear_color(int target) {
        m_current_pass_desc.color_target_ops[target] = GfxRtOp::ClearStore;
    }
    void VKRenderPassBinder::clear_depth() {
        m_current_pass_desc.depth_op = GfxRtOp::ClearStore;
    }
    void VKRenderPassBinder::clear_stencil() {
        m_current_pass_desc.stencil_op = GfxRtOp::ClearStore;
    }

    void VKRenderPassBinder::start(const Strid& name) {
        WG_AUTO_PROFILE_VULKAN("VKRenderPassBinder::start");

        m_current_name = name;
    }
    void VKRenderPassBinder::validate(VkCommandBuffer cmd) {
        WG_AUTO_PROFILE_VULKAN("VKRenderPassBinder::validate");

        prepare_render_pass();
        prepare_framebuffer();

        for (auto& target : m_color_targets) {
            if (target.texture) {
                target.texture->transition_layout(cmd, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
            }
        }

        if (m_depth_stencil_target.texture) {
            m_depth_stencil_target.texture->transition_layout(cmd, VKDefs::rt_layout_from_fmt(m_depth_stencil_target.texture->format()));
        }
    }
    void VKRenderPassBinder::finish(VkCommandBuffer cmd) {
        WG_AUTO_PROFILE_VULKAN("VKRenderPassBinder::finish");

        for (auto& target : m_color_targets) {
            if (target.texture) {
                target.texture->transition_layout(cmd, target.texture->primary_layout());
                target = VKTargetInfo();
            }
        }

        if (m_depth_stencil_target.texture) {
            m_depth_stencil_target.texture->transition_layout(cmd, m_depth_stencil_target.texture->primary_layout());
            m_depth_stencil_target = VKTargetInfo();
        }

        m_current_render_pass.reset();
        m_current_framebuffer.reset();
        m_current_size      = {0, 0};
        m_current_pass_desc = GfxRenderPassDesc{};
        m_current_name      = Strid();
    }

    const Ref<VKRenderPass>& VKRenderPassBinder::get_or_create_render_pass() {
        prepare_render_pass();

        return m_current_render_pass;
    }

    void VKRenderPassBinder::prepare_render_pass() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPassBinder::prepare_render_pass");

        m_current_render_pass = m_driver.make_render_pass(m_current_pass_desc, m_current_name).cast<VKRenderPass>();
    }
    void VKRenderPassBinder::prepare_framebuffer() {
        WG_AUTO_PROFILE_VULKAN("VKRenderPassBinder::prepare_framebuffer");

        m_current_fb_desc.color_targets        = m_color_targets;
        m_current_fb_desc.depth_stencil_target = m_depth_stencil_target;
        m_current_fb_desc.render_pass          = m_current_render_pass;

        m_current_framebuffer = m_driver.make_frame_buffer(m_current_fb_desc, m_current_name);
        m_current_size        = m_current_framebuffer->size();
    }

}// namespace wmoge
