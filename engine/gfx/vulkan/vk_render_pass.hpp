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
        Ref<VKTexture> texture;
        int            slice = -1;
        int            mip   = -1;
    };

    /**
     * @class VKRenderPass
     * @brief Vulkan single render pass implementation required for PSO and drawing
     */
    class VKRenderPass : public VKResource<GfxResource> {
    public:
        VKRenderPass(const GfxRenderPassDesc& pass_desc, const StringId& name, class VKDriver& driver);
        ~VKRenderPass() override;

        [[nodiscard]] const GfxRenderPassDesc& pass_desc() const { return m_pass_desc; }
        [[nodiscard]] VkRenderPass             render_pass() const { return m_render_pass; }
        [[nodiscard]] int                      color_targets_count() const { return m_color_targets_count; }
        [[nodiscard]] bool                     has_depth_stencil() const { return m_has_depth_stencil; }

    private:
        GfxRenderPassDesc m_pass_desc{};
        VkRenderPass      m_render_pass         = VK_NULL_HANDLE;
        bool              m_has_depth_stencil   = false;
        int               m_color_targets_count = 0;
    };

    /**
     * @class VKFramebufferObject
     * @brief Raii wrapper for VkFramebuffer object
     */
    class VKFramebufferObject : public VKResource<GfxResource> {
    public:
        VKFramebufferObject(VkFramebuffer framebuffer, class VKDriver& driver);
        ~VKFramebufferObject() override;

        VkFramebuffer framebuffer() const { return m_framebuffer; }

    private:
        VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    };

    /**
     * @class VKRenderPassBinder
     * @brief Binds color targets to prepare render pass and frame buffer
     */
    class VKRenderPassBinder {
    public:
        explicit VKRenderPassBinder(class VKDriver& driver);
        ~VKRenderPassBinder() = default;

        void bind_target(const Ref<VKWindow>& window);
        void bind_color_target(const Ref<VKTexture>& texture, int target, int mip, int slice);
        void bind_depth_target(const Ref<VKTexture>& texture, int mip, int slice);
        void clear_color(int target);
        void clear_depth();
        void clear_stencil();

        void start(const StringId& name);
        void validate();
        void finish();

        [[nodiscard]] const Ref<VKRenderPass>&        render_pass() const { return m_current_render_pass; }
        [[nodiscard]] const Ref<VKFramebufferObject>& framebuffer() const { return m_current_framebuffer; }
        [[nodiscard]] int                             width() const { return m_current_size[0]; }
        [[nodiscard]] int                             height() const { return m_current_size[1]; }

    private:
        void prepare_render_pass();
        void prepare_framebuffer();

    private:
        std::array<VKTargetInfo, GfxLimits::MAX_COLOR_TARGETS> m_color_targets{};
        VKTargetInfo                                           m_depth_stencil_target{};
        Ref<VKWindow>                                          m_window;

        GfxRenderPassDesc        m_current_pass_desc{};
        Ref<VKRenderPass>        m_current_render_pass{};
        Ref<VKFramebufferObject> m_current_framebuffer{};
        Size2i                   m_current_size{};
        StringId                 m_current_name{};

        class VKDriver& m_driver;
    };

}// namespace wmoge

#endif//WMOGE_VK_RENDER_PASS_HPP
