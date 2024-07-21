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

#pragma once

#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_render_pass.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"
#include "gfx/vulkan/vk_texture.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class VKRenderPass
     * @brief Vulkan single render pass implementation required for PSO and drawing
    */
    class VKRenderPass : public VKResource<GfxRenderPass> {
    public:
        VKRenderPass(const GfxRenderPassDesc& desc, const Strid& name, class VKDriver& driver);
        ~VKRenderPass() override;

        const GfxRenderPassDesc& desc() const override { return m_desc; }

        [[nodiscard]] VkRenderPass render_pass() const { return m_render_pass; }
        [[nodiscard]] int          color_targets_count() const { return m_color_targets_count; }
        [[nodiscard]] bool         has_depth_stencil() const { return m_has_depth_stencil; }
        [[nodiscard]] Size2i       get_size() const { return m_size; }

    private:
        GfxRenderPassDesc m_desc{};
        VkRenderPass      m_render_pass = VK_NULL_HANDLE;
        Size2i            m_size{0, 0};
        bool              m_has_depth_stencil   = false;
        int               m_color_targets_count = 0;
    };

    /**
     * @class VKFrameBuffer
     * @brief Vulkan frame buffer object implementation
    */
    class VKFrameBuffer : public VKResource<GfxFrameBuffer> {
    public:
        VKFrameBuffer(const GfxFrameBufferDesc& desc, const Strid& name, class VKDriver& driver);
        ~VKFrameBuffer() override;

        const GfxFrameBufferDesc& desc() const override { return m_desc; }

        [[nodiscard]] Size2i        size() const { return m_size; }
        [[nodiscard]] VkFramebuffer framebuffer() const { return m_framebuffer; }

    private:
        Size2i             m_size;
        GfxFrameBufferDesc m_desc;
        VkFramebuffer      m_framebuffer = VK_NULL_HANDLE;
    };

}// namespace wmoge