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

#include "pso_cache.hpp"

#include "core/ioc_container.hpp"
#include "core/task.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/shader_compiler.hpp"
#include "grc/shader_library.hpp"
#include "profiler/profiler.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <mutex>
#include <shared_mutex>

namespace wmoge {

    PsoCache::PsoCache(IocContainer* ioc) {
        m_shader_library = ioc->resolve_value<ShaderLibrary>();
        m_task_manager   = ioc->resolve_value<ShaderTaskManager>();
        m_gfx_driver     = ioc->resolve_value<GfxDriver>();
    }

    Ref<GfxVertFormat> PsoCache::get_or_create_vert_format(const GfxVertElements& elements, const Strid& name) {
        auto fast_lookup = find_vert_format(elements);
        if (fast_lookup) {
            return fast_lookup.value();
        }

        std::unique_lock    lock(m_mutex_vert_format);
        Ref<GfxVertFormat>& entry = m_vert_formats[elements];
        if (!entry) {
            entry = m_gfx_driver->make_vert_format(elements, name);
        }
        return entry;
    }
    Ref<GfxDescSetLayout> PsoCache::get_or_create_desc_layout(const GfxDescSetLayoutDesc& desc, const Strid& name) {
        auto fast_lookup = find_desc_layout(desc);
        if (fast_lookup) {
            return fast_lookup.value();
        }

        std::unique_lock       lock(m_mutex_desc_layout);
        Ref<GfxDescSetLayout>& entry = m_desc_layouts[desc];
        if (!entry) {
            entry = m_gfx_driver->make_desc_layout(desc, name);
        }
        return entry;
    }
    Ref<GfxShaderProgram> PsoCache::get_or_create_program(const GfxShaderProgramDesc& desc, const Strid& name) {
        auto fast_lookup = find_program(desc);
        if (fast_lookup) {
            return fast_lookup.value();
        }

        std::unique_lock       lock(m_mutex_program);
        Ref<GfxShaderProgram>& entry = m_programs[desc];
        if (!entry) {
            entry = m_gfx_driver->make_program(desc, name);
        }
        return entry;
    }
    Ref<GfxShaderProgram> PsoCache::get_or_create_program(const GfxShaderProgramHeader& program_header, const Strid& name) {
        GfxShaderProgramDesc desc;
        GfxShaderPlatform    platform = m_gfx_driver->get_shader_platform();

        for (const GfxShaderHeader& shader_header : program_header) {
            Ref<GfxShader> shader = m_shader_library->get_or_create_shader(m_gfx_driver->get_shader_platform(), shader_header.module_type, shader_header.shader_hash);
            if (!shader) {
                WG_LOG_ERROR("failed to fetch shader from library for " << name);
                return Ref<GfxShaderProgram>();
            }
            desc.push_back(std::move(shader));
        }

        return get_or_create_program(desc, name);
    }
    Ref<GfxPsoLayout> PsoCache::get_or_create_pso_layout(const GfxDescSetLayouts& layouts, const Strid& name) {
        auto fast_lookup = find_pso_layout(layouts);
        if (fast_lookup) {
            return fast_lookup.value();
        }

        std::unique_lock   lock(m_mutex_pso_layouts);
        Ref<GfxPsoLayout>& entry = m_pso_layouts[layouts];
        if (!entry) {
            entry = m_gfx_driver->make_pso_layout(layouts, name);
        }
        return entry;
    }
    Ref<GfxPsoGraphics> PsoCache::get_or_create_pso(const GfxPsoStateGraphics& state, const Strid& name) {
        auto fast_lookup = find_pso(state);
        if (fast_lookup) {
            return fast_lookup.value();
        }

        std::unique_lock lock(m_mutex_pso);

        PsoCacheEntry& entry = get_or_add_entry(state);
        if (entry.status == PsoStatus::None) {
            entry.pso    = m_gfx_driver->make_pso_graphics(state, name);
            entry.status = entry.pso ? PsoStatus::Compiled : PsoStatus::Failed;
        }

        return entry.pso.cast<GfxPsoGraphics>();
    }
    Ref<GfxPsoCompute> PsoCache::get_or_create_pso(const GfxPsoStateCompute& state, const Strid& name) {
        auto fast_lookup = find_pso(state);
        if (fast_lookup) {
            return fast_lookup.value();
        }

        std::unique_lock lock(m_mutex_pso);

        PsoCacheEntry& entry = get_or_add_entry(state);
        if (entry.status == PsoStatus::None) {
            entry.pso    = m_gfx_driver->make_pso_compute(state, name);
            entry.status = entry.pso ? PsoStatus::Compiled : PsoStatus::Failed;
        }

        return entry.pso.cast<GfxPsoCompute>();
    }

    std::optional<Ref<GfxVertFormat>> PsoCache::find_vert_format(const GfxVertElements& elements) {
        std::shared_lock lock(m_mutex_vert_format);
        auto             query = m_vert_formats.find(elements);
        return query != m_vert_formats.end() ? std::make_optional(query->second) : std::nullopt;
    }
    std::optional<Ref<GfxDescSetLayout>> PsoCache::find_desc_layout(const GfxDescSetLayoutDesc& desc) {
        std::shared_lock lock(m_mutex_desc_layout);
        auto             query = m_desc_layouts.find(desc);
        return query != m_desc_layouts.end() ? std::make_optional(query->second) : std::nullopt;
    }
    std::optional<Ref<GfxShaderProgram>> PsoCache::find_program(const GfxShaderProgramDesc& desc) {
        std::shared_lock lock(m_mutex_program);
        auto             query = m_programs.find(desc);
        return query != m_programs.end() ? std::make_optional(query->second) : std::nullopt;
    }
    std::optional<Ref<GfxPsoLayout>> PsoCache::find_pso_layout(const GfxDescSetLayouts& layouts) {
        std::shared_lock lock(m_mutex_pso_layouts);
        auto             query = m_pso_layouts.find(layouts);
        return query != m_pso_layouts.end() ? std::make_optional(query->second) : std::nullopt;
    }
    std::optional<Ref<GfxPsoGraphics>> PsoCache::find_pso(const GfxPsoStateGraphics& state) {
        std::shared_lock lock(m_mutex_pso);
        auto             query = m_pso_graphics.find(state);
        if (query == m_pso_graphics.end()) {
            return std::nullopt;
        }
        if (!m_psos[query->second.value].pso) {
            return std::nullopt;
        }
        return m_psos[query->second.value].pso.cast<GfxPsoGraphics>();
    }
    std::optional<Ref<GfxPsoCompute>> PsoCache::find_pso(const GfxPsoStateCompute& state) {
        std::shared_lock lock(m_mutex_pso);
        auto             query = m_pso_compute.find(state);
        if (query == m_pso_compute.end()) {
            return std::nullopt;
        }
        if (!m_psos[query->second.value].pso) {
            return std::nullopt;
        }
        return m_psos[query->second.value].pso.cast<GfxPsoCompute>();
    }

    Async PsoCache::precache_psos(const array_view<GfxPsoStateGraphics>& states, const array_view<Strid>& names, Async depends_on) {
        WG_AUTO_PROFILE_GRC("PsoCache::precache_psos");

        assert(states.size() == names.size());

        if (states.empty()) {
            return Async::completed();
        }

        std::unique_lock lock(m_mutex_pso);

        Ref<GfxAsyncPsoRequestGraphics> request = make_ref<GfxAsyncPsoRequestGraphics>();
        request->states.reserve(states.size());
        request->names.reserve(names.size());

        for (std::size_t i = 0; i < states.size(); i++) {
            const GfxPsoStateGraphics& state = states[i];

            PsoCacheEntry& cached_state = get_or_add_entry(state);
            if (cached_state.status == PsoStatus::InCompilation ||
                cached_state.status == PsoStatus::Failed) {
                continue;
            }

            request->states.push_back(state);
            request->names.push_back(names[i]);
        }

        Async result = m_gfx_driver->make_psos_graphics(request);

        Task task(SID("pso_store_cache"), [r = request, this](TaskContext&) {
            std::unique_lock lock(m_mutex_pso);

            for (std::size_t i = 0; i < r->states.size(); i++) {
                const GfxPsoStateGraphics& state = r->states[i];

                PsoKey& key = m_pso_graphics[state];
                assert(key);

                PsoCacheEntry& cached_state = m_psos[key];
                cached_state.pso            = r->pso[i];
                cached_state.status         = r->pso[i] ? PsoStatus::Compiled : PsoStatus::Failed;
                cached_state.compilation_op.reset();
            }

            return 0;
        });

        return task.schedule(m_task_manager, depends_on).as_async();
    }
    Async PsoCache::precache_psos(const array_view<GfxPsoStateCompute>& states, const array_view<Strid>& names, Async depends_on) {
        WG_AUTO_PROFILE_GRC("PsoCache::precache_psos");

        assert(states.size() == names.size());

        if (states.empty()) {
            return Async::completed();
        }

        std::unique_lock lock(m_mutex_pso);

        Ref<GfxAsyncPsoRequestCompute> request = make_ref<GfxAsyncPsoRequestCompute>();
        request->states.reserve(states.size());
        request->names.reserve(names.size());

        for (std::size_t i = 0; i < states.size(); i++) {
            const GfxPsoStateCompute& state = states[i];

            PsoCacheEntry& cached_state = get_or_add_entry(state);
            if (cached_state.status == PsoStatus::InCompilation ||
                cached_state.status == PsoStatus::Failed) {
                continue;
            }

            cached_state.status = PsoStatus::InCompilation;

            request->states.push_back(state);
            request->names.push_back(names[i]);
        }

        Async result = m_gfx_driver->make_psos_compute(request);

        Task task(SID("pso_store_cache"), [r = request, this](TaskContext&) {
            std::unique_lock lock(m_mutex_pso);

            for (std::size_t i = 0; i < r->states.size(); i++) {
                const GfxPsoStateCompute& state = r->states[i];

                PsoCacheEntry& cached_state = get_or_add_entry(state);
                cached_state.pso            = r->pso[i];
                cached_state.status         = r->pso[i] ? PsoStatus::Compiled : PsoStatus::Failed;
            }

            return 0;
        });

        return task.schedule(m_task_manager, depends_on).as_async();
    }

    PsoKey PsoCache::get_next_key() {
        PsoKey key = m_next_key;
        m_next_key.value++;
        return key;
    }
    PsoCacheEntry& PsoCache::get_or_add_entry(PsoKey& key) {
        if (!key) {
            key = get_next_key();
            m_psos.resize(key.value + 1);
            m_psos[key].key = key;
        }
        return m_psos[key];
    }
    PsoCacheEntry& PsoCache::get_or_add_entry(const GfxPsoStateGraphics& state) {
        PsoKey& key = m_pso_graphics[state];
        return get_or_add_entry(key);
    }
    PsoCacheEntry& PsoCache::get_or_add_entry(const GfxPsoStateCompute& state) {
        PsoKey& key = m_pso_compute[state];
        return get_or_add_entry(key);
    }

}// namespace wmoge