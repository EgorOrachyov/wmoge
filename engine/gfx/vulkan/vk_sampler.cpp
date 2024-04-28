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

#include "vk_sampler.hpp"

#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_driver.hpp"
#include "math/math_utils.hpp"

namespace wmoge {

    VKSampler::VKSampler(const GfxSamplerDesc& desc, const Strid& name, class VKDriver& driver) : VKResource<GfxSampler>(driver) {
        m_name = name;
        m_desc = desc;
    }

    void VKSampler::create() {
        VkSamplerCreateInfo info{};
        info.sType            = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.flags            = 0;
        info.pNext            = nullptr;
        info.addressModeU     = VKDefs::get_address_mode(m_desc.u);
        info.addressModeV     = VKDefs::get_address_mode(m_desc.v);
        info.addressModeW     = VKDefs::get_address_mode(m_desc.w);
        info.minFilter        = VKDefs::get_filter(m_desc.min_flt);
        info.magFilter        = VKDefs::get_filter(m_desc.mag_flt);
        info.mipmapMode       = VKDefs::get_mipmap_mode(m_desc.min_flt);
        info.minLod           = m_desc.min_lod;
        info.maxLod           = m_desc.max_lod;
        info.anisotropyEnable = m_desc.max_anisotropy > 0.0f;
        info.maxAnisotropy    = Math::min(m_desc.max_anisotropy, m_driver.device_caps().max_anisotropy);
        info.borderColor      = VKDefs::get_border_color(m_desc.brd_clr);
        info.compareEnable    = VK_FALSE;
        info.compareOp        = VK_COMPARE_OP_ALWAYS;

        WG_VK_CHECK(vkCreateSampler(m_driver.device(), &info, nullptr, &m_sampler));
        WG_VK_NAME(m_driver.device(), m_sampler, VK_OBJECT_TYPE_SAMPLER, m_name.str());
    }

    VKSampler::~VKSampler() {
        if (m_sampler) {
            vkDestroySampler(m_driver.device(), m_sampler, nullptr);
            m_sampler = VK_NULL_HANDLE;
        }
    }

}// namespace wmoge
