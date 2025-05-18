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

#include "asset/asset_cache.hpp"
#include "asset/asset_db.hpp"
#include "asset/asset_import_manager.hpp"
#include "asset/asset_load_manager.hpp"
#include "core/task_manager.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    AssetManager::AssetManager(FileSystem*         file_system,
                               AssetDb*            asset_db,
                               AssetCache*         asset_cache,
                               AssetLoadManager*   load_manager,
                               AssetImportManager* import_manager)
        : m_file_system(file_system),
          m_asset_db(asset_db),
          m_asset_cache(asset_cache),
          m_load_manager(load_manager),
          m_import_manager(import_manager) {
        m_callback = std::make_shared<EvictCallback>([asset_cache](Asset* asset) {
            asset_cache->remove_if_expired(asset->get_id());
        });
    }

    AssetManager::AsyncAssetRef AssetManager::load(UUID asset_id) {
        WG_PROFILE_CPU_ASSET("AssetManager::load");

        std::lock_guard guard(m_mutex);

        Ref<Asset> asset = m_asset_cache->try_acquire(asset_id);
        if (asset) {
            return AsyncAssetRef::completed(std::move(asset));
        }

        auto library = m_load_manager->find_library(asset_id);
        if (!library) {
            WG_LOG_ERROR("failed to find library to load " << asset_id);
            return AsyncAssetRef::failed();
        }

        if (is_import_enabled()) {
            bool need_reimport = false;
            if (!m_asset_db->need_asset_reimport(asset_id, need_reimport)) {
                WG_LOG_ERROR("failed to check reimport need for " << asset_id);
            }

            if (need_reimport) {
                auto reimport_result = reimport(asset_id);
                auto load_result     = make_async_op<Ref<Asset>>();

                reimport_result.add_on_completion([load_result, this](AsyncStatus status, std::optional<UUID>& asset_id) {
                    if (status == AsyncStatus::Failed) {
                        load_result->set_failed();
                        return;
                    }
                    auto wrapped_load_result = load(*asset_id);
                    wrapped_load_result.add_on_completion([load_result](AsyncStatus status, std::optional<Ref<Asset>>& asset) {
                        if (status == AsyncStatus::Failed) {
                            load_result->set_failed();
                            return;
                        }
                        load_result->set_result(Ref<Asset>(*asset));
                    });
                });

                return AsyncAssetRef(load_result);
            }
        }

        auto on_loaded = [this](const Ref<Asset>& asset) {
            asset->set_release_callback(m_callback);
            cache(asset, true);
        };

        return m_load_manager->load(asset_id, *library, on_loaded, [this](UUID asset_id) -> AsyncAssetRef {
            return load(asset_id);
        });
    }

    AssetManager::AsyncAssetRef AssetManager::load(const std::string& asset_name) {
        std::lock_guard guard(m_mutex);

        std::optional<AssetLibrary*> library = m_load_manager->find_library(asset_name);
        if (!library) {
            WG_LOG_ERROR("failed to find library to load asset " << asset_name);
            return AsyncAssetRef::failed();
        }

        UUID asset_id;
        if (!(*library)->resolve_asset(asset_name, asset_id)) {
            WG_LOG_ERROR("failed to resolve uuid to load asset " << asset_name);
            return AsyncAssetRef::failed();
        }

        return load(asset_id);
    }

    Ref<Asset> AssetManager::load_wait(UUID asset_id) {
        auto async = load(asset_id);
        async.wait_completed();
        return async.is_ok() ? async.result() : Ref<Asset>();
    }

    Ref<Asset> AssetManager::load_wait(const std::string& asset_name) {
        auto async = load(asset_name);
        async.wait_completed();
        return async.is_ok() ? async.result() : Ref<Asset>();
    }

    AssetManager::AsyncAssetId AssetManager::import(const std::string& asset_path, AssetFlags flags, AssetImporter* importer, const Ref<AssetImportSettings>& settings, const AssetImportEnv& env) {
        WG_PROFILE_CPU_ASSET("AssetManager::import");

        if (!is_import_enabled()) {
            WG_LOG_ERROR("import not allowed by asset manager settings");
            return AsyncAssetId::failed();
        }

        std::lock_guard guard(m_mutex);

        auto query_importing = m_importing.find(asset_path);
        if (query_importing != m_importing.end()) {
            return query_importing->second;
        }

        AsyncOp<UUID> async_result = make_async_op<UUID>();

        auto import_result = m_import_manager->import(asset_path, importer, settings, env, [this](UUID asset_id) -> AsyncAssetRef {
            return load(asset_id);
        });

        import_result.add_on_completion([asset_path, async_result, flags, settings, importer = importer->get_class(), asset_db = m_asset_db, this](AsyncStatus status, std::optional<AssetImportResult>& result) {
            std::lock_guard guard(m_mutex);
            m_importing.erase(asset_path);

            if (status == AsyncStatus::Failed) {
                async_result->set_failed();
                return;
            }
            UUID asset_id = result->main.uuid;
            if (!asset_db->import_asset(flags, importer, settings, *result)) {
                WG_LOG_ERROR("failed to import asset at " << result->main.path << " for uuid " << asset_id);
                async_result->set_failed();
                return;
            }
            async_result->set_result(UUID(asset_id));
        });

        m_importing[asset_path] = AsyncAssetId(async_result);

        return AsyncAssetId(async_result);
    }

    AssetManager::AsyncAssetId AssetManager::reimport(UUID asset_id) {
        WG_PROFILE_CPU_ASSET("AssetManager::reimport");

        if (!is_import_enabled()) {
            WG_LOG_ERROR("import not allowed by asset manager settings");
            return AsyncAssetId::failed();
        }

        std::lock_guard guard(m_mutex);

        std::string              asset_path;
        RttiRefClass             importer_cls;
        Ref<AssetImportSettings> settings;
        AssetImportEnv           env;

        std::optional<UUID> parent_id = m_asset_db->resolve_asset_parent(asset_id);
        if (!parent_id) {
            WG_LOG_ERROR("failed to resolve parent for assset " << asset_id);
            return AsyncAssetId::failed();
        }

        if (!m_asset_db->get_asset_import_settings(*parent_id, asset_path, importer_cls, settings, env)) {
            WG_LOG_ERROR("failed to get import settings for assset " << asset_id);
            return AsyncAssetId::failed();
        }

        auto query_importing = m_importing.find(asset_path);
        if (query_importing != m_importing.end()) {
            return query_importing->second;
        }

        std::optional<AssetImporter*> importer = m_import_manager->find_importer(importer_cls);
        if (!importer) {
            WG_LOG_ERROR("failed to get importer of class " << importer_cls << " for assset " << asset_id);
            return AsyncAssetId::failed();
        }

        auto async_result = make_async_op<UUID>();

        auto import_result = m_import_manager->import(asset_path, *importer, settings, env, [this](UUID asset_id) -> AsyncAssetRef {
            return load(asset_id);
        });

        import_result.add_on_completion([req_id = asset_id, asset_path, async_result, settings, asset_db = m_asset_db, this](AsyncStatus status, std::optional<AssetImportResult>& result) {
            std::lock_guard guard(m_mutex);
            m_importing.erase(asset_path);

            if (status == AsyncStatus::Failed) {
                async_result->set_failed();
                return;
            }
            UUID asset_id = result->main.uuid;
            if (!asset_db->reimport_asset(asset_id, settings, *result)) {
                WG_LOG_ERROR("failed to reimport asset at " << result->main.path << " for uuid " << req_id);
                async_result->set_failed();
                return;
            }
            if (!asset_db->has_asset(req_id)) {
                WG_LOG_ERROR("no asset after reimport " << result->main.path << " with uuid " << req_id);
                async_result->set_failed();
                return;
            }

            async_result->set_result(UUID(req_id));
        });

        m_importing[asset_path] = AsyncAssetId(async_result);

        return AsyncAssetId(async_result);
    }

    void AssetManager::cache(Ref<Asset> asset, bool replace) {
        std::lock_guard guard(m_mutex);

        if (is_caching_enabled()) {
            m_asset_cache->add(asset, replace);
        }
    }

}// namespace wmoge