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
#include "core/ioc_container.hpp"
#include "core/timer.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"
#include "rtti/type_storage.hpp"

#include <chrono>

namespace wmoge {

    AssetManager::AssetManager(IocContainer* ioc) {
        m_ioc_container = ioc;
        m_file_system   = ioc->resolve_value<FileSystem>();
        m_type_storage  = ioc->resolve_value<RttiTypeStorage>();
        m_task_manager  = ioc->resolve_value<TaskManager>();

        m_callback = std::make_shared<typename Asset::ReleaseCallback>([this](Asset* asset) {
            std::lock_guard guard(m_mutex);

            auto entry = m_assets.find(asset->get_id());
            if (entry != m_assets.end()) {
                m_assets.erase(entry);
            }
        });
    }

    AsyncResult<Ref<Asset>> AssetManager::load_async(const AssetId& name) {
        WG_PROFILE_CPU_ASSET("AssetManager::load_async");

        std::lock_guard guard(m_mutex);

        Ref<Asset> res = find(name);
        if (res) {
            std::optional<Ref<Asset>> res_opt(res);
            auto                      async_op = make_async_op<Ref<Asset>>();
            async_op->set_result(std::move(res));
            return AsyncResult<Ref<Asset>>(async_op);
        }

        auto loading = m_loading.find(name);
        if (loading != m_loading.end()) {
            auto& async_op = loading->second->async_op;
            return AsyncResult<Ref<Asset>>(async_op);
        }

        std::optional<AssetLibrary*> asset_library = resolve_asset(name);
        if (!asset_library) {
            WG_LOG_ERROR("failed to resolve asset library for " << name);
            return AsyncResult<Ref<Asset>>::failed();
        }

        AssetMeta asset_meta;
        if (!asset_library.value()->find_asset_meta(name, asset_meta)) {
            WG_LOG_ERROR("failed to find meta info for " << name);
            return AsyncResult<Ref<Asset>>::failed();
        }

        std::optional<AssetLoader*> loader = find_loader(asset_meta.loader);
        if (!loader) {
            WG_LOG_ERROR("failed to find loader for " << name);
            return AsyncResult<Ref<Asset>>::failed();
        }

        buffered_vector<Async> deps;
        for (const Strid& dep : asset_meta.deps) {
            deps.push_back(load_async(dep).as_async());
        }

        Ref<LoadState> load_state      = make_ref<LoadState>();
        load_state->deps               = std::move(deps);
        load_state->async_op           = make_async_op<Ref<Asset>>();
        load_state->library            = asset_library.value();
        load_state->loader             = loader.value();
        load_state->context.ioc        = m_ioc_container;
        load_state->context.asset_meta = std::move(asset_meta);

        Task task_fill_requests(name, [=](TaskContext&) -> int {
            if (!load_state->loader->fill_request(load_state->context, name, load_state->request)) {
                WG_LOG_ERROR("failed to fill request for " << name);
                return 1;
            }

            buffered_vector<Async, 16> file_data_requests;

            file_data_requests.reserve(load_state->request.data_files.size());
            load_state->data_buffers.reserve(load_state->request.data_files.size());

            for (const auto& tag_path : load_state->request.data_files) {
                AssetDataMeta data_meta;
                if (!load_state->library->find_asset_data_meta(tag_path.second, data_meta)) {
                    WG_LOG_ERROR("failed to find meta for " << tag_path.second);
                    return 1;
                }

                Ref<Data>& buffer = load_state->data_buffers.emplace_back();
                buffer            = make_ref<Data>(data_meta.size);

                auto async_result = load_state->library->read_data(tag_path.second, {buffer->buffer(), buffer->size()});
                file_data_requests.push_back(async_result);

                load_state->result.add_data_file(tag_path.first, {buffer->buffer(), buffer->size()});
            }

            Task task_load(name, [=](TaskContext&) -> int {
                Timer timer;
                timer.start();

                Ref<Asset> asset;
                if (!load_state->loader->load(load_state->context, name, load_state->result, asset)) {
                    WG_LOG_ERROR("failed load for " << name);
                    return 1;
                }

                timer.stop();
                WG_LOG_INFO("load asset " << name << ", time: " << timer.get_elapsed_sec() << " sec");

                std::lock_guard guard(m_mutex);
                asset->set_id(name);
                asset->set_release_callback(m_callback);
                m_assets[name] = WeakRef<Asset>(asset);
                load_state->async_op->set_result(std::move(asset));

                return 0;
            });

            task_load.schedule(m_task_manager, Async::join(array_view(file_data_requests))).add_on_completion([=](AsyncStatus status, std::optional<int>&) {
                if (status == AsyncStatus::Failed) {
                    load_state->async_op->set_failed();
                }
                std::lock_guard guard(m_mutex);
                m_loading.erase(name);
            });

            return 0;
        });

        task_fill_requests.schedule(m_task_manager, Async::join(array_view(load_state->deps))).add_on_completion([=](AsyncStatus status, std::optional<int>&) {
            if (status == AsyncStatus::Failed) {
                load_state->async_op->set_failed();
            }
            std::lock_guard guard(m_mutex);
            m_loading.erase(name);
        });

        m_loading[name] = load_state;
        return AsyncResult<Ref<Asset>>(load_state->async_op);
    }

    Ref<Asset> AssetManager::load(const AssetId& name) {
        WG_PROFILE_CPU_ASSET("AssetManager::load");

        Ref<Asset> fast_look_up = find(name);
        if (fast_look_up) {
            return fast_look_up;
        }

        AsyncResult<Ref<Asset>> async = load_async(name);
        async.wait_completed();
        return async.is_ok() ? async.result() : nullptr;
    }

    Ref<Asset> AssetManager::find(const AssetId& name) {
        WG_PROFILE_CPU_ASSET("AssetManager::find");

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
        WG_PROFILE_CPU_ASSET("AssetManager::clear");

        std::lock_guard guard(m_mutex);
        m_assets.clear();
    }

    void AssetManager::load_loaders() {
        WG_PROFILE_CPU_ASSET("AssetManager::load_loaders");

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