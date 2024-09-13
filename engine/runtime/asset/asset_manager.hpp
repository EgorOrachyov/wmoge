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
#include "asset/asset_loader.hpp"
#include "asset/asset_meta.hpp"
#include "asset/asset_pak.hpp"
#include "core/async.hpp"
#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "core/object.hpp"
#include "core/string_id.hpp"
#include "core/task.hpp"
#include "rtti/traits.hpp"

#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace wmoge {

    /**
     * @brief Callback function called when asset loading request is finished
     *
     * Use this callback function in async asset loading request.
     * As argument function accepts asset being loaded.
     * Function called when either asset successfully loaded or failed to load.
     */
    using AssetCallback = AsyncCallback<Ref<Asset>>;

    /**
     * @class AssetManager
     * @brief Manages assets loading and caching in the engine
     *
     * Asset manager is responsible for all engine assets management.
     * It supports all common asset manipulation operations.
     *
     * - Automatically resolves asset names
     * - Uses asset pak to abstract asset storage
     * - Allows to load a asset using name
     * - Allows async loading
     * - Allows async loading of asset with dependencies
     */
    class AssetManager {
    public:
        AssetManager();
        ~AssetManager() = default;

        AsyncResult<Ref<Asset>>       load_async(const AssetId& name, AssetCallback callback = AssetCallback());
        Ref<Asset>                    load(const AssetId& name);
        Ref<Asset>                    find(const AssetId& name);
        void                          add_loader(Ref<AssetLoader> loader);
        void                          add_unloader(Ref<AssetUnloader> unloader);
        void                          add_pak(std::shared_ptr<AssetPak> pak);
        std::optional<AssetLoader*>   find_loader(const Strid& loader_rtti);
        std::optional<AssetUnloader*> find_unloader(const Strid& asset_rtti);
        std::optional<AssetMeta>      find_meta(const AssetId& asset);
        void                          clear();
        void                          load_loaders();

    private:
        struct LoadState {
            buffered_vector<Async> deps;
            AsyncOp<Ref<Asset>>    async_op;
            TaskHnd                task_hnd;
        };

        buffered_vector<std::shared_ptr<AssetPak>>   m_paks;
        flat_map<AssetId, WeakRef<Asset>>            m_assets;
        flat_map<AssetId, LoadState>                 m_loading;
        flat_map<Strid, Ref<AssetLoader>>            m_loaders;
        flat_map<Strid, Ref<AssetUnloader>>          m_unloaders;
        std::shared_ptr<std::function<void(Asset*)>> m_callback;

        class FileSystem*      m_file_system  = nullptr;
        class RttiTypeStorage* m_type_storage = nullptr;

        mutable std::recursive_mutex m_mutex;
    };

}// namespace wmoge