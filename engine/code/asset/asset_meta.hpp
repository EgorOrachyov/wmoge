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
#include "asset/asset_import_data.hpp"
#include "core/buffered_vector.hpp"
#include "core/date_time.hpp"
#include "core/sha256.hpp"
#include "core/string_id.hpp"
#include "core/uuid.hpp"
#include "rtti/traits.hpp"

#include <cinttypes>
#include <optional>
#include <vector>

namespace wmoge {

    /** @brief Asset data compression on dics */
    enum class AssetCompressionMode {
        None = 0,
        LZ4,
        Zip
    };

    /**
     * @class AssetDataDesc
     * @brief Describes asset associated data stored in asset system
     */
    struct AssetDataMeta {
        WG_RTTI_STRUCT(AssetDataMeta);

        UUID                 uuid;
        Sha256               hash;
        std::size_t          size            = 0;
        std::size_t          size_compressed = 0;
        AssetCompressionMode compression     = AssetCompressionMode::None;
    };

    WG_RTTI_STRUCT_BEGIN(AssetDataMeta) {
        WG_RTTI_META_DATA();
        WG_RTTI_FIELD(uuid, {RttiOptional});
        WG_RTTI_FIELD(hash, {RttiOptional});
        WG_RTTI_FIELD(size, {RttiOptional});
        WG_RTTI_FIELD(size_compressed, {RttiOptional});
        WG_RTTI_FIELD(compression, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class AssetMeta
     * @brief Describes asset information stored in asset system
     */
    struct AssetMeta {
        WG_RTTI_STRUCT(AssetMeta);

        UUID                 uuid;
        int                  version = 0;
        Strid                rtti;
        Strid                loader;
        Strid                importer;
        std::vector<Strid>   deps;
        std::vector<Strid>   data;
        std::string          description;
        Ref<AssetImportData> import_data;
    };

    WG_RTTI_STRUCT_BEGIN(AssetMeta) {
        WG_RTTI_META_DATA();
        WG_RTTI_FIELD(uuid, {RttiOptional});
        WG_RTTI_FIELD(version, {RttiOptional});
        WG_RTTI_FIELD(rtti, {});
        WG_RTTI_FIELD(loader, {});
        WG_RTTI_FIELD(importer, {RttiOptional});
        WG_RTTI_FIELD(deps, {RttiOptional});
        WG_RTTI_FIELD(data, {RttiOptional});
        WG_RTTI_FIELD(description, {RttiOptional});
        WG_RTTI_FIELD(import_data, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge