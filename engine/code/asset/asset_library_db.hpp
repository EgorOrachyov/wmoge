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

#include "asset/asset_library.hpp"

namespace wmoge {
    class AssetDb;
    class AssetResolver;
    class AssetArtifactCache;
}// namespace wmoge

namespace wmoge {

    /**
     * @class AssetLibraryAssetDb
     * @brief Asset library to load assets from asset db and artifact cache in dev builds
     */
    class AssetLibraryAssetDb final : public AssetLibrary {
    public:
        AssetLibraryAssetDb(AssetDb*            asset_db,
                            AssetResolver*      asset_resolver,
                            AssetArtifactCache* artifact_cache);

        Status resolve_asset(const std::string& asset_name, UUID& asset_id) override;
        Status get_asset_info(UUID asset_id, AssetLibraryRecord& asset_info) override;
        Status get_artifact_info(UUID artifact_id, AssetArtifact& artifact_info) override;
        bool   has_asset(UUID asset_id) const override;
        bool   has_artifact(UUID artifact_id) const override;
        Async  read_artifact(UUID artifact_id, array_view<std::uint8_t> buffer, Ref<RttiObject> artifact) override;

    private:
        AssetDb*            m_asset_db       = nullptr;
        AssetResolver*      m_asset_resolver = nullptr;
        AssetArtifactCache* m_artifact_cache = nullptr;
    };

}// namespace wmoge