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

#include "core/data.hpp"
#include "core/flat_map.hpp"
#include "core/sha256.hpp"
#include "core/simple_id.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_shader.hpp"
#include "platform/file_system.hpp"

#include <array>
#include <memory>
#include <vector>

namespace wmoge {

    /** 
     * @class ShaderModule
     * @brief Compiled shader module info for a particular platform
     * 
     * @note Ref to GfxShader created on demand
     */
    struct ShaderModule {
        Ref<GfxShader>  shader;       // Gfx object (may be null if not requested yet)
        Ref<Data>       bytecode;     // Platform specific bytecode
        GfxShaderModule module_type;  // Type of shader
        Sha256          source_hash;  // Hash of source code (text begore compilation)
        Sha256          bytecode_hash;// Hash of bytecode for fast look-ups and load from binary
        Strid           name;         // Debug name of module
    };

    /** 
     * @class ShaderModuleMap
     * @brief Map of compiled shader modules for a particular platform
     */
    class ShaderModuleMap {
    public:
        ShaderModuleMap();

        Ref<GfxShader>                get_or_create_shader(GfxShaderModule module_type, const Sha256& bytecode_hash);
        std::optional<Ref<GfxShader>> find_shader(GfxShaderModule module_type, const Sha256& bytecode_hash);
        void                          fit_module(ShaderModule& module);
        void                          dump_modules(std::vector<ShaderModule>& out_modules);

    private:
        flat_map<Sha256, ShaderModule> m_modules;
        GfxDriver*                     m_driver;
    };

    /** 
     * @class ShaderLibrary
     * @brief Library of compiled shader modules
     * 
     * Shader library manages bytecode for native gfx shader modules for each platfrom, which required for
     * actual gfx shader programs construction. Shader library caches new compiled modules, allows
     * to create gfx shaders required, and allows to store and load cache from file system on demand.
     * 
     * In actual game no shader compiler is allowed at runtime. Thus, all gfx shaders creation will
     * be handled using cached bytecode of modules in a shader library for a specific platform.
     * 
     * @note Thread-safe 
    */
    class ShaderLibrary {
    public:
        ShaderLibrary();

        Ref<GfxShader>                get_or_create_shader(GfxShaderPlatform platform, GfxShaderModule module_type, const Sha256& bytecode_hash);
        std::optional<Ref<GfxShader>> find_shader(GfxShaderPlatform platform, GfxShaderModule module_type, const Sha256& bytecode_hash);
        void                          fit_module(GfxShaderPlatform platform, ShaderModule& module);
        void                          dump_modules(GfxShaderPlatform platform, std::vector<ShaderModule>& out_modules);
        std::string                   make_cache_file_name(const std::string& folder, const GfxShaderPlatform platform);
        Status                        load_cache(const std::string& folder, const GfxShaderPlatform platform);
        Status                        save_cache(const std::string& folder, const GfxShaderPlatform platform);

    private:
        std::array<ShaderModuleMap, GfxLimits::NUM_PLATFORMS> m_libraries;

        mutable RwMutexReadPrefer m_mutex;
    };

}// namespace wmoge