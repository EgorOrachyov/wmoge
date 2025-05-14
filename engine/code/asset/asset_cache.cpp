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

#include "asset_cache.hpp"

namespace wmoge {

    WeakRef<Asset> AssetCache::try_find(UUID id) const {
        std::lock_guard guard(m_mutex);
        auto            query = m_assets.find(id);
        if (query != m_assets.end()) {
            return query->second;
        }
        return {};
    }

    Ref<Asset> AssetCache::try_acquire(UUID id) const {
        std::lock_guard guard(m_mutex);
        auto            query = m_assets.find(id);
        if (query != m_assets.end()) {
            return query->second.acquire();
        }
        return {};
    }

    bool AssetCache::has(UUID id) const {
        std::lock_guard guard(m_mutex);
        auto            query = m_assets.find(id);
        return query != m_assets.end();
    }

    void AssetCache::add(const Ref<Asset>& asset, bool replace) {
        std::lock_guard guard(m_mutex);
        auto            query = m_assets.find(asset->get_id());
        if (query != m_assets.end() && !replace) {
            return;
        }
        m_assets[asset->get_id()] = asset;
    }

    void AssetCache::remove(UUID id) {
        std::lock_guard guard(m_mutex);
        m_assets.erase(id);
    }

    void AssetCache::remove_if_expired(UUID id) {
        std::lock_guard guard(m_mutex);
        auto            query = m_assets.find(id);
        if (query == m_assets.end()) {
            return;
        }
        if (query->second.is_expired()) {
            m_assets.erase(id);
        }
    }

}// namespace wmoge