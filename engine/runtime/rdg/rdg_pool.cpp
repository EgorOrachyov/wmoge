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

#include "rdg_pool.hpp"

#include "gfx/gfx_driver.hpp"

#include <algorithm>

namespace wmoge {

    template<typename Res, typename Desc, typename Entry, typename Factory>
    Res& rdg_allocate_resource(std::vector<Entry>& pool, const Desc& desc, GfxDriver* driver, Factory&& factory) {
        Entry* entry = nullptr;

        auto query = std::find_if(pool.begin(), pool.end(), [&](const Entry& e) {
            return !e.is_allocated && e.resource->desc().is_compatible(desc);
        });

        if (query != pool.end()) {
            entry = &(*query);
        } else {
            entry = &pool.emplace_back();
        }

        if (!entry->resource) {
            entry->resource = factory(desc);
        }

        entry->last_frame_used = driver->frame_number();
        entry->is_allocated    = true;

        return entry->resource;
    }

    template<typename Res, typename Entry>
    void rdg_release_resource(const Res& res, std::vector<Entry>& pool) {
        auto query = std::find_if(pool.begin(), pool.end(), [&](const Entry& e) {
            return e.resource == res;
        });

        assert(query != pool.end());
        assert(query->is_allocated);

        query->is_allocated = false;
    }

    template<typename Entry>
    void rdg_gc_resources(std::vector<Entry>& pool, std::size_t frame_number, std::size_t frames_before_gc) {
        for (auto it = pool.begin(); it != pool.end();) {
            if (it->last_frame_used + frames_before_gc < frame_number) {
                it = pool.erase(it);
            } else {
                ++it;
            }
        }
    }

    RdgPool::RdgPool(GfxDriver* driver) {
        m_driver = driver;
    }

    void RdgPool::gc() {
        std::size_t frame_number = m_driver->frame_number();

        rdg_gc_resources(m_texture_pool, frame_number, m_frames_before_gc);
        rdg_gc_resources(m_uniform_buffer_pool, frame_number, m_frames_before_gc);
        rdg_gc_resources(m_storage_buffer_pool, frame_number, m_frames_before_gc);
    }

    void RdgPool::set_frames_before_gc(int frames) {
        m_frames_before_gc = frames;
    }

    GfxTextureRef RdgPool::allocate_texture(const GfxTextureDesc& desc) {
        return rdg_allocate_resource<GfxTextureRef>(m_texture_pool, desc, m_driver, [&, this](const GfxTextureDesc& desc) {
            return m_driver->make_texture(desc, SID("pool_texture"));
        });
    }

    void RdgPool::release_texture(const GfxTextureRef& texture) {
        rdg_release_resource(texture, m_texture_pool);
    }

    GfxUniformBufferRef RdgPool::allocate_uniform_buffer(const GfxBufferDesc& desc) {
        return rdg_allocate_resource<GfxUniformBufferRef>(m_uniform_buffer_pool, desc, m_driver, [&, this](const GfxBufferDesc& desc) {
            return m_driver->make_uniform_buffer(desc.size, desc.usage, SID("pool_buffer"));
        });
    }

    void RdgPool::release_uniform_buffer(const GfxUniformBufferRef& buffer) {
        rdg_release_resource(buffer, m_uniform_buffer_pool);
    }

    GfxStorageBufferRef RdgPool::allocate_storage_buffer(const GfxBufferDesc& desc) {
        return rdg_allocate_resource<GfxStorageBufferRef>(m_storage_buffer_pool, desc, m_driver, [&, this](const GfxBufferDesc& desc) {
            return m_driver->make_storage_buffer(desc.size, desc.usage, SID("pool_buffer"));
        });
    }

    void RdgPool::release_storage_buffer(const GfxStorageBufferRef& buffer) {
        rdg_release_resource(buffer, m_storage_buffer_pool);
    }

}// namespace wmoge