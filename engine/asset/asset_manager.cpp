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

#include "asset_manager.hpp"

#include "core/timer.hpp"
#include "debug/profiler.hpp"
#include "event/event_asset.hpp"
#include "event/event_manager.hpp"
#include "system/engine.hpp"

#include "asset/paks/asset_pak_fs.hpp"

#include "asset/loaders/asset_loader_default.hpp"
#include "asset/loaders/asset_loader_texture.hpp"
#include "asset/loaders/asset_loader_wav.hpp"

#include <chrono>

namespace wmoge {

    AssetManager::AssetManager() {
        add_pak(std::make_shared<AssetPakFileSystem>());
        add_loader(std::make_shared<AssetLoaderDefault>());
        add_loader(std::make_shared<AssetLoaderTexture2d>());
        add_loader(std::make_shared<AssetLoaderTextureCube>());
        add_loader(std::make_shared<AssetLoaderWav>());
    }

    AsyncResult<Ref<Asset>> AssetManager::load_async(const AssetId& name, AssetCallback callback) {
        WG_AUTO_PROFILE_ASSET("AssetManager::load_async");

        std::lock_guard guard(m_mutex);

        // already loaded and cached
        Ref<Asset> res = find(name);
        if (res) {
            std::optional<Ref<Asset>> res_opt(res);
            auto                      async_op = make_async_op<Ref<Asset>>();
            async_op->set_result(std::move(res));
            async_op->add_on_completion(std::move(callback));
            return AsyncResult<Ref<Asset>>(async_op);
        }

        // not yet cached, but is loading
        auto loading = m_loading.find(name);
        if (loading != m_loading.end()) {
            auto& async_op = loading->second.async_op;
            async_op->add_on_completion(std::move(callback));
            return AsyncResult<Ref<Asset>>(async_op);
        }

        // try to find meta info, to load from pak
        std::optional<AssetMeta> asset_meta = find_meta(name);

        if (!asset_meta.has_value()) {
            // failed to load, return dummy async in error state
            auto async_op = make_async_op<Ref<Asset>>();
            async_op->set_failed();
            async_op->add_on_completion(std::move(callback));

            WG_LOG_ERROR("failed to find meta info for " << name);
            return AsyncResult<Ref<Asset>>(async_op);
        }

        // get dependencies which still loading or already loaded
        buffered_vector<Async> deps;

        for (const Strid& dep : asset_meta.value().deps) {
            deps.push_back(load_async(dep).as_async());
        }

        // create loading state to track result
        AsyncOp<Ref<Asset>> async_op = make_async_op<Ref<Asset>>();

        // create task to load
        Task task(name, [=, meta = std::move(asset_meta.value())](TaskContext&) {
            Timer timer;
            timer.start();

            Ref<Asset> asset;
            if (meta.loader->load(name, meta, asset)) {
                timer.stop();
                WG_LOG_INFO("load asset " << name << ", time: " << timer.get_elapsed_sec() << " sec");

                if (asset->get_name().empty()) {
                    asset->set_name(name);
                }

                auto event          = make_event<EventAsset>();
                event->asset_id     = name;
                event->asset_ref    = asset;
                event->notification = AssetNotification::Loaded;
                Engine::instance()->event_manager()->dispatch_deferred(event);

                std::lock_guard guard(m_mutex);
                m_assets[name] = WeakRef<Asset>(asset);
                async_op->set_result(std::move(asset));
                return 0;
            }
            return 1;
        });

        // schedule to run only if all deps are loaded
        auto task_hnd = task.schedule(Async::join(ArrayView(deps)));

        // add erase of loading state here, since it is possible, that task can be aborted
        task_hnd.add_on_completion([this, name, async_op](AsyncStatus status, std::optional<int>&) {
            std::lock_guard guard(m_mutex);

            if (status == AsyncStatus::Failed) {
                auto event          = make_event<EventAsset>();
                event->asset_id     = name;
                event->notification = AssetNotification::FailedLoad;
                Engine::instance()->event_manager()->dispatch_deferred(event);

                async_op->set_failed();
                WG_LOG_ERROR("failed load asset " << name);
            }

            m_loading.erase(name);
        });

        auto& state    = m_loading[name];
        state.deps     = std::move(deps);
        state.task_hnd = std::move(task_hnd);
        state.async_op = std::move(async_op);

        state.async_op->add_on_completion(std::move(callback));
        return AsyncResult<Ref<Asset>>(state.async_op);
    }

    Ref<Asset> AssetManager::load(const AssetId& name) {
        WG_AUTO_PROFILE_ASSET("AssetManager::load");

        Ref<Asset> fast_look_up = find(name);
        if (fast_look_up) {
            return fast_look_up;
        }

        AsyncResult<Ref<Asset>> async = load_async(name);
        async.wait_completed();
        return async.is_ok() ? async.result() : nullptr;
    }

    Ref<Asset> AssetManager::find(const AssetId& name) {
        WG_AUTO_PROFILE_ASSET("AssetManager::find");

        std::lock_guard guard(m_mutex);

        auto cached = m_assets.find(name);
        if (cached != m_assets.end()) {
            return cached->second.acquire();
        }

        return {};
    }

    void AssetManager::add_loader(std::shared_ptr<AssetLoader> loader) {
        std::lock_guard guard(m_mutex);
        m_loaders[loader->get_name()] = std::move(loader);
    }

    void AssetManager::add_pak(std::shared_ptr<AssetPak> pak) {
        std::lock_guard guard(m_mutex);
        m_paks.push_back(std::move(pak));
    }

    std::optional<AssetLoader*> AssetManager::find_loader(const Strid& loader) {
        std::lock_guard guard(m_mutex);
        auto            query = m_loaders.find(loader);
        return query != m_loaders.end() ? std::make_optional(query->second.get()) : std::nullopt;
    }

    std::optional<AssetMeta> AssetManager::find_meta(const AssetId& asset) {
        std::lock_guard guard(m_mutex);

        AssetMeta asset_meta;
        for (auto& pak : m_paks) {
            if (pak->get_meta(asset, asset_meta)) {
                if (asset_meta.cls && asset_meta.loader && asset_meta.pak) {
                    return std::make_optional(std::move(asset_meta));
                }
                if (!asset_meta.cls) {
                    WG_LOG_ERROR("no class found in runtime for " << asset << " in " << pak->get_name());
                }
                if (!asset_meta.loader) {
                    WG_LOG_ERROR("no loader found in runtime for " << asset << " in " << pak->get_name());
                }
                if (!asset_meta.pak) {
                    WG_LOG_ERROR("no pak found in runtime for " << asset << " in " << pak->get_name());
                }
            }
        }

        return std::nullopt;
    }

    void AssetManager::gc() {
        WG_AUTO_PROFILE_ASSET("AssetManager::gc");

        std::lock_guard guard(m_mutex);
        int             evicted = 0;
        for (auto iter = m_assets.begin(); iter != m_assets.end(); ++iter) {
            if (iter->second.acquire()->refs_count() == 1) {
                iter = m_assets.erase(iter);
                evicted += 1;
            }
        }
        WG_LOG_INFO("gc " << evicted << " unreferenced assets");
    }

    void AssetManager::clear() {
        WG_AUTO_PROFILE_ASSET("AssetManager::clear");

        std::lock_guard guard(m_mutex);
        m_assets.clear();
    }

}// namespace wmoge