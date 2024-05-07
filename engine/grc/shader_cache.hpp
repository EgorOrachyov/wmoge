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
#include "core/simple_id.hpp"
#include "core/synchronization.hpp"
#include "core/task_manager.hpp"
#include "gfx/gfx_driver.hpp"
#include "gfx/gfx_shader.hpp"
#include "grc/shader.hpp"
#include "grc/shader_manager.hpp"
#include "grc/shader_reflection.hpp"

#include <cinttypes>
#include <vector>

namespace wmoge {

    /**
     * @brief Status of the shader
    */
    enum class ShaderStatus {
        InCompilation,// Shader in async compilation progress, need to wait for result
        Compiled,     // Shader compiled and can be used
        Failed,       // Shader failed to compile, need to evict and try again (shaders hot-reload)
        None          // Shader not requested to compile yet
    };

    /** 
     * @class ShaderProgram
     * @brief Compiled shader program info 
     */
    struct ShaderProgram {
        buffered_vector<Sha256> modules;
        Ref<GfxShaderProgram>   program;
        ShaderPermutation       permutation;
        ShaderStatus            status;
    };

    /**
     * @class ShaderCacheMap
     * @brief Cache of created programs for a particular shader and particular platform
    */
    class ShaderCacheMap {
    public:
        ShaderCacheMap() = default;

        std::optional<ShaderProgram*> find_program(const ShaderPermutation& permutation) const;
        void                          fit_program(const ShaderProgram& program);
        void                          dump_programs(std::vector<ShaderProgram>& out_programs);

    private:
        flat_map<ShaderPermutation, ShaderProgram> m_programs;
    };

    /**
     * @class ShaderCache
     * @brief Runtime cache of compiled gfx shaders from high-level shader programs
     * 
     * @note Thread-safe
    */
    class ShaderCache {
    public:
        ShaderCache(Shader* shader);

        Ref<GfxShaderProgram>                get_or_create_program(GfxShaderPlatform platform, const ShaderPermutation& permutation);
        std::optional<Ref<GfxShaderProgram>> find_program(GfxShaderPlatform platform, const ShaderPermutation& permutation) const;

        Async precompile_programs(GfxShaderPlatform platform, const array_view<ShaderPermutation>& permutations);

    private:
        std::array<ShaderCacheMap, int(GfxShaderPlatform::Max)> m_maps;

        Shader*      m_shader       = nullptr;
        GfxDriver*   m_driver       = nullptr;
        TaskManager* m_task_manager = nullptr;

        mutable RwMutexReadPrefer m_mutex;
    };

}// namespace wmoge