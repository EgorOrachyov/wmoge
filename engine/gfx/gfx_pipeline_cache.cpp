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

#include "gfx_pipeline_cache.hpp"

#include <mutex>

namespace wmoge {

    std::optional<Ref<GfxPsoLayout>> GfxPsoLayoutCache::get(const GfxDescSetLayouts& layouts) {
        std::lock_guard guard(m_mutex);
        auto            it = m_cache.find(layouts);
        if (it != m_cache.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void GfxPsoLayoutCache::add(const GfxDescSetLayouts& layouts, const Ref<GfxPsoLayout>& layout) {
        std::lock_guard guard(m_mutex);
        m_cache[layouts] = layout;
    }

    std::optional<Ref<GfxPsoGraphics>> GfxPsoGraphicsCache::get(const GfxPsoStateGraphics& state) {
        std::lock_guard guard(m_mutex);
        auto            it = m_cache.find(state);
        if (it != m_cache.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void GfxPsoGraphicsCache::add(const GfxPsoStateGraphics& state, const Ref<GfxPsoGraphics>& pipeline) {
        std::lock_guard guard(m_mutex);
        m_cache[state] = pipeline;
    }

    std::optional<Ref<GfxPsoCompute>> GfxPsoComputeCache::get(const GfxPsoStateCompute& state) {
        std::lock_guard guard(m_mutex);
        auto            it = m_cache.find(state);
        if (it != m_cache.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    void GfxPsoComputeCache::add(const GfxPsoStateCompute& state, const Ref<GfxPsoCompute>& pipeline) {
        std::lock_guard guard(m_mutex);
        m_cache[state] = pipeline;
    }

}// namespace wmoge