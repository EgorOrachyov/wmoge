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

#include "shader_library.hpp"

#include "core/log.hpp"
#include "gfx/gfx_driver.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>

namespace wmoge {

    ShaderModuleMap::ShaderModuleMap() {
        m_driver = IocContainer::iresolve_v<GfxDriver>();
    }

    Ref<GfxShader> ShaderModuleMap::get_or_create_shader(GfxShaderModule module_type, const Sha256& bytecode_hash) {
        auto query = m_modules.find(bytecode_hash);
        if (query == m_modules.end()) {
            return Ref<GfxShader>();
        }

        ShaderModule& shader_module = query->second;

        if (shader_module.bytecode_hash != bytecode_hash) {
            WG_LOG_ERROR("Hash mismatched for cached module for " << shader_module.name);
            return Ref<GfxShader>();
        }

        if (shader_module.module_type != module_type) {
            WG_LOG_ERROR("Hash mismatched module type for " << shader_module.name);
            return Ref<GfxShader>();
        }

        if (!shader_module.shader) {
            GfxShaderDesc shader_desc{};
            shader_desc.bytecode    = shader_module.bytecode;
            shader_desc.module_type = shader_module.module_type;
            shader_desc.shader_hash = shader_module.bytecode_hash;
            shader_module.shader    = m_driver->make_shader(std::move(shader_desc), shader_module.name);
        }

        if (!shader_module.shader) {
            WG_LOG_ERROR("Failed to create shader module for " << shader_module.name);
            return Ref<GfxShader>();
        }

        return shader_module.shader;
    }

    std::optional<Ref<GfxShader>> ShaderModuleMap::find_shader(GfxShaderModule module_type, const Sha256& bytecode_hash) {
        auto query = m_modules.find(bytecode_hash);
        if (query == m_modules.end()) {
            return std::nullopt;
        }
        if (!query->second.shader) {
            return std::nullopt;
        }
        return query->second.shader;
    }

    void ShaderModuleMap::fit_module(const ShaderModule& module) {
        ShaderModule& new_entry = m_modules[module.bytecode_hash];
        new_entry               = module;
    }

    void ShaderModuleMap::dump_modules(std::vector<ShaderModule>& out_modules) {
        for (const auto& entry : m_modules) {
            out_modules.push_back(entry.second);
        }
    }

    Ref<GfxShader> ShaderLibrary::get_or_create_shader(GfxShaderPlatform platform, GfxShaderModule module_type, const Sha256& bytecode_hash) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::get_or_create_shader");

        auto query = find_shader(platform, module_type, bytecode_hash);
        if (query) {
            assert(query.value());
            return query.value();
        }

        std::unique_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_libraries[int(platform)].get_or_create_shader(module_type, bytecode_hash);
    }

    std::optional<Ref<GfxShader>> ShaderLibrary::find_shader(GfxShaderPlatform platform, GfxShaderModule module_type, const Sha256& bytecode_hash) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::find_shader");

        std::shared_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        return m_libraries[int(platform)].find_shader(module_type, bytecode_hash);
    }

    void ShaderLibrary::fit_module(GfxShaderPlatform platform, const ShaderModule& module) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::fit_module");

        std::unique_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        m_libraries[int(platform)].fit_module(module);
    }

    void ShaderLibrary::dump_modules(GfxShaderPlatform platform, std::vector<ShaderModule>& out_modules) {
        WG_AUTO_PROFILE_GRC("ShaderLibrary::dump_modules");

        std::shared_lock lock(m_mutex);

        assert(int(platform) < GfxLimits::NUM_PLATFORMS);
        m_libraries[int(platform)].dump_modules(out_modules);
    }

}// namespace wmoge