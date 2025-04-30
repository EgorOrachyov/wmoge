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
#include "asset/asset_meta.hpp"
#include "core/array_view.hpp"
#include "core/flat_map.hpp"
#include "core/ioc_container.hpp"
#include "io/context.hpp"
#include "rtti/traits.hpp"

namespace wmoge {

    /**
     * @class AssetLoadRequest
     * @brief Request files to load for an asset load
     */
    struct AssetLoadRequest {
        flat_map<Strid, std::string> data_files;

        void  add_data_file(const Strid& name);
        void  add_data_file(const Strid& name, const std::string& path);
        Strid get_data_file(Strid tag) const;
    };

    /**
     * @class AssetLoadResult
     * @brief Loaded files requested by an asset loader 
     */
    struct AssetLoadResult {
        flat_map<Strid, array_view<const std::uint8_t>> data_files;

        void                           add_data_file(Strid tag, array_view<const std::uint8_t> data);
        array_view<const std::uint8_t> get_data_file(Strid tag) const;
    };

    /**
     * @class AssetLoadContext
     * @brief Context passed to the loader
     */
    struct AssetLoadContext {
        IocContainer* ioc;
        IoContext     io_context;
        AssetMeta     asset_meta;
    };

    /**
     * @class AssetLoader
     * @brief Class responsible for loading or asset(s) in a specific format
     */
    class AssetLoader : public RttiObject {
    public:
        WG_RTTI_CLASS(AssetLoader, RttiObject);

        virtual Status fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) { return StatusCode::NotImplemented; };
        virtual Status load(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Asset>& asset) { return StatusCode::NotImplemented; }
        virtual Status unload(Asset* asset) { return StatusCode::Ok; }
    };

    WG_RTTI_CLASS_BEGIN(AssetLoader) {
        WG_RTTI_META_DATA({RttiUiHint("Interface for an asset loader to implement custom loading")});
    }
    WG_RTTI_END;

}// namespace wmoge