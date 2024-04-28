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

#include "core/flat_map.hpp"
#include "core/string_id.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_pipeline.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class GfxPsoLayoutCache
     * @brief Runtime cache for pipeline layouts
    */
    class GfxPsoLayoutCache final {
    public:
        GfxPsoLayoutCache()                         = default;
        GfxPsoLayoutCache(const GfxPsoLayoutCache&) = delete;
        GfxPsoLayoutCache(GfxPsoLayoutCache&&)      = delete;
        ~GfxPsoLayoutCache()                        = default;

        std::optional<Ref<GfxPsoLayout>> get(const GfxDescSetLayouts& layouts);
        void                             add(const GfxDescSetLayouts& layouts, const Ref<GfxPsoLayout>& layout);

    private:
        flat_map<GfxDescSetLayouts, Ref<GfxPsoLayout>> m_cache;
        SpinMutex                                      m_mutex;
    };

    /**
     * @class GfxPsoGraphicsCache
     * @brief Runtime cache of graphics pipelines
    */
    class GfxPsoGraphicsCache final {
    public:
        GfxPsoGraphicsCache()                           = default;
        GfxPsoGraphicsCache(const GfxPsoGraphicsCache&) = delete;
        GfxPsoGraphicsCache(GfxPsoGraphicsCache&&)      = delete;
        ~GfxPsoGraphicsCache()                          = default;

        std::optional<Ref<GfxPsoGraphics>> get(const GfxPsoStateGraphics& state);
        void                               add(const GfxPsoStateGraphics& state, const Ref<GfxPsoGraphics>& pipeline);

    private:
        flat_map<GfxPsoStateGraphics, Ref<GfxPsoGraphics>> m_cache;
        SpinMutex                                          m_mutex;
    };

    /**
     * @class GfxPsoComputeCache
     * @brief Runtime cache of compute pipelines
    */
    class GfxPsoComputeCache final {
    public:
        GfxPsoComputeCache()                          = default;
        GfxPsoComputeCache(const GfxPsoComputeCache&) = delete;
        GfxPsoComputeCache(GfxPsoComputeCache&&)      = delete;
        ~GfxPsoComputeCache()                         = default;

        std::optional<Ref<GfxPsoCompute>> get(const GfxPsoStateCompute& state);
        void                              add(const GfxPsoStateCompute& state, const Ref<GfxPsoCompute>& pipeline);

    private:
        flat_map<GfxPsoStateCompute, Ref<GfxPsoCompute>> m_cache;
        SpinMutex                                        m_mutex;
    };

}// namespace wmoge