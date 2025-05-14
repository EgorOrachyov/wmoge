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
#include "asset/asset_library.hpp"
#include "asset/asset_loader.hpp"
#include "core/async.hpp"
#include "core/flat_map.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

namespace wmoge {
    class FileSystem;
    class RttiTypeStorage;
    class IocContainer;
    class TaskManager;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetLoadManager
     * @brief Manages asset loading in engine runtime format
     */
    class AssetLoadManager {
    public:
        using AsyncAssetRef  = AsyncResult<Ref<Asset>>;
        using DepsResolver   = std::function<AsyncAssetRef(UUID)>;
        using NameResolver   = std::function<std::string(UUID)>;
        using LoadedCallback = std::function<void(const Ref<Asset>&)>;

        AssetLoadManager(NameResolver  name_resolver,
                         IoContext     io_context,
                         FileSystem*   file_system,
                         IocContainer* ioc_container,
                         TaskManager*  task_manager);

        [[nodiscard]] AsyncAssetRef                load(UUID asset_id, AssetLibrary* library, LoadedCallback callback, const DepsResolver& deps_resolver);
        [[nodiscard]] std::optional<AssetLoader*>  find_loader(RttiRefClass cls);
        [[nodiscard]] std::optional<AssetLibrary*> find_library(UUID asset_id);
        [[nodiscard]] std::optional<AssetLibrary*> find_library(const std::string& asset_name);
        void                                       add_loader(Ref<AssetLoader> loader);
        void                                       add_library(Ref<AssetLibrary> library);

    private:
        struct Entry : public RefCnt {
            std::vector<Async>           deps;
            std::vector<Ref<Data>>       buffers;
            std::vector<Ref<RttiObject>> artifacts;
            AssetLibraryRecord           asset_info;
            LoadedCallback               callback;
            AsyncOp<Ref<Asset>>          async_op;
            AssetLibrary*                library = nullptr;
            AssetLoader*                 loader  = nullptr;
            RttiRefClass                 cls;
        };

        flat_map<UUID, Ref<Entry>>             m_loading;
        flat_map<RttiClass*, Ref<AssetLoader>> m_loaders;
        std::vector<Ref<AssetLibrary>>         m_libraries;
        NameResolver                           m_name_resolver;
        IoContext                              m_io_context;
        FileSystem*                            m_file_system   = nullptr;
        IocContainer*                          m_ioc_container = nullptr;
        TaskManager*                           m_task_manager  = nullptr;
        mutable std::recursive_mutex           m_mutex;
    };

}// namespace wmoge