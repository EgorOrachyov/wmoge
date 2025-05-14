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

#include "asset_db.hpp"

#include "asset/asset_artifact_cache.hpp"
#include "asset/asset_manifest.hpp"
#include "asset/asset_resolver.hpp"
#include "io/stream_file.hpp"
#include "io/tree_yaml.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

#include <algorithm>
#include <filesystem>
#include <unordered_set>

namespace wmoge {

    AssetDb::AssetDb(std::string         file_path,
                     IoContext           io_context,
                     FileSystem*         file_system,
                     AssetResolver*      asset_resolver,
                     AssetArtifactCache* artifact_cache)
        : m_file_path(std::move(file_path)),
          m_io_context(std::move(io_context)),
          m_file_system(file_system),
          m_asset_resolver(asset_resolver),
          m_artifact_cache(artifact_cache) {
    }

    bool AssetDb::has_asset(UUID asset_id) {
        std::lock_guard guard(m_mutex);

        return m_assets.find(asset_id) != m_assets.end();
    }

    std::optional<UUID> AssetDb::find_asset(const std::string& name) {
        WG_PROFILE_CPU_ASSET("AssetDb::find_asset");

        std::lock_guard guard(m_mutex);

        for (const auto& iter : m_assets) {
            if (iter.second.path == name) {
                return iter.second.uuid;
            }
        }

        return std::nullopt;
    }

    std::optional<UUID> AssetDb::resolve_asset_parent(UUID asset_id) {
        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return std::nullopt;
        }

        const UUID parent = m_assets[asset_id].parent;
        return parent ? parent : asset_id;
    }

    Status AssetDb::import_asset(AssetFlags flags, RttiRefClass importer, Ref<AssetImportSettings> import_settings, const AssetImportResult& import_result) {
        WG_PROFILE_CPU_ASSET("AssetDb::import_asset");

        std::lock_guard guard(m_mutex);

        if (has_asset(import_result.main.uuid)) {
            return StatusCode::InvalidState;
        }

        auto store_artifacts = [&](const std::vector<AssetImportArtifactInfo>& in, std::vector<AssetArtifact>& out) -> Status {
            for (const AssetImportArtifactInfo& info : in) {
                UUID artifact_id;
                WG_CHECKED(m_artifact_cache->add(info.object, info.name, artifact_id));

                AssetArtifact artifact;
                WG_CHECKED(m_artifact_cache->get_info(artifact_id, artifact));

                out.push_back(std::move(artifact));
            }
            return WG_OK;
        };

        const AssetImportAssetInfo& main = import_result.main;

        std::size_t       next_child_id = 0;
        std::vector<UUID> children_uuid;

        children_uuid.reserve(import_result.children.size());

        for (const AssetImportAssetInfo& child : import_result.children) {
            AssetData asset;
            asset.uuid      = child.uuid;
            asset.path      = child.path;
            asset.flags     = child.flags;
            asset.parent    = main.uuid;
            asset.cls       = child.cls;
            asset.loader    = child.loader;
            asset.timestamp = import_result.timestamp;

            children_uuid.push_back(asset.uuid);
            WG_CHECKED(store_artifacts(child.artifacts, asset.artifacts));
            WG_CHECKED(add_asset(std::move(asset)));
        }

        AssetData asset;
        asset.uuid            = main.uuid;
        asset.flags           = flags | main.flags;
        asset.path            = main.path;
        asset.deps            = main.deps;
        asset.cls             = main.cls;
        asset.children        = std::move(children_uuid);
        asset.loader          = main.loader;
        asset.importer        = importer;
        asset.import_env      = import_result.env;
        asset.import_settings = import_settings;
        asset.timestamp       = import_result.timestamp;

        for (const std::string& source : import_result.sources) {
            AssetSource asset_source;
            asset_source.path = source;
            WG_CHECKED(m_file_system->hash_file(asset_source.path, asset_source.hash));
            WG_CHECKED(m_file_system->get_file_timestamp(asset_source.path, asset_source.timestamp));
            asset.sources.push_back(std::move(asset_source));
        }

        WG_CHECKED(store_artifacts(main.artifacts, asset.artifacts));
        WG_CHECKED(add_asset(std::move(asset)));
        return WG_OK;
    }

    Status AssetDb::reimport_asset(UUID asset_id, Ref<AssetImportSettings> import_settings, const AssetImportResult& import_result) {
        WG_PROFILE_CPU_ASSET("AssetDb::reimport_asset");

        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            WG_LOG_ERROR("no asset to re-import with uuid " << asset_id);
            return StatusCode::NoAsset;
        }
        if (asset_id != import_result.main.uuid) {
            WG_LOG_ERROR("unexpected main asset id " << import_result.main.uuid << " for " << asset_id);
            return StatusCode::InvalidData;
        }

        const AssetData& asset = m_assets[asset_id];

        const AssetFlags   flags    = asset.flags;
        const RttiRefClass importer = asset.importer;

        WG_CHECKED(remove_asset(asset_id));
        WG_CHECKED(import_asset(flags, importer, import_settings, import_result));

        return WG_OK;
    }

    Status AssetDb::add_asset(AssetData asset_data, UUID& asset_id) {
        std::lock_guard guard(m_mutex);

        asset_id        = gen_asset_uuid();
        asset_data.uuid = asset_id;

        return add_asset(std::move(asset_data));
    }

    Status AssetDb::add_asset(AssetData asset_data) {
        std::lock_guard guard(m_mutex);

        const UUID asset_id = asset_data.uuid;

        asset_data.import_env.file_to_id[asset_data.path] = asset_id;
        m_asset_resolver->add(asset_data.path, asset_id);
        m_assets[asset_id] = std::move(asset_data);

        WG_CHECKED(validate_asset_artifacts(asset_id));
        WG_CHECKED(save_asset_meta(asset_id));

        return WG_OK;
    }

    Status AssetDb::create_asset_from_meta(const std::string& asset_path, const AssetMetaData& asset_meta_data, UUID& asset_id) {
        WG_PROFILE_CPU_ASSET("AssetDb::create_asset_from_meta");

        std::lock_guard guard(m_mutex);

        asset_id = asset_meta_data.uuid ? asset_meta_data.uuid : alloc_asset_uuid();

        AssetData asset;
        asset.path            = asset_path;
        asset.uuid            = asset_id;
        asset.cls             = asset_meta_data.cls;
        asset.flags           = asset_meta_data.flags;
        asset.importer        = asset_meta_data.importer;
        asset.import_env      = asset_meta_data.import_env;
        asset.import_settings = asset_meta_data.import_settings;

        return add_asset(std::move(asset));
    }

    Status AssetDb::create_asset_from_meta_path(const std::string& asset_path, UUID& asset_id, bool skip_if_exists) {
        WG_PROFILE_CPU_ASSET("AssetDb::create_asset_from_meta");

        std::lock_guard guard(m_mutex);

        AssetMetaData meta_data;
        {
            IoContext  context = m_io_context;
            IoYamlTree tree;
            WG_CHECKED(tree.parse_file(m_file_system, asset_meta_file(asset_path)));
            WG_TREE_READ(context, tree, meta_data);
        }

        if (meta_data.uuid && has_asset(meta_data.uuid)) {
            if (skip_if_exists) {
                asset_id = meta_data.uuid;
                return WG_OK;
            } else {
                WG_CHECKED(remove_asset(meta_data.uuid));
            }
        }

        return create_asset_from_meta(asset_path, std::move(meta_data), asset_id);
    }

    Status AssetDb::remove_asset(UUID asset_id) {
        WG_PROFILE_CPU_ASSET("AssetDb::remove_asset");

        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        while (!m_assets[asset_id].children.empty()) {
            UUID child = m_assets[asset_id].children.back();
            WG_CHECKED(remove_asset(child));
        }

        AssetData& self = m_assets[asset_id];

        if (self.parent.is_not_null()) {
            AssetData& parent = m_assets[self.parent];
            parent.children.erase(std::find(parent.children.begin(), parent.children.end(), asset_id));
        }

        for (const auto& artifact : self.artifacts) {
            WG_CHECKED(m_artifact_cache->remove(artifact.uuid));
        }

        m_asset_resolver->remove(self.path);
        m_assets.erase(asset_id);
        return WG_OK;
    }

    Status AssetDb::collect_asset_usages(UUID asset_id, std::vector<UUID>& asset_users) {
        WG_PROFILE_CPU_ASSET("AssetDb::collect_asset_usages");

        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        for (const auto& entry : m_assets) {
            const AssetData& asset = entry.second;
            auto             iter  = std::find(asset.children.begin(), asset.children.end(), asset_id);
            if (iter != asset.children.end()) {
                asset_users.push_back(entry.first);
            }
        }

        return WG_OK;
    }

    Status AssetDb::validate_asset_artifacts(UUID asset_id) {
        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        bool need_invalidation = false;

        AssetData& asset = m_assets[asset_id];
        for (const AssetArtifact& artifact : asset.artifacts) {
            if (!m_artifact_cache->has(artifact.uuid)) {
                need_invalidation = true;
                break;
            }
        }

        if (need_invalidation) {
            WG_CHECKED(invalidate_asset_artifacts(asset_id));
        }

        return WG_OK;
    }

    Status AssetDb::invalidate_asset_artifacts(UUID asset_id) {
        WG_PROFILE_CPU_ASSET("AssetDb::invalidate_asset_artifacts");

        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        AssetData& asset = m_assets[asset_id];
        for (const AssetArtifact& artifact : asset.artifacts) {
            m_artifact_cache->remove(artifact.uuid);
        }
        asset.artifacts.clear();

        return WG_OK;
    }

    Status AssetDb::get_asset_import_settings(UUID asset_id, std::string& asset_path, RttiRefClass& importer, Ref<AssetImportSettings>& import_settings, AssetImportEnv& import_env) {
        WG_PROFILE_CPU_ASSET("AssetDb::get_asset_import_settings");

        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        const AssetData& asset = m_assets[asset_id];
        asset_path             = asset.path;
        importer               = asset.importer;
        import_settings        = asset.import_settings;
        import_env             = asset.import_env;

        return WG_OK;
    }

    Status AssetDb::get_asset_meta(UUID asset_id, AssetMetaData& asset_meta_data) {
        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        const AssetData& asset          = m_assets[asset_id];
        asset_meta_data.uuid            = asset.uuid;
        asset_meta_data.flags           = asset.flags;
        asset_meta_data.cls             = asset.cls;
        asset_meta_data.importer        = asset.importer;
        asset_meta_data.import_env      = asset.import_env;
        asset_meta_data.import_settings = asset.import_settings;

        return WG_OK;
    }

    Status AssetDb::get_asset_loading_info(UUID asset_id, RttiRefClass& cls, RttiRefClass& loader, std::vector<UUID>& deps, std::vector<UUID>& artifacts) {
        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        const AssetData& asset = m_assets[asset_id];
        cls                    = asset.cls;
        loader                 = asset.loader;
        deps                   = asset.deps;

        artifacts.resize(asset.artifacts.size());
        std::transform(asset.artifacts.begin(), asset.artifacts.end(), artifacts.begin(), [](const AssetArtifact& artifact) {
            return artifact.uuid;
        });

        return WG_OK;
    }

    Status AssetDb::save_asset_meta(UUID asset_id) {
        std::lock_guard guard(m_mutex);

        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }

        const AssetData& asset = m_assets[asset_id];

        AssetMetaData meta_data;
        WG_CHECKED(get_asset_meta(asset_id, meta_data));
        {
            IoContext  context = m_io_context;
            IoYamlTree tree;
            WG_CHECKED(tree.create_tree());
            WG_TREE_WRITE(context, tree, meta_data);
            WG_CHECKED(tree.save_tree(m_file_system, asset_meta_file(asset.path)));
        }

        return WG_OK;
    }

    Status AssetDb::need_asset_reimport(UUID asset_id, bool& need_reimport) {
        WG_PROFILE_CPU_ASSET("AssetDb::need_asset_reimport");

        std::lock_guard guard(m_mutex);

        WG_CHECKED(remap_to_parent_and_check(asset_id));

        const AssetData& asset = m_assets[asset_id];

        need_reimport = false;

        need_reimport = need_reimport || asset.sources.empty();
        need_reimport = need_reimport || asset.artifacts.empty();

        if (!need_reimport) {
            for (const AssetSource& source : asset.sources) {
                DateTime timestamp;
                WG_CHECKED(m_file_system->get_file_timestamp(source.path, timestamp));
                need_reimport = need_reimport || timestamp > source.timestamp;
            }
        }

        if (!need_reimport) {
            DateTime timestamp;
            WG_CHECKED(m_file_system->get_file_timestamp(asset_meta_file(asset.path), timestamp));
        }

        if (!need_reimport) {
            for (const UUID& child_id : asset.children) {
                if (!has_asset(child_id)) {
                    return StatusCode::NoAsset;
                }
                const AssetData& child = m_assets[child_id];
                need_reimport          = need_reimport || child.artifacts.empty();
            }
        }

        return WG_OK;
    }

    Status AssetDb::load_manifest(const std::string& manifest_path) {
        WG_PROFILE_CPU_ASSET("AssetDb::load_manifest");

        AssetManifest manifest;
        {
            IoContext  context;
            IoYamlTree tree;
            WG_CHECKED(tree.parse_file(m_file_system, manifest_path));
            WG_TREE_READ(context, tree, manifest);
        }

        const std::string folder_path = std::filesystem::path(manifest_path).parent_path().string();
        std::size_t       counter     = 0;

        for (std::string asset : manifest.assets) {
            if (StringUtils::is_starts_with(asset, "./")) {
                asset = asset.substr(2);
            }
            UUID        asset_id;
            std::string asset_path = folder_path + "/" + asset;
            if (!create_asset_from_meta_path(asset_path, asset_id)) {
                WG_LOG_ERROR("failed to create from manifest asset, skipping " << asset_path);
            } else {
                counter++;
            }
        }

        WG_LOG_INFO("created " << counter << " asset(s) from manifest " << manifest_path);
        return WG_OK;
    }

    Status AssetDb::save_db() {
        WG_PROFILE_CPU_ASSET("AssetDb::save_db");

        std::lock_guard guard(m_mutex);

        std::vector<AssetData> data;
        data.reserve(m_assets.size());

        for (const auto& entry : m_assets) {
            data.push_back(entry.second);
        }

        IoContext    context = m_io_context;
        IoStreamFile stream;
        WG_CHECKED(stream.open(m_file_system, m_file_path, {FileOpenMode::Out, FileOpenMode::Binary}));
        WG_ARCHIVE_WRITE(context, stream, data);

        WG_LOG_INFO("saved " << data.size() << " asset(s) to " << m_file_path);
        return WG_OK;
    }

    Status AssetDb::load_db(bool allow_missing) {
        WG_PROFILE_CPU_ASSET("AssetDb::load_db");

        std::lock_guard guard(m_mutex);

        if (!m_file_system->exists(m_file_path)) {
            if (allow_missing) {
                WG_LOG_INFO("no asset db to load " << m_file_path << ", ok - creating empty");
                return WG_OK;
            } else {
                WG_LOG_ERROR("no asset db to load " << m_file_path);
                return StatusCode::FailedFindFile;
            }
        }

        std::vector<AssetData> data;

        IoContext    context = m_io_context;
        IoStreamFile stream;
        WG_CHECKED(stream.open(m_file_system, m_file_path, {FileOpenMode::In, FileOpenMode::Binary}));
        WG_ARCHIVE_READ(context, stream, data);

        for (auto& entry : data) {
            UUID id = entry.uuid;
            WG_CHECKED(add_asset(std::move(entry)));
        }

        WG_LOG_INFO("loaded " << data.size() << " asset(s) from " << m_file_path);
        return WG_OK;
    }

    Status AssetDb::reconcile_db() {
        WG_PROFILE_CPU_ASSET("AssetDb::reconcile_db");

        std::lock_guard guard(m_mutex);

        std::vector<UUID> asset_ids;
        asset_ids.reserve(m_assets.size());

        for (const auto& entry : m_assets) {
            if (entry.second.parent.is_null()) {
                asset_ids.push_back(entry.second.uuid);
            }
        }

        std::size_t count_total   = m_assets.size();
        std::size_t count_removed = 0;

        for (const UUID& asset_id : asset_ids) {
            const AssetData& asset = m_assets[asset_id];
            if (!m_file_system->exists(asset_meta_file(asset.path))) {
                count_removed += 1 + asset.children.size();
                WG_CHECKED(remove_asset(asset_id));
            }
        }

        WG_LOG_INFO("reconciled assets removed=" << count_removed << " preserved=" << count_total - count_removed);
        return WG_OK;
    }

    UUID AssetDb::alloc_asset_uuid() {
        std::lock_guard guard(m_mutex);
        return gen_asset_uuid();
    }

    UUID AssetDb::gen_asset_uuid() {
        while (true) {
            UUID id = UUID::generate();
            if (m_assets.find(id) == m_assets.end() && m_ids.find(id) == m_ids.end()) {
                m_ids.insert(id);
                return id;
            }
        }
    }

    std::vector<UUID> AssetDb::gen_asset_uuids(std::size_t count) {
        std::vector<UUID> ids;
        for (std::size_t i = 0; i < count; i++) {
            ids.push_back(gen_asset_uuid());
        }
        return std::move(ids);
    }

    std::string AssetDb::asset_meta_file(const std::string& asset_path) const {
        return asset_path + AssetMetaData::get_extension_static();
    }

    Status AssetDb::remap_to_parent_and_check(UUID& asset_id) {
        if (!has_asset(asset_id)) {
            return StatusCode::NoAsset;
        }
        const AssetData& asset = m_assets[asset_id];
        if (asset.parent.is_not_null()) {
            asset_id = asset.parent;
            if (!has_asset(asset_id)) {
                return StatusCode::NoAsset;
            }
        }
        return WG_OK;
    }

}// namespace wmoge