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

#include "asset/asset_loader.hpp"

namespace wmoge {

    /**
     * @class AssetLoaderTyped
     * @brief Adapter to implement asset loaders of a particular asset type
     */
    template<typename T>
    class AssetLoaderTyped : public AssetLoader {
    public:
        static_assert(std::is_base_of_v<Asset, T>, "T must be an Asset type");

        static inline const Strid FILE_TAG = SID("file");

        virtual Status load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<T>& asset) { return StatusCode::NotImplemented; }
        virtual Status unload_typed(T* asset) { return StatusCode::Ok; }

        Status fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) override {
            for (const Strid& name : context.asset_meta.data) {
                request.add_data_file(name);
            }
            return WG_OK;
        }

        Status load(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Asset>& asset) override final {
            Ref<T> asset_typed;
            WG_CHECKED(load_typed(context, asset_id, result, asset_typed));
            asset = asset_typed.template as<Asset>();
            return WG_OK;
        }

        Status unload(Asset* asset) override final {
            return unload_typed(dynamic_cast<T*>(asset));
        }
    };

}// namespace wmoge