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

#ifndef WMOGE_VK_VERT_FORMAT_HPP
#define WMOGE_VK_VERT_FORMAT_HPP

#include "gfx/gfx_vert_format.hpp"
#include "gfx/vulkan/vk_defs.hpp"

namespace wmoge {

    /**
     * @class VKVertFormat
     * @brief Vulkan vertex format description
     */
    class VKVertFormat final : public GfxVertFormat {
    public:
        VKVertFormat(const GfxVertElements& elements, const Strid& name);
        ~VKVertFormat() override = default;

        const GfxVertElements& elements() const override;

        int buffers_count() const { return m_buffers_count; }
        int attributes_count() const { return m_attributes_count; }

        const std::array<VkVertexInputBindingDescription, GfxLimits::MAX_VERT_ATTRIBUTES>&   buffers() const { return m_buffers; }
        const std::array<VkVertexInputAttributeDescription, GfxLimits::MAX_VERT_ATTRIBUTES>& attributes() const { return m_attributes; }

    private:
        GfxVertElements m_elements;

        int m_buffers_count;
        int m_attributes_count;

        std::array<VkVertexInputBindingDescription, GfxLimits::MAX_VERT_ATTRIBUTES>   m_buffers;
        std::array<VkVertexInputAttributeDescription, GfxLimits::MAX_VERT_ATTRIBUTES> m_attributes;
    };

}// namespace wmoge

#endif//WMOGE_VK_VERT_FORMAT_HPP
