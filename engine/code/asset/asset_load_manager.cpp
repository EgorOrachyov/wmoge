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

#include "asset_load_manager.hpp"

#include "core/ioc_container.hpp"
#include "core/task.hpp"
#include "core/task_manager.hpp"
#include "core/timer.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    AssetLoadManager::AssetLoadManager(NameResolver  name_resolver,
                                       IoContext     io_context,
                                       FileSystem*   file_system,
                                       IocContainer* ioc_container,
                                       TaskManager*  task_manager)
        : m_name_resolver(std::move(name_resolver)),
          m_io_context(std::move(io_context)),
          m_file_system(file_system),
          m_ioc_container(ioc_container),
          m_task_manager(task_manager) {
    }

    AssetLoadManager::AsyncAssetRef AssetLoadManager::load(UUID asset_id, AssetLibrary* library, LoadedCallback callback, const DepsResolver& deps_resolver) {
        WG_PROFILE_CPU_ASSET("AssetLoadManager::load");

        std::lock_guard guard(m_mutex);

        auto asset_query = m_loading.find(asset_id);
        if (asset_query != m_loading.end()) {
            return AsyncResult<Ref<Asset>>(asset_query->second->async_op);
        }

        Ref<Entry> entry = make_ref<Entry>();

        if (!library->get_asset_info(asset_id, entry->asset_info)) {
            WG_LOG_ERROR("failed to get asset info from library for " << m_name_resolver(asset_id));
            return AsyncResult<Ref<Asset>>::failed();
        }

        auto loader_query = m_loaders.find(entry->asset_info.loader.get());
        if (loader_query == m_loaders.end()) {
            WG_LOG_ERROR("failed to find asset loader " << entry->asset_info.loader << " for " << m_name_resolver(asset_id));
            return AsyncResult<Ref<Asset>>::failed();
        }

        entry->library  = library;
        entry->loader   = loader_query->second;
        entry->async_op = make_async_op<Ref<Asset>>();
        entry->callback = std::move(callback);

        for (const UUID& dep_id : entry->asset_info.deps) {
            entry->deps.emplace_back(deps_resolver(dep_id).as_async());
        }

        Task start_loading(SIDDBG(m_name_resolver(asset_id)), [asset_id, entry, this](TaskContext&) -> Status {
            std::vector<Async> artifact_requests;

            for (const UUID& artifact_id : entry->asset_info.artifacts) {
                AssetArtifact artifact_info;
                if (!entry->library->get_artifact_info(artifact_id, artifact_info)) {
                    WG_LOG_ERROR("failed find artifact info " << artifact_id << " for " << m_name_resolver(asset_id));
                    return StatusCode::Error;
                }

                Ref<Data>& buffer = entry->buffers.emplace_back();
                buffer            = make_ref<Data>(artifact_info.size);

                Ref<RttiObject>& artifact = entry->artifacts.emplace_back();
                artifact                  = artifact_info.cls->instantiate();

                artifact_requests.push_back(entry->library->read_artifact(artifact_id, {buffer->buffer(), buffer->size()}, artifact));
            }

            Task load_asset(SIDDBG(m_name_resolver(asset_id)), [asset_id, entry, this](TaskContext&) -> Status {
                Timer timer;
                timer.start();

                Ref<Asset> asset;

                AssetLoadContext context(m_name_resolver(asset_id), m_io_context, m_ioc_container, entry->cls, entry->artifacts, entry->buffers);
                if (!entry->loader->load(context, asset_id, asset)) {
                    WG_LOG_ERROR("failed load asset " << m_name_resolver(asset_id));
                    return StatusCode::Error;
                }

                timer.stop();
                WG_LOG_INFO("load asset " << m_name_resolver(asset_id) << ", time: " << timer.get_elapsed_sec() << " sec");

                asset->set_id(asset_id);
                if (entry->callback) {
                    entry->callback(asset);
                }
                entry->async_op->set_result(std::move(asset));
                return WG_OK;
            });

            const Async depends_on = Async::join(artifact_requests);
            load_asset.schedule(m_task_manager, depends_on).add_on_completion([asset_id, entry, this](AsyncStatus status, std::optional<int>&) {
                if (status == AsyncStatus::Failed) {
                    entry->async_op->set_failed();
                }
                std::lock_guard guard(m_mutex);
                m_loading.erase(asset_id);
            });

            return WG_OK;
        });

        const Async depends_on = Async::join(entry->deps);
        start_loading.schedule(m_task_manager, depends_on).add_on_completion([entry, this](AsyncStatus status, std::optional<int>&) {
            if (status == AsyncStatus::Failed) {
                entry->async_op->set_failed();
            }
        });

        m_loading[asset_id] = entry;
        return AsyncResult<Ref<Asset>>(entry->async_op);
    }

    std::optional<AssetLoader*> AssetLoadManager::find_loader(RttiRefClass cls) {
        std::lock_guard guard(m_mutex);
        auto            query = m_loaders.find(cls.get());
        return query != m_loaders.end() ? std::optional<AssetLoader*>(query->second.get()) : std::optional<AssetLoader*>();
    }

    std::optional<AssetLibrary*> AssetLoadManager::find_library(UUID asset_id) {
        std::lock_guard guard(m_mutex);
        for (const Ref<AssetLibrary>& lib : m_libraries) {
            if (lib->has_asset(asset_id)) {
                return lib;
            }
        }
        return std::nullopt;
    }

    std::optional<AssetLibrary*> AssetLoadManager::find_library(const std::string& asset_name) {
        std::lock_guard guard(m_mutex);
        for (const Ref<AssetLibrary>& lib : m_libraries) {
            UUID asset_id;
            if (lib->resolve_asset(asset_name, asset_id)) {
                return lib;
            }
        }
        return std::nullopt;
    }

    void AssetLoadManager::add_loader(Ref<AssetLoader> loader) {
        std::lock_guard guard(m_mutex);
        m_loaders[loader->get_class()] = std::move(loader);
    }

    void AssetLoadManager::add_library(Ref<AssetLibrary> library) {
        std::lock_guard guard(m_mutex);
        m_libraries.push_back(std::move(library));
    }

}// namespace wmoge