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

#include "asset/asset_flags.hpp"
#include "asset/asset_id.hpp"
#include "core/date_time.hpp"
#include "core/flat_map.hpp"
#include "core/flat_set.hpp"
#include "core/uuid.hpp"
#include "rtti/object.hpp"
#include "rtti/traits.hpp"
#include "rtti/type_ref.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class AssetImportError
     * @brief Asset import error reported by importer and saved in import history
     */
    struct AssetImportError {
        WG_RTTI_STRUCT(AssetImportError)

        std::string file;
        std::string message;
    };

    WG_RTTI_STRUCT_BEGIN(AssetImportError) {
        WG_RTTI_FIELD(file, {});
        WG_RTTI_FIELD(message, {});
    }
    WG_RTTI_END;

    /**
     * @class AssetImportEnv
     * @brief Asset import enviroment serialized and re-used for every import action
     */
    struct AssetImportEnv {
        WG_RTTI_STRUCT(AssetImportEnv)

        std::vector<std::pair<std::string, UUID>> file_to_id;
        std::vector<AssetId>                      deps;
        std::vector<AssetImportError>             errors;
    };

    WG_RTTI_STRUCT_BEGIN(AssetImportEnv) {
        WG_RTTI_FIELD(file_to_id, {RttiOptional});
        WG_RTTI_FIELD(deps, {RttiOptional});
        WG_RTTI_FIELD(errors, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class AssetImportArtifactInfo
     * @brief Artifact info created by imported
     */
    struct AssetImportArtifactInfo {
        std::string     name;
        Ref<RttiObject> object;
    };

    /**
     * @class AssetImportAssetInfo
     * @brief Asset info created by importer
     */
    struct AssetImportAssetInfo {
        std::string                          path;
        UUID                                 uuid;
        AssetFlags                           flags;
        RttiRefClass                         cls;
        RttiRefClass                         loader;
        std::vector<UUID>                    deps;
        std::vector<AssetImportArtifactInfo> artifacts;
    };

    /**
     * @class AssetImportResult
     * @brief Asset importing results
     */
    struct AssetImportResult {
        AssetImportAssetInfo              main;
        std::vector<AssetImportAssetInfo> children;
        std::vector<std::string>          sources;
        DateTime                          timestamp;
        flat_map<std::string, UUID>       file_to_id;
        flat_set<AssetId>                 deps;
        std::vector<AssetImportError>     errors;
    };

}// namespace wmoge