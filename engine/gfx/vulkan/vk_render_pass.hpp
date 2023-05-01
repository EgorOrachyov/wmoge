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

#ifndef WMOGE_VK_RENDER_PASS_HPP
#define WMOGE_VK_RENDER_PASS_HPP

#include "core/fast_vector.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"
#include "gfx/vulkan/vk_texture.hpp"
#include "gfx/vulkan/vk_window.hpp"

#include <array>

namespace wmoge {

    /**
     * @class VKTargetInfo
     * @brief Info to bind texture as a render target
     */
    struct VKTargetInfo {
        ref_ptr<VKTexture> texture;
        int                slice = -1;
        int                mip   = -1;
    };

    /**
     * @class VKRenderPassHnd
     * @brief Raii wrapper for VkRenderPass object
     */
    class VKRenderPassHnd : public VKResource<GfxResource> {
    public:
        VKRenderPassHnd(VkRenderPass render_pass, int color_targets, bool has_depth_stencil, class VKDriver& driver);
        ~VKRenderPassHnd() override;

        VkRenderPass render_pass() const { return m_render_pass; }
        bool         has_depth_stencil() const { return m_has_depth_stencil; }
        int          color_targets_count() const { return m_color_targets_count; }

    private:
        VkRenderPass m_render_pass         = VK_NULL_HANDLE;
        bool         m_has_depth_stencil   = false;
        int          m_color_targets_count = 0;
    };

    /**
     * @class VKFramebufferHnd
     * @brief Raii wrapper for VkFramebuffer object
     */
    class VKFramebufferHnd : public VKResource<GfxResource> {
    public:
        VKFramebufferHnd(VkFramebuffer framebuffer, class VKDriver& driver);
        ~VKFramebufferHnd() override;

        VkFramebuffer framebuffer() const { return m_framebuffer; }

    private:
        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    };

    /**
     * @class VKRenderPass
     * @brief Vulkan single render pass implementation required for PSO and drawing
     */
    class VKRenderPass : public VKResource<GfxRenderPass> {
    public:
        VKRenderPass(GfxRenderPassType pass, const StringId& name, class VKDriver& driver);
        ~VKRenderPass() override;

        void bind_target(const ref_ptr<VKWindow>& window);
        void bind_color_target(const ref_ptr<VKTexture>& texture, int target, int mip, int slice);
        void bind_depth_target(const ref_ptr<VKTexture>& texture, int mip, int slice);
        void clear_color(int target);
        void clear_depth_stencil();
        void validate();

        ref_ptr<VKRenderPassHnd>  render_pass() const { return m_render_pass; }
        ref_ptr<VKFramebufferHnd> framebuffer() const { return m_window ? m_window_framebuffers[m_window->current()] : m_framebuffer; }
        int                       width() const { return m_size.width; }
        int                       height() const { return m_size.height; }
        int                       version() const { return m_version; }
        int                       color_targets_count() const { return m_render_pass ? m_render_pass->color_targets_count() : 0; }
        bool                      has_depth_stencil() const { return m_render_pass && m_render_pass->has_depth_stencil(); }

    private:
        void init_render_pass();
        void init_framebuffer();
        void release_render_pass();
        void release_framebuffer();

    private:
        std::array<VKTargetInfo, GfxLimits::MAX_COLOR_TARGETS>       m_color_targets{};
        std::array<VkFormat, GfxLimits::MAX_COLOR_TARGETS>           m_color_formats{};
        std::array<VkAttachmentLoadOp, GfxLimits::MAX_COLOR_TARGETS> m_color_op{};
        VKTargetInfo                                                 m_depth_stencil_target{};
        VkFormat                                                     m_depth_stencil_format{};
        VkAttachmentLoadOp                                           m_depth_stencil_op{};
        ref_ptr<VKWindow>                                            m_window;

        VkExtent2D                             m_size{};
        ref_ptr<VKRenderPassHnd>               m_render_pass;
        ref_ptr<VKFramebufferHnd>              m_framebuffer;
        fast_vector<ref_ptr<VKFramebufferHnd>> m_window_framebuffers;

        bool m_is_pass_dirty = true;
        bool m_is_fb_dirty   = true;
        int  m_version       = -1;
        int  m_fb_version    = -1;
    };

}// namespace wmoge

#endif//WMOGE_VK_RENDER_PASS_HPP
