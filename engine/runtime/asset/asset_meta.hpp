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
#include "core/string_id.hpp"
#include "core/uuid.hpp"
#include "rtti/traits.hpp"

#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class AssetMetaFile
     * @brief Structure for AssetMeta info stored as `.asset` file in file system
     */
    struct AssetMetaFile {
        WG_RTTI_STRUCT(AssetMetaFile);

        static constexpr char FILE_EXTENSION[] = ".asset";

        UUID                   uuid;
        Strid                  rtti;
        Strid                  loader;
        buffered_vector<Strid> deps;
        std::string            description;
        Ref<AssetImportData>   import_data;
    };

    WG_RTTI_STRUCT_BEGIN(AssetMetaFile) {
        WG_RTTI_META_DATA();
        WG_RTTI_FIELD(uuid, {RttiOptional});
        WG_RTTI_FIELD(rtti, {});
        WG_RTTI_FIELD(loader, {});
        WG_RTTI_FIELD(deps, {RttiOptional});
        WG_RTTI_FIELD(description, {RttiOptional});
        WG_RTTI_FIELD(import_data, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class AssetMeta
     * @brief Meta information of a particular asset
     */
    struct AssetMeta {
        WG_RTTI_STRUCT(AssetMeta);

        UUID                   uuid   = UUID();
        class RttiClass*       rtti   = nullptr;
        class AssetPak*        pak    = nullptr;
        class AssetLoader*     loader = nullptr;
        buffered_vector<Strid> deps;
        Ref<AssetImportData>   import_data;
    };

    WG_RTTI_STRUCT_BEGIN(AssetMeta) {
        WG_RTTI_META_DATA();
        WG_RTTI_FIELD(uuid, {RttiOptional});
        WG_RTTI_FIELD(deps, {RttiOptional});
        WG_RTTI_FIELD(import_data, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge