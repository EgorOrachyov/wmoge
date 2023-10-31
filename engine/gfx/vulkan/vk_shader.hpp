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
#include "gfx/gfx_desc_set.hpp"
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
     * @class VKShaderBinary
     * @brief Struct used for serialization of vulkan shader into binary format
     */
    struct VKShaderBinary {
        fast_vector<Ref<Data>> spirvs;
        GfxDescSetLayoutDescs  layouts;
        GfxShaderReflection    reflection;

        friend Status archive_read(Archive& archive, VKShaderBinary& binary);
        friend Status archive_write(Archive& archive, const VKShaderBinary& binary);
    };

    /**
     * @class VKShader
     * @brief Vulkan implementation of gfx shader program resource
     */
    class VKShader final : public VKResource<GfxShader> {
    public:
        VKShader(std::string vertex, std::string fragment, const GfxDescSetLayouts& layouts, const StringId& name, class VKDriver& driver);
        VKShader(std::string compute, const GfxDescSetLayouts& layouts, const StringId& name, class VKDriver& driver);
        VKShader(Ref<Data> byte_code, const StringId& name, class VKDriver& driver);
        ~VKShader() override;

        void compile_from_source();
        void compile_from_byte_code();

        [[nodiscard]] GfxShaderStatus                           status() const override;
        [[nodiscard]] std::string                               message() const override;
        [[nodiscard]] std::optional<const GfxShaderReflection*> reflection() const override;
        [[nodiscard]] Ref<Data>                                 byte_code() const override;
        [[nodiscard]] const fast_vector<VkShaderModule>&        modules() const { return m_modules; }
        [[nodiscard]] VkPipelineLayout                          layout() const { return m_layout; }

    private:
        void reflect(glslang::TProgram& program);
        void gen_byte_code(const fast_vector<Ref<Data>>& spirvs);
        void init(const fast_vector<Ref<Data>>& spirvs);

    private:
        fast_vector<std::string>     m_sources;
        fast_vector<VkShaderModule>  m_modules;
        std::atomic<GfxShaderStatus> m_status{GfxShaderStatus::Compiling};
        std::string                  m_message;
        GfxShaderReflection          m_reflection;
        GfxDescSetLayouts            m_set_layouts{};
        Ref<Data>                    m_byte_code;
        VkPipelineLayout             m_layout = VK_NULL_HANDLE;
    };

}// namespace wmoge

#endif//WMOGE_VK_SHADER_HPP
