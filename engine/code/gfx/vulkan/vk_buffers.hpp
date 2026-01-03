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

#include "core/array_view.hpp"
#include "core/data.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"

namespace wmoge {

    /**
     * @class VKBuffer
     * @brief Vulkan implementation for gfx buffer class
     */
    class VKBuffer final : public VKResource<GfxBuffer> {
    public:
        VKBuffer(const GfxBufferDesc& desc, const Strid& name, class VKDriver& driver);
        ~VKBuffer() override;

        void update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, array_view<const std::uint8_t> data);

        [[nodiscard]] VkBuffer      buffer() const { return m_buffer; }
        [[nodiscard]] VmaAllocation allocation() const { return m_allocation; }

    protected:
        VkBuffer      m_buffer     = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
    };

}// namespace wmoge