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

#include "vk_shader.hpp"

#include "gfx/vulkan/vk_driver.hpp"
#include "profiler/profiler.hpp"

namespace wmoge {

    VKShader::VKShader(const Strid& name, VKDriver& driver) : VKResource<GfxShader>(driver) {
        m_name = name;
    }

    VKShader::~VKShader() {
        WG_AUTO_PROFILE_VULKAN("VKShader::~VKShader");

        if (m_module) {
            vkDestroyShaderModule(m_driver.device(), m_module, nullptr);
        }
    }

    void VKShader::create(GfxShaderDesc desc) {
        WG_AUTO_PROFILE_VULKAN("VKShader::create");

        m_desc = std::move(m_desc);

        VkShaderModuleCreateInfo create_info{};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = static_cast<uint32_t>(m_desc.bytecode->size());
        create_info.pCode    = reinterpret_cast<std::uint32_t*>(m_desc.bytecode->buffer());

        WG_VK_CHECK(vkCreateShaderModule(m_driver.device(), &create_info, nullptr, &m_module));
        WG_VK_NAME(m_driver.device(), m_module, VK_OBJECT_TYPE_SHADER_MODULE, name().str());
    }

    VKShaderProgram::VKShaderProgram(const Strid& name, VKDriver& driver) : VKResource<GfxShaderProgram>(driver) {
        m_name = name;
    }

    void VKShaderProgram::create(GfxShaderProgramDesc desc) {
        WG_AUTO_PROFILE_VULKAN("VKShaderProgram::create");

        m_desc = std::move(desc);
    }

}// namespace wmoge