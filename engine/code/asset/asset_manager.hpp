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

#include "asset/asset.hpp"
#include "asset/asset_import_env.hpp"
#include "asset/asset_import_settings.hpp"
#include "core/async.hpp"
#include "core/flat_map.hpp"
#include "core/uuid.hpp"
#include "rtti/type_ref.hpp"

#include <functional>
#include <memory>
#include <mutex>

namespace wmoge {
    class FileSystem;
    class TaskManager;
    class AssetDb;
    class AssetCache;
    class AssetLoadManager;
    class AssetImporter;
    class AssetImportManager;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetManager
     * @brief Manages assets importing, loading and caching in the engine
     */
    class AssetManager {
    public:
        using AsyncAssetRef = AsyncResult<Ref<Asset>>;
        using AsyncAssetId  = AsyncResult<UUID>;

        AssetManager(FileSystem*         file_system,
                     AssetDb*            asset_db,
                     AssetCache*         asset_cache,
                     AssetLoadManager*   load_manager,
                     AssetImportManager* import_manager);

        [[nodiscard]] AsyncAssetRef load(UUID asset_id);
        [[nodiscard]] AsyncAssetRef load(const std::string& asset_name);
        [[nodiscard]] Ref<Asset>    load_wait(UUID asset_id);
        [[nodiscard]] Ref<Asset>    load_wait(const std::string& asset_name);
        [[nodiscard]] AsyncAssetId  import(const std::string& asset_path, AssetFlags flags, AssetImporter* importer, const Ref<AssetImportSettings>& settings, AssetImportEnv env);
        [[nodiscard]] AsyncAssetId  reimport(UUID asset_id);
        void                        cache(Ref<Asset> asset, bool replace = true);
        [[nodiscard]] bool          is_import_enabled() const { return m_import_enabled; }
        [[nodiscard]] bool          is_caching_enabled() const { return m_caching_enabled; }

    private:
        using EvictCallback    = std::function<void(Asset*)>;
        using EvictCallbackPtr = std::shared_ptr<EvictCallback>;

        flat_map<std::string, AsyncAssetId> m_importing;
        EvictCallbackPtr                    m_callback;
        FileSystem*                         m_file_system     = nullptr;
        AssetDb*                            m_asset_db        = nullptr;
        AssetCache*                         m_asset_cache     = nullptr;
        AssetLoadManager*                   m_load_manager    = nullptr;
        AssetImportManager*                 m_import_manager  = nullptr;
        bool                                m_import_enabled  = true;
        bool                                m_caching_enabled = true;
        mutable std::recursive_mutex        m_mutex;
    };

}// namespace wmoge