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

#include "gfx/gfx_pipeline.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_render_pass.hpp"
#include "gfx/vulkan/vk_resource.hpp"
#include "gfx/vulkan/vk_shader.hpp"

namespace wmoge {

    /**
     * @class VKPsoLayout
     * @brief Vulkan pipeline layout implementation
     */
    class VKPsoLayout final : public VKResource<GfxPsoLayout> {
    public:
        VKPsoLayout(const GfxDescSetLayouts& layouts, const Strid& name, class VKDriver& driver);
        ~VKPsoLayout() override;

        VkPipelineLayout layout() const { return m_layout; }

    private:
        VkPipelineLayout m_layout = VK_NULL_HANDLE;
    };

    /**
     * @class VKPsoGraphics
     * @brief Vulkan graphics pipeline implementation
     */
    class VKPsoGraphics final : public VKResource<GfxPsoGraphics> {
    public:
        VKPsoGraphics(const Strid& name, class VKDriver& driver);
        ~VKPsoGraphics() override;

        Status compile(const GfxPsoStateGraphics& state);
        void   release();

        VkPipeline              pipeline() const { return m_pipeline; }
        const Ref<VKPsoLayout>& layout() const { return m_layout; }

    private:
        VkPipeline       m_pipeline = VK_NULL_HANDLE;
        Ref<VKPsoLayout> m_layout;
    };

    /**
     * @class VKPsoCompute
     * @brief  Vulkan compute pipeline implementation
    */
    class VKPsoCompute final : public VKResource<GfxPsoCompute> {
    public:
        VKPsoCompute(const Strid& name, class VKDriver& driver);
        ~VKPsoCompute() override;

        Status compile(const GfxPsoStateCompute& state);
        void   release();

        VkPipeline              pipeline() const { return m_pipeline; }
        const Ref<VKPsoLayout>& layout() const { return m_layout; }

    private:
        VkPipeline       m_pipeline = VK_NULL_HANDLE;
        Ref<VKPsoLayout> m_layout;
    };

}// namespace wmoge