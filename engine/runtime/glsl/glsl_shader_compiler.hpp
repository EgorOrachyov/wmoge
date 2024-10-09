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
#include "core/data.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_defs.hpp"
#include "glsl/glsl_builder.hpp"
#include "grc/shader_compiler.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class GlslShaderCompiler
     * @brief Glslang based compiler for shaders
    */
    class GlslShaderCompiler {
    public:
        GlslShaderCompiler(class IocContainer* ioc);
        ~GlslShaderCompiler();

        Status compile(ShaderCompilerRequest& request);

    private:
        class FileSystem* m_file_system = nullptr;
    };

    /**
     * @class GlslShaderCompilerAdapter
     * @brief Interface implementation for ShaderCompiler
    */
    class GlslShaderCompilerAdapter : public ShaderCompiler {
    public:
        WG_RTTI_CLASS(GlslShaderCompilerAdapter, ShaderCompiler);

        GlslShaderCompilerAdapter(class IocContainer* ioc, GfxShaderPlatform platform);
        GlslShaderCompilerAdapter() = default;

        Async                              compile(const Ref<ShaderCompilerRequest>& request, const Async& depends_on) override;
        std::shared_ptr<ShaderCodeBuilder> make_builder() override;
        GfxShaderPlatform                  get_platform() override;
        GfxShaderLang                      get_lang() override;

    private:
        class GlslShaderCompiler* m_glsl_compiler = nullptr;
        class TaskManager*        m_task_manager  = nullptr;
        GfxShaderPlatform         m_platform      = GfxShaderPlatform::None;
    };

    WG_RTTI_CLASS_BEGIN(GlslShaderCompilerAdapter) {
    }
    WG_RTTI_END;

    /**
     * @class GlslShaderCompilerVulkanLinux
     * @brief Adapter for glsl compiler for GfxShaderPlatform::VulkanLinux
    */
    class GlslShaderCompilerVulkanLinux : public GlslShaderCompilerAdapter {
    public:
        WG_RTTI_CLASS(GlslShaderCompilerVulkanLinux, GlslShaderCompilerAdapter);

        GlslShaderCompilerVulkanLinux(class IocContainer* ioc) : GlslShaderCompilerAdapter(ioc, GfxShaderPlatform::VulkanLinux) {}
        GlslShaderCompilerVulkanLinux() = default;
    };

    WG_RTTI_CLASS_BEGIN(GlslShaderCompilerVulkanLinux) {
        WG_RTTI_META_DATA();
    }
    WG_RTTI_END;

    /**
     * @class GlslShaderCompilerVulkanWindows
     * @brief Adapter for glsl compiler for GfxShaderPlatform::VulkanWindows
    */
    class GlslShaderCompilerVulkanWindows : public GlslShaderCompilerAdapter {
    public:
        WG_RTTI_CLASS(GlslShaderCompilerVulkanWindows, GlslShaderCompilerAdapter);

        GlslShaderCompilerVulkanWindows(class IocContainer* ioc) : GlslShaderCompilerAdapter(ioc, GfxShaderPlatform::VulkanWindows) {}
        GlslShaderCompilerVulkanWindows() = default;
    };

    WG_RTTI_CLASS_BEGIN(GlslShaderCompilerVulkanWindows) {
        WG_RTTI_META_DATA();
    }
    WG_RTTI_END;

    /**
     * @class GlslShaderCompilerVulkanMacOS
     * @brief Adapter for glsl compiler for GfxShaderPlatform::VulkanMacOS
    */
    class GlslShaderCompilerVulkanMacOS : public GlslShaderCompilerAdapter {
    public:
        WG_RTTI_CLASS(GlslShaderCompilerVulkanMacOS, GlslShaderCompilerAdapter);

        GlslShaderCompilerVulkanMacOS(class IocContainer* ioc) : GlslShaderCompilerAdapter(ioc, GfxShaderPlatform::VulkanMacOS) {}
        GlslShaderCompilerVulkanMacOS() = default;
    };

    WG_RTTI_CLASS_BEGIN(GlslShaderCompilerVulkanMacOS) {
        WG_RTTI_META_DATA();
    }
    WG_RTTI_END;

}// namespace wmoge