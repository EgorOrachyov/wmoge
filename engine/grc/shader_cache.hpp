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
#include "core/async.hpp"
#include "core/data.hpp"
#include "core/flat_map.hpp"
#include "core/sha256.hpp"
#include "core/task_manager.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_shader.hpp"
#include "grc/shader_reflection.hpp"

#include <array>
#include <cinttypes>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @brief Status of the shader
    */
    enum class ShaderStatus {
        InCompilation,// Shader in async compilation progress, need to wait for result
        InBytecode,   // Shader in bytecode, need to try to create program
        Compiled,     // Shader compiled and can be used
        Failed,       // Shader failed to compile, need to evict and try again (shaders hot-reload)
        None          // Shader not requested to compile yet, and has no cache
    };

    /** 
     * @class ShaderProgram
     * @brief Compiled shader program info 
     */
    struct ShaderProgram {
        buffered_vector<Sha256> modules;                    // Bytecode hashes of modules (for fast load from bytecode cache)
        Ref<GfxShaderProgram>   program;                    // Gfx object (may be null if failed compile)
        ShaderPermutation       permutation;                // Program unique key
        ShaderStatus            status = ShaderStatus::None;// Current program status
        Strid                   name;                       // Name for saving, recreation
        Async                   compilation_task;           // Optional pending compilation task to compile program
    };

    /**
     * @class ShaderCacheMap
     * @brief Cache of created programs for a particular shader and particular platform
    */
    class ShaderCacheMap {
    public:
        ShaderCacheMap() = default;

        std::optional<ShaderProgram*> find_program(const ShaderPermutation& permutation);
        ShaderProgram&                get_or_add_entry(const ShaderPermutation& permutation);
        void                          fit_program(const ShaderProgram& program);
        void                          dump_programs(std::vector<ShaderProgram>& out_programs);

    private:
        flat_map<ShaderPermutation, ShaderProgram> m_programs;
    };

    /**
     * @class ShaderCache
     * @brief Runtime cache of compiled gfx shaders from high-level shader programs
     * 
     * Shader cache allows to get particular program variations for a given platform.
     * Already compiled programs are cached, can be saved and loaded to/from disk. 
    */
    class ShaderCache {
    public:
        ShaderCache();

        std::optional<ShaderProgram*> find_program(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        ShaderProgram&                get_or_add_entry(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        void                          fit_program(GfxShaderPlatform platform, const ShaderProgram& program);
        void                          dump_programs(GfxShaderPlatform platform, std::vector<ShaderProgram>& out_programs);
        std::string                   make_cache_file_name(const std::string& folder, const std::string& prefix, GfxShaderPlatform platform);
        Status                        load_cache(const std::string& file_path, GfxShaderPlatform platform, bool allow_missing = true);
        Status                        save_cache(const std::string& file_path, GfxShaderPlatform platform);

    private:
        std::array<ShaderCacheMap, GfxLimits::NUM_PLATFORMS> m_maps;
    };

}// namespace wmoge