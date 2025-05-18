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
#include "asset/asset_importer.hpp"
#include "core/async.hpp"
#include "core/flat_map.hpp"
#include "io/context.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {
    class FileSystem;
    class IocContainer;
    class TaskManager;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetImportManager
     * @brief Manages import process of assets to engine from external formats
     */
    class AssetImportManager {
    public:
        using AsyncAssetRef = AsyncResult<Ref<Asset>>;
        using DepsResolver  = std::function<AsyncAssetRef(UUID)>;
        using UuidProvider  = std::function<UUID()>;

        AssetImportManager(UuidProvider  uuid_provider,
                           IoContext     io_context,
                           FileSystem*   file_system,
                           IocContainer* ioc_containter,
                           TaskManager*  task_manager);

        [[nodiscard]] AsyncResult<AssetImportResult> import(const std::string& path, AssetImporter* importer, const Ref<AssetImportSettings>& settings, const AssetImportEnv& env, const DepsResolver& deps_resolver);
        [[nodiscard]] AsyncResult<AssetImportResult> try_find_import(const std::string& path);
        [[nodiscard]] std::optional<AssetImporter*>  find_importer(const std::string& path);
        [[nodiscard]] std::optional<AssetImporter*>  find_importer(RttiRefClass importer_cls);
        void                                         add_importer(Ref<AssetImporter> importer);

    private:
        struct Entry : public RefCnt {
            std::vector<Async>                  deps;
            AsyncOp<AssetImportResult>          async_op;
            Ref<AssetImportSettings>            settings;
            AssetImportEnv                      env;
            AssetImporter*                      importer;
            std::unique_ptr<AssetImportContext> import_context;
        };

        flat_map<std::string, Ref<Entry>>     m_importing;
        std::vector<Ref<AssetImporter>>       m_importers;
        flat_map<std::string, AssetImporter*> m_extensions_map;
        UuidProvider                          m_uuid_provider;
        IoContext                             m_io_context;
        FileSystem*                           m_file_system    = nullptr;
        IocContainer*                         m_ioc_containter = nullptr;
        TaskManager*                          m_task_manager   = nullptr;
        mutable std::recursive_mutex          m_mutex;
    };

}// namespace wmoge