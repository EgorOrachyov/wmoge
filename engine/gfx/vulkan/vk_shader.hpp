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

#ifndef WMOGE_VK_SHADER_HPP
#define WMOGE_VK_SHADER_HPP

#include "core/fast_vector.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"

#include <SPIRV/GlslangToSpv.h>
#include <StandAlone/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include <atomic>
#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class VKShader
     * @brief Vulkan implementation of gfx shader program resource
     */
    class VKShader final : public VKResource<GfxShader> {
    public:
        VKShader(class VKDriver& driver);
        ~VKShader() override;

        void setup(std::string vertex, std::string fragment, const StringId& name);
        void setup(Ref<Data> byte_code, const StringId& name);
        void compile_from_source();
        void compile_from_byte_code();

        GfxShaderStatus                                                    status() const override;
        std::string                                                        message() const override;
        const GfxShaderReflection*                                         reflection() const override;
        Ref<Data>                                                          byte_code() const override;
        const fast_vector<VkShaderModule>&                                 modules() const { return m_modules; }
        const std::array<VkDescriptorSetLayout, GfxLimits::MAX_DESC_SETS>& set_layouts() const { return m_set_layouts; }
        VkPipelineLayout                                                   layout() const { return m_layout; }

    private:
        void reflect(glslang::TProgram& program);
        void gen_byte_code(const std::vector<uint32_t>& vertex, const std::vector<uint32_t>& fragment);
        void init(const std::vector<uint32_t>& vertex, const std::vector<uint32_t>& fragment);

    private:
        fast_vector<std::string>                                    m_sources;
        fast_vector<VkShaderModule>                                 m_modules;
        std::atomic<GfxShaderStatus>                                m_status{GfxShaderStatus::Compiling};
        std::string                                                 m_message;
        GfxShaderReflection                                         m_reflection;
        std::array<VkDescriptorSetLayout, GfxLimits::MAX_DESC_SETS> m_set_layouts{};
        Ref<Data>                                                   m_byte_code;
        VkPipelineLayout                                            m_layout = VK_NULL_HANDLE;
    };

}// namespace wmoge

#endif//WMOGE_VK_SHADER_HPP
