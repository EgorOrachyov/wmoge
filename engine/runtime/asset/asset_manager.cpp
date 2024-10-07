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

#include "asset/asset_library_fs.hpp"
#include "core/timer.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler.hpp"
#include "rtti/type_storage.hpp"
#include "system/ioc_container.hpp"

#include <chrono>

namespace wmoge {

    AssetManager::AssetManager(IocContainer* ioc) {
        m_ioc_container = ioc;
        m_file_system   = ioc->resolve_value<FileSystem>();
        m_type_storage  = ioc->resolve_value<RttiTypeStorage>();

        m_callback = std::make_shared<typename Asset::ReleaseCallback>([this](Asset* asset) {
            std::lock_guard guard(m_mutex);

            auto entry = m_assets.find(asset->get_id());
            if (entry != m_assets.end()) {
                m_assets.erase(entry);
            }
        });
    }

    AsyncResult<Ref<Asset>> AssetManager::load_async(const AssetId& name) {
        WG_AUTO_PROFILE_ASSET("AssetManager::load_async");

        std::lock_guard guard(m_mutex);

        // already loaded and cached
        Ref<Asset> res = find(name);
        if (res) {
            std::optional<Ref<Asset>> res_opt(res);
            auto                      async_op = make_async_op<Ref<Asset>>();
            async_op->set_result(std::move(res));
            return AsyncResult<Ref<Asset>>(async_op);
        }

        // not yet cached, but is loading
        auto loading = m_loading.find(name);
        if (loading != m_loading.end()) {
            auto& async_op = loading->second.async_op;
            return AsyncResult<Ref<Asset>>(async_op);
        }

        // try to find asset library
        std::optional<AssetLibrary*> asset_library = resolve_asset(name);
        if (!asset_library) {
            WG_LOG_ERROR("failed to resolve asset library for " << name);
            return AsyncResult<Ref<Asset>>::failed();
        }

        // try to find resolve meta info
        AssetMeta asset_meta;
        if (!asset_library.value()->find_asset_meta(name, asset_meta)) {
            WG_LOG_ERROR("failed to find meta info for " << name);
            return AsyncResult<Ref<Asset>>::failed();
        }

        // try find loader
        std::optional<AssetLoader*> loader = find_loader(asset_meta.loader);
        if (!loader) {
            WG_LOG_ERROR("failed to find loader for " << name);
            return AsyncResult<Ref<Asset>>::failed();
        }

        // get dependencies which still loading or already loaded
        buffered_vector<Async> deps;
        for (const Strid& dep : asset_meta.deps) {
            deps.push_back(load_async(dep).as_async());
        }

        // create loading state to track result
        AsyncOp<Ref<Asset>> async_op = make_async_op<Ref<Asset>>();

        // create task to load
        Task task(name, [=, meta = std::move(asset_meta), library = asset_library.value(), loader = loader.value()](TaskContext&) {
            Timer timer;
            timer.start();

            Ref<Asset>             asset;
            AssetLoadContext       context;
            AssetLoadRequest       request;
            AssetLoadResult        result;
            std::vector<Ref<Data>> data_buffers;

            context.io_context.add<IocContainer*>(m_ioc_container);
            context.asset_meta = std::move(meta);

            if (!loader->fill_request(context, name, request)) {
                WG_LOG_ERROR("failed fill_request for " << name);
                return 1;
            }

            data_buffers.reserve(request.data_files.size());
            for (const auto& tag_path : request.data_files) {
                AssetDataMeta data_meta;

                if (!library->find_asset_data_meta(tag_path.second, data_meta)) {
                    WG_LOG_ERROR("failed to find meta for " << tag_path.second);
                    return 1;
                }

                Ref<Data>& buffer = data_buffers.emplace_back();
                buffer            = make_ref<Data>(data_meta.size);

                auto async_result = library->read_data(tag_path.second, {buffer->buffer(), buffer->size()});
                async_result.wait_completed();

                if (async_result.is_failed()) {
                    WG_LOG_ERROR("failed to read " << tag_path.second);
                    return 1;
                }

                result.add_data_file(tag_path.first, {buffer->buffer(), buffer->size()});
            }

            if (!loader->load(context, name, result, asset)) {
                WG_LOG_ERROR("failed load for " << name);
                return 1;
            }

            timer.stop();
            WG_LOG_INFO("load asset " << name << ", time: " << timer.get_elapsed_sec() << " sec");

            if (asset->get_name().empty()) {
                asset->set_id(name);
            }

            std::lock_guard guard(m_mutex);
            asset->set_release_callback(m_callback);
            m_assets[name] = WeakRef<Asset>(asset);
            async_op->set_result(std::move(asset));
            return 0;
        });

        // schedule to run only if all deps are loaded
        auto task_hnd = task.schedule(Async::join(array_view(deps)));

        // add erase of loading state here, since it is possible, that task can be aborted
        task_hnd.add_on_completion([this, name, async_op](AsyncStatus status, std::optional<int>&) {
            std::lock_guard guard(m_mutex);
            if (status == AsyncStatus::Failed) {
                async_op->set_failed();
            }
            m_loading.erase(name);
        });

        auto& state    = m_loading[name];
        state.deps     = std::move(deps);
        state.task_hnd = std::move(task_hnd);
        state.async_op = std::move(async_op);

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

    void AssetManager::add_loader(Ref<AssetLoader> loader) {
        std::lock_guard guard(m_mutex);
        m_loaders[loader->get_class_name()] = std::move(loader);
    }

    void AssetManager::add_library(std::shared_ptr<AssetLibrary> library) {
        m_libraries.push_back(std::move(library));
    }

    std::optional<AssetLoader*> AssetManager::find_loader(const Strid& loader_rtti) {
        std::lock_guard guard(m_mutex);
        auto            query = m_loaders.find(loader_rtti);
        return query != m_loaders.end() ? std::make_optional(query->second.get()) : std::nullopt;
    }

    std::optional<AssetMeta> AssetManager::resolve_asset_meta(const AssetId& asset) {
        AssetMeta asset_meta;
        for (auto& library : m_libraries) {
            if (library->find_asset_meta(asset, asset_meta)) {
                return std::make_optional(std::move(asset_meta));
            }
        }
        return std::nullopt;
    }

    std::optional<AssetLibrary*> AssetManager::resolve_asset(const AssetId& asset) {
        for (auto& library : m_libraries) {
            if (library->has_asset(asset)) {
                return std::make_optional(library.get());
            }
        }
        return std::nullopt;
    }

    void AssetManager::clear() {
        WG_AUTO_PROFILE_ASSET("AssetManager::clear");

        std::lock_guard guard(m_mutex);
        m_assets.clear();
    }

    void AssetManager::load_loaders() {
        WG_AUTO_PROFILE_ASSET("AssetManager::load_loaders");

        std::vector<RttiClass*> loaders = m_type_storage->find_classes([](const Ref<RttiClass>& type) {
            return type->is_subtype_of(AssetLoader::get_class_static()) && type->can_instantiate();
        });

        for (auto& loader : loaders) {
            assert(loader);
            assert(loader->can_instantiate());
            add_loader(loader->instantiate().cast<AssetLoader>());
        }
    }

}// namespace wmoge