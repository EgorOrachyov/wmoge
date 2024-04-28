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

#include "core/buffered_vector.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"

#include <atomic>
#include <memory>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class VKShader
     * @brief Vulkan implementation of gfx shader
     */
    class VKShader final : public VKResource<GfxShader> {
    public:
        VKShader(const Strid& name, class VKDriver& driver);
        ~VKShader() override;

        void create(Ref<Data> bytecode, GfxShaderModule module);

        [[nodiscard]] VkShaderModule module() const { return m_module; }

    private:
        VkShaderModule m_module = VK_NULL_HANDLE;
    };

    /**
     * @class VKShaderProgram
     * @brief Vulkan implementation of gfx shader program
     */
    class VKShaderProgram final : public VKResource<GfxShaderProgram> {
    public:
        VKShaderProgram(const Strid& name, class VKDriver& driver);
        ~VKShaderProgram() override = default;

        void create(GfxShaderProgramDesc desc);
    };

}// namespace wmoge