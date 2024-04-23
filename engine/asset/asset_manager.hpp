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

        /**
         * @brief Async load of engine asset using provided asset name
         *
         * Allows to load game asset by its name. Assets are stored inside game
         * asset pak file. Asset manager automatically resolves asset path by its name.
         *
         * @note Each asset must have a asset meta info file (in .xml) format. This meta info
         *       file allows to get asset reflection data, required to load asset at runtime.
         *
         * @note If asset already loaded and cached in the engine,
         *       the reference to loaded instance is returned.
         *
         * @note If asset is already queued to be loaded, reference to loaded asset is returned.
         *
         * @note Pass callback function to be notified when asset loading is finished.
         *       If asset already cached, this function will be called immediately before function return.
         *
         * @param name Unique name of the asset to load
         * @param callback Callback to call on main thread when asset is loaded.
         *
         * @return Asset reference
         */
        AsyncResult<Ref<Asset>> load_async(const AssetId& name, AssetCallback callback = AssetCallback());

        /**
         * @brief Sync load of the engine asset using provided asset name
         *
         * Allows to load game asset by its name. Assets are stored inside game
         * asset pak file. Asset manager automatically resolves asset path by its name.
         *
         * @note Each asset must have a asset meta info file (in .xml) format. This meta info
         *       file allows to get asset reflection data, required to load asset at runtime.
         *
         * @note If asset already loaded and cached in the engine,
         *       the reference to loaded instance is returned.
         *
         * @note Sync asset loading is a simple and straightforward approach to work with
         *       assets. But blocks current thread and takes extra processing time in case of
         *       sync load of a large amount of assets. Consider using `load_async` method.
         *
         * @param name Unique name of the asset to load
         *
         * @return Asset reference
         */
        Ref<Asset> load(const AssetId& name);

        /** @brief Find a asset by name if it is already cached */
        Ref<Asset> find(const AssetId& name);

        /** @brief Add specific format asset loader */
        void add_loader(Ref<AssetLoader> loader);

        /** @brief Add additional pak for assets loading */
        void add_pak(std::shared_ptr<AssetPak> pak);

        /** @brief Find asset loader by loader name */
        std::optional<AssetLoader*> find_loader(const Strid& loader);

        /** @brief Find asset meta by asset name */
        std::optional<AssetMeta> find_meta(const AssetId& asset);

        /**
         * @brief Clear from a cache only unused asset
         *
         * This is a costly operation, which traverses all cached
         * assets in a asset system and evicts those entries,
         * which are not used by the engine at this time.
         *
         * @note This operation allows to free some used memory and assets
         *       at cost of traversal plus potential loading of asset,
         *       if they requested in future.
         *
         * @note Call this operation with regular intervals in couple of frames
         *       or on scene changes or large streaming chunks updates.
         */
        void gc();

        /**
         * @brief Evicts all loaded assets from a cache
         *
         * Clear entirely cache of loaded assets. Any new asset loading
         * operation will require asset loading from a disk.
         *
         * @note Clearing cache does not free memory of currently used assets
         *       in the engine due to strong memory references.
         */
        void clear();

    private:
        /**
         * @class LoadState
         * @brief Tracks loading state of a single asset
         */
        class LoadState {
        public:
            buffered_vector<Async> deps;
            AsyncOp<Ref<Asset>>    async_op;
            TaskHnd                task_hnd;
        };

    private:
        buffered_vector<std::shared_ptr<AssetPak>> m_paks;
        flat_map<AssetId, WeakRef<Asset>>          m_assets;
        flat_map<AssetId, LoadState>               m_loading;
        flat_map<Strid, Ref<AssetLoader>>          m_loaders;
        class FileSystem*                          m_file_system  = nullptr;
        class RttiTypeStorage*                     m_type_storage = nullptr;

        mutable std::recursive_mutex m_mutex;
    };

}// namespace wmoge