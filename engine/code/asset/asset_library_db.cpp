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

#include "asset_library_db.hpp"

#include "asset/asset_artifact_cache.hpp"
#include "asset/asset_db.hpp"
#include "asset/asset_resolver.hpp"

namespace wmoge {

    AssetLibraryAssetDb::AssetLibraryAssetDb(AssetDb*            asset_db,
                                             AssetResolver*      asset_resolver,
                                             AssetArtifactCache* artifact_cache)
        : m_asset_db(asset_db),
          m_asset_resolver(asset_resolver),
          m_artifact_cache(artifact_cache) {
    }

    Status AssetLibraryAssetDb::resolve_asset(const std::string& asset_name, UUID& asset_id) {
        auto resolve = m_asset_resolver->resolve(asset_name);
        if (resolve) {
            asset_id = *resolve;
            return WG_OK;
        }
        return StatusCode::NoAsset;
    }

    Status AssetLibraryAssetDb::get_asset_info(UUID asset_id, AssetLibraryRecord& asset_info) {
        return m_asset_db->get_asset_loading_info(asset_id, asset_info.cls, asset_info.loader, asset_info.deps, asset_info.artifacts);
    }

    Status AssetLibraryAssetDb::get_artifact_info(UUID artifact_id, AssetArtifact& artifact_info) {
        return m_artifact_cache->get_info(artifact_id, artifact_info);
    }

    bool AssetLibraryAssetDb::has_asset(UUID asset_id) const {
        return m_asset_db->has_asset(asset_id);
    }

    bool AssetLibraryAssetDb::has_artifact(UUID artifact_id) const {
        return m_artifact_cache->has(artifact_id);
    }

    Async AssetLibraryAssetDb::read_artifact(UUID artifact_id, array_view<std::uint8_t> buffer, Ref<RttiObject> artifact) {
        return m_artifact_cache->read(artifact_id, buffer, artifact);
    }

}// namespace wmoge
