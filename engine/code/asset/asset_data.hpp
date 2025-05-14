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

#include "asset/asset_artifact.hpp"
#include "asset/asset_flags.hpp"
#include "asset/asset_import_env.hpp"
#include "asset/asset_import_settings.hpp"
#include "core/date_time.hpp"
#include "core/sha256.hpp"
#include "core/uuid.hpp"
#include "rtti/object.hpp"
#include "rtti/traits.hpp"
#include "rtti/type_ref.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class AssetSource
     * @brief Asset source file info stored for re-import on changes
     */
    struct AssetSource {
        WG_RTTI_STRUCT(AssetSource)

        std::string path;
        Sha256      hash;
        DateTime    timestamp;
    };

    WG_RTTI_STRUCT_BEGIN(AssetSource) {
        WG_RTTI_FIELD(path, {});
        WG_RTTI_FIELD(hash, {});
        WG_RTTI_FIELD(timestamp, {});
    }
    WG_RTTI_END;

    /**
     * @class AssetData
     * @brief Asset info stored in db and required for re-import, cooking, packaging, and loading asset
     */
    struct AssetData {
        WG_RTTI_STRUCT(AssetData)

        std::string                path;
        AssetFlags                 flags;
        UUID                       uuid;
        UUID                       parent;
        std::vector<UUID>          children;
        std::vector<UUID>          deps;
        std::vector<AssetArtifact> artifacts;
        std::vector<AssetSource>   sources;
        RttiRefClass               cls;
        RttiRefClass               loader;
        RttiRefClass               importer;
        AssetImportEnv             import_env;
        Ref<AssetImportSettings>   import_settings;
        DateTime                   timestamp;
    };

    WG_RTTI_STRUCT_BEGIN(AssetData) {
        WG_RTTI_FIELD(path, {});
        WG_RTTI_FIELD(flags, {});
        WG_RTTI_FIELD(uuid, {});
        WG_RTTI_FIELD(parent, {});
        WG_RTTI_FIELD(children, {});
        WG_RTTI_FIELD(deps, {});
        WG_RTTI_FIELD(artifacts, {});
        WG_RTTI_FIELD(sources, {});
        WG_RTTI_FIELD(cls, {});
        WG_RTTI_FIELD(loader, {});
        WG_RTTI_FIELD(importer, {});
        WG_RTTI_FIELD(import_env, {});
        WG_RTTI_FIELD(import_settings, {});
        WG_RTTI_FIELD(timestamp, {});
    }
    WG_RTTI_END;

}// namespace wmoge