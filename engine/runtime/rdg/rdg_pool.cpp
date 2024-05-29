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
#include "system/config.hpp"
#include "system/ioc_container.hpp"

#include <algorithm>

namespace wmoge {

    RDGPool::RDGPool() {
        Config* config = IocContainer::iresolve_v<Config>();

        m_driver           = IocContainer::iresolve_v<GfxDriver>();
        m_frames_before_gc = config->get_int_or_default(SID("rdg.pool.frames_before_gc"), 6);
    }

    void RDGPool::update() {
        std::size_t frame_number = m_driver->frame_number();

        for (auto it = m_texture_pool.begin(); it != m_texture_pool.end();) {
            if (it->last_frame_used + m_frames_before_gc < frame_number) {
                it = m_texture_pool.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = m_storage_buffer_pool.begin(); it != m_storage_buffer_pool.end();) {
            if (it->last_frame_used + m_frames_before_gc < frame_number) {
                it = m_storage_buffer_pool.erase(it);
            } else {
                ++it;
            }
        }
    }

    GfxTextureRef RDGPool::allocate_texture(const GfxTextureDesc& desc) {
        auto query = std::find_if(m_texture_pool.begin(), m_texture_pool.end(), [&](const PoolTexture& t) {
            return t.resource->refs_count() == 1 && t.resource->desc().is_compatible(desc);
        });

        if (query != m_texture_pool.end()) {
            query->last_frame_used = m_driver->frame_number();
            return query->resource;
        }

        PoolTexture& t    = m_texture_pool.emplace_back();
        t.resource        = m_driver->make_texture(desc, SID("pool-texture"));
        t.last_frame_used = m_driver->frame_number();

        return t.resource;
    }

    GfxStorageBufferRef RDGPool::allocate_storage_buffer(const GfxBufferDesc& desc) {
        auto query = std::find_if(m_storage_buffer_pool.begin(), m_storage_buffer_pool.end(), [&](const PoolStorageBuffer& b) {
            return b.resource->refs_count() == 1 && b.resource->desc().is_compatible(desc);
        });

        if (query != m_storage_buffer_pool.end()) {
            query->last_frame_used = m_driver->frame_number();
            return query->resource;
        }

        PoolStorageBuffer& t = m_storage_buffer_pool.emplace_back();
        t.resource           = m_driver->make_storage_buffer(desc.size, desc.usage, SID("pool-buffer"));
        t.last_frame_used    = m_driver->frame_number();

        return t.resource;
    }

}// namespace wmoge