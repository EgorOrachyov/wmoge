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

#include "core/async.hpp"
#include "core/flat_map.hpp"
#include "core/sha256.hpp"
#include "core/simple_id.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_desc_set.hpp"
#include "gfx/gfx_pipeline.hpp"
#include "gfx/gfx_shader.hpp"
#include "gfx/gfx_vert_format.hpp"

#include <cinttypes>

namespace wmoge {

    /**
     * @brief Status of the pso
    */
    enum class PsoStatus {
        InCompilation,// Pso in async compilation progress, need to wait for result
        Compiled,     // Pso compiled and can be used
        Failed,       // Pso failed to compile, need to evict and try again (shaders hot-reload)
        None          // Pso not requested to compile yet
    };

    /**
     * @brief How to fetch pso
    */
    enum class PsoFetchMode {
        CreateBlocking,
        CreateAsync
    };

    /**
     * @brief Unique key to identify pso
    */
    using PsoKey = SimpleId<std::uint32_t>;

    /**
     * @class PsoCacheEntry
     * @brief Cached pso info
    */
    struct PsoCacheEntry {
        Ref<GfxPso> pso;                     // graphics or compute pso
        PsoKey      key;                     // unique key of pso
        PsoStatus   status = PsoStatus::None;// status
        Async       compilation_op;          // to track compilation
    };

    /**
     * @class PsoCache
     * @brief Cache for pipeline state object to automate and speed-up their creation
     * 
     * Pso cache handles all aspects of pipeline state objects creation and caching.
     * It manages required for pso objects, like vertex formats and decriptor layouts.
     * Cache tracks and caches all referenced pso. 
     * 
     * Caching is necessary, since pso compilation is a heavy task, which can cause 
     * freezes and glitches in the game, if too many psos will be created during gameplay.
     * 
     * Also cache provides ways to precache psos upfront using async compilation. It allows
     * to warmup cache up front and do all heavy work before any actual gameplay
     * 
     * @note Thread-safe
    */
    class PsoCache {
    public:
        PsoCache();

        Ref<GfxVertFormat>    get_or_create_vert_format(const GfxVertElements& elements, const Strid& name = Strid());
        Ref<GfxDescSetLayout> get_or_create_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name = Strid());
        Ref<GfxShaderProgram> get_or_create_program(const GfxShaderProgramDesc& desc, const Strid& name = Strid());
        Ref<GfxShaderProgram> get_or_create_program(const GfxShaderProgramHeader& header, const Strid& name = Strid());
        Ref<GfxPsoLayout>     get_or_create_pso_layout(const GfxDescSetLayouts& layouts, const Strid& name = Strid());
        Ref<GfxPsoGraphics>   get_or_create_pso(const GfxPsoStateGraphics& state, const Strid& name = Strid());
        Ref<GfxPsoCompute>    get_or_create_pso(const GfxPsoStateCompute& state, const Strid& name = Strid());

        std::optional<Ref<GfxVertFormat>>    find_vert_format(const GfxVertElements& elements);
        std::optional<Ref<GfxDescSetLayout>> find_desc_layout(const GfxDescSetLayoutDesc& desc);
        std::optional<Ref<GfxShaderProgram>> find_program(const GfxShaderProgramDesc& desc);
        std::optional<Ref<GfxPsoLayout>>     find_pso_layout(const GfxDescSetLayouts& layouts);
        std::optional<Ref<GfxPsoGraphics>>   find_pso(const GfxPsoStateGraphics& state);
        std::optional<Ref<GfxPsoCompute>>    find_pso(const GfxPsoStateCompute& state);

        Async precache_psos(const array_view<GfxPsoStateGraphics>& states, const array_view<Strid>& names, Async depends_on = Async());
        Async precache_psos(const array_view<GfxPsoStateCompute>& states, const array_view<Strid>& names, Async depends_on = Async());

    private:
        [[nodiscard]] PsoKey         get_next_key();
        [[nodiscard]] PsoCacheEntry& get_or_add_entry(PsoKey& key);
        [[nodiscard]] PsoCacheEntry& get_or_add_entry(const GfxPsoStateGraphics& state);
        [[nodiscard]] PsoCacheEntry& get_or_add_entry(const GfxPsoStateCompute& state);

    private:
        flat_map<GfxVertElements, Ref<GfxVertFormat>>         m_vert_formats;
        flat_map<GfxDescSetLayoutDesc, Ref<GfxDescSetLayout>> m_desc_layouts;
        flat_map<GfxShaderProgramDesc, Ref<GfxShaderProgram>> m_programs;
        flat_map<GfxDescSetLayouts, Ref<GfxPsoLayout>>        m_pso_layouts;
        flat_map<GfxPsoStateGraphics, PsoKey>                 m_pso_graphics;
        flat_map<GfxPsoStateCompute, PsoKey>                  m_pso_compute;
        std::vector<PsoCacheEntry>                            m_psos;

        mutable RwMutexReadPrefer m_mutex_vert_format;
        mutable RwMutexReadPrefer m_mutex_desc_layout;
        mutable RwMutexReadPrefer m_mutex_program;
        mutable RwMutexReadPrefer m_mutex_pso_layouts;
        mutable RwMutexReadPrefer m_mutex_pso;

        PsoKey m_next_key{0};

        class ShaderLibrary* m_shader_library = nullptr;
        class TaskManager*   m_task_manager   = nullptr;
        class GfxDriver*     m_gfx_driver     = nullptr;
    };

}// namespace wmoge