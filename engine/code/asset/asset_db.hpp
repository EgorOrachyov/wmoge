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

#pragma once

#include "asset/asset_data.hpp"
#include "asset/asset_meta_data.hpp"
#include "core/flat_set.hpp"
#include "io/context.hpp"

#include <mutex>
#include <unordered_map>

namespace wmoge {
    class FileSystem;
    class AssetResolver;
    class AssetArtifactCache;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetDb
     * @brief Asset database for accessing engine or editor assets for processing
     */
    class AssetDb {
    public:
        AssetDb(std::string         file_path,
                IoContext           io_context,
                FileSystem*         file_system,
                AssetResolver*      asset_resolver,
                AssetArtifactCache* artifact_cache);

        [[nodiscard]] bool                has_asset(UUID asset_id);
        [[nodiscard]] std::optional<UUID> find_asset(const std::string& name);
        [[nodiscard]] std::optional<UUID> resolve_asset_parent(UUID asset_id);
        Status                            import_asset(AssetFlags flags, RttiRefClass importer, Ref<AssetImportSettings> import_settings, const AssetImportResult& import_result);
        Status                            reimport_asset(UUID asset_id, Ref<AssetImportSettings> import_settings, const AssetImportResult& import_result);
        Status                            add_asset(AssetData asset_data, UUID& asset_id);
        Status                            add_asset(AssetData asset_data);
        Status                            create_asset_from_meta(const std::string& asset_path, const AssetMetaData& asset_meta_data, UUID& asset_id);
        Status                            create_asset_from_meta_path(const std::string& asset_meta_path, UUID& asset_id, bool skip_if_exists = true);
        Status                            remove_asset(UUID asset_id);
        Status                            collect_asset_usages(UUID asset_id, std::vector<UUID>& asset_users);
        Status                            validate_asset_artifacts(UUID asset_id);
        Status                            invalidate_asset_artifacts(UUID asset_id);
        Status                            get_asset_import_settings(UUID asset_id, std::string& asset_path, RttiRefClass& importer, Ref<AssetImportSettings>& import_settings, AssetImportEnv& import_env);
        Status                            get_asset_meta(UUID asset_id, AssetMetaData& asset_meta_data);
        Status                            get_asset_loading_info(UUID asset_id, RttiRefClass& cls, RttiRefClass& loader, std::vector<UUID>& deps, std::vector<UUID>& artifacts);
        Status                            save_asset_meta(UUID asset_id);
        Status                            need_asset_reimport(UUID asset_id, bool& need_reimport);
        Status                            load_manifest(const std::string& manifest_path);
        Status                            save_db();
        Status                            load_db(bool allow_missing = true);
        Status                            reconcile_db();
        [[nodiscard]] UUID                alloc_asset_uuid();

    private:
        [[nodiscard]] UUID              gen_asset_uuid();
        [[nodiscard]] std::vector<UUID> gen_asset_uuids(std::size_t count);
        [[nodiscard]] std::string       asset_meta_file(const std::string& asset_path) const;
        Status                          remap_to_parent_and_check(UUID& asset_id);

    private:
        std::unordered_map<UUID, AssetData> m_assets;
        flat_set<UUID>                      m_ids;
        std::string                         m_file_path;
        IoContext                           m_io_context;
        FileSystem*                         m_file_system    = nullptr;
        AssetResolver*                      m_asset_resolver = nullptr;
        AssetArtifactCache*                 m_artifact_cache = nullptr;
        mutable std::recursive_mutex        m_mutex;
    };

}// namespace wmoge