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

#include "texture_pool.hpp"

#include <cassert>

namespace wmoge {

    TexturePool::TexturePool(GfxDriver& driver) : m_driver(driver) {
    }

    TexturePool::~TexturePool() {
        for (const PoolKeyVal& entry : m_pools) {
            assert(entry.second.n_items_used == 0);
        }
    }

    GfxTextureRef TexturePool::allocate(const GfxTextureDesc& desc, const Strid& name) {
        std::lock_guard guard(m_mutex);

        PoolList* pool = get_or_create_pool(desc);

        std::optional<std::size_t> item_slot;

        if (pool->n_items_used < pool->n_items) {
            for (std::size_t i = 0; i < pool->n_items; i++) {
                if (!pool->items_used[i]) {
                    item_slot = i;
                    break;
                }
            }
        }

        if (!item_slot) {
            item_slot = pool->n_items;

            GfxTextureRef new_texture = m_driver.make_texture(desc);
            pool->items.push_back(PoolItem{new_texture});
            pool->items_used.push_back(false);
            pool->n_items++;
        }

        pool->items_used[*item_slot] = true;
        pool->n_items_used++;

        return pool->items[*item_slot].texture;
    }

    void TexturePool::release(const GfxTextureRef& handle) {
        std::lock_guard guard(m_mutex);

        assert(handle);

        PoolList* pool = get_pool(handle->desc());

        for (std::size_t i = 0; i < pool->n_items; i++) {
            if (pool->items[i].texture == handle) {
                assert(pool->items_used[i]);
                assert(pool->n_items_used > 0);
                pool->items_used[i] = false;
                pool->n_items_used--;
                return;
            }
        }

        assert(false);
    }

    TexturePool::PoolList* TexturePool::get_or_create_pool(const GfxTextureDesc& desc) {
        PoolList* pool = nullptr;

        for (PoolKeyVal& entry : m_pools) {
            if (entry.first == desc) {
                pool = &entry.second;
            }
        }

        if (!pool) {
            m_pools.emplace_back(desc, PoolList());
            pool = &m_pools.back().second;
        }

        return pool;
    }

    TexturePool::PoolList* TexturePool::get_pool(const GfxTextureDesc& desc) {
        for (PoolKeyVal& entry : m_pools) {
            if (entry.first == desc) {
                return &entry.second;
            }
        }

        assert(false);
        return nullptr;
    }

}// namespace wmoge
