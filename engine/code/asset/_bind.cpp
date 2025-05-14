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

#include "_bind.hpp"

#include "asset/asset_artifact_cache.hpp"
#include "asset/asset_cache.hpp"
#include "asset/asset_db.hpp"
#include "asset/asset_import_manager.hpp"
#include "asset/asset_library_db.hpp"
#include "asset/asset_load_manager.hpp"
#include "asset/asset_manager.hpp"
#include "asset/asset_resolver.hpp"
#include "core/ioc_container.hpp"
#include "core/task_manager.hpp"
#include "io/async_file_system.hpp"
#include "io/context.hpp"
#include "platform/file_system.hpp"
#include "rtti/type_storage.hpp"

namespace wmoge {

    void bind_asset(class IocContainer* ioc) {
        ioc->bind_by_factory<IoContext>([ioc]() {
            IoContext context;
            context.add(ioc->resolve_value<FileSystem>());
            context.add(ioc->resolve_value<RttiTypeStorage>());
            context.add(ioc->resolve_value<AssetCache>());
            context.add(ioc->resolve_value<AssetResolver>());
            return std::make_shared<IoContext>(std::move(context));
        });

        ioc->bind<AssetCache>();
        ioc->bind<AssetResolver>();

        ioc->bind_by_factory<AssetArtifactCache>([ioc]() {
            return std::make_shared<AssetArtifactCache>(
                    "local/asset/cache",
                    *ioc->resolve_value<IoContext>(),
                    ioc->resolve_value<FileSystem>(),
                    ioc->resolve_value<IoAsyncFileSystem>(),
                    ioc->resolve_value<TaskManager>());
        });

        ioc->bind_by_factory<AssetDb>([ioc]() {
            return std::make_shared<AssetDb>(
                    "local/asset/db/assets.db",
                    *ioc->resolve_value<IoContext>(),
                    ioc->resolve_value<FileSystem>(),
                    ioc->resolve_value<AssetResolver>(),
                    ioc->resolve_value<AssetArtifactCache>());
        });

        ioc->bind_by_factory<AssetImportManager>([ioc]() {
            AssetDb*                         asset_db      = ioc->resolve_value<AssetDb>();
            AssetImportManager::UuidProvider uuid_provider = [asset_db]() -> UUID {
                return asset_db->alloc_asset_uuid();
            };

            return std::make_shared<AssetImportManager>(
                    std::move(uuid_provider),
                    *ioc->resolve_value<IoContext>(),
                    ioc->resolve_value<FileSystem>(),
                    ioc,
                    ioc->resolve_value<TaskManager>());
        });

        ioc->bind_by_factory<AssetLoadManager>([ioc]() {
            AssetResolver*                 asset_resolver = ioc->resolve_value<AssetResolver>();
            AssetLoadManager::NameResolver name_resolver  = [asset_resolver](UUID asset_id) -> std::string {
                auto resolve = asset_resolver->resolve(asset_id);
                return resolve ? *resolve + " uuid=" + asset_id.to_str() : "uuid=" + asset_id.to_str();
            };

            return std::make_shared<AssetLoadManager>(
                    std::move(name_resolver),
                    *ioc->resolve_value<IoContext>(),
                    ioc->resolve_value<FileSystem>(),
                    ioc,
                    ioc->resolve_value<TaskManager>());
        });

        ioc->bind_by_factory<AssetManager>([ioc]() {
            return std::make_shared<AssetManager>(
                    ioc->resolve_value<FileSystem>(),
                    ioc->resolve_value<AssetDb>(),
                    ioc->resolve_value<AssetCache>(),
                    ioc->resolve_value<AssetLoadManager>(),
                    ioc->resolve_value<AssetImportManager>());
        });
    }

    void unbind_asset(class IocContainer* ioc) {
        ioc->unbind<AssetManager>();
        ioc->unbind<AssetLoadManager>();
        ioc->unbind<AssetImportManager>();
        ioc->unbind<AssetDb>();
        ioc->unbind<AssetArtifactCache>();
        ioc->unbind<AssetResolver>();
        ioc->unbind<AssetCache>();
        ioc->unbind<IoContext>();
    }

}// namespace wmoge