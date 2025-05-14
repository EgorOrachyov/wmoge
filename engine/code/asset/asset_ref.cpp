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

#include "asset_ref.hpp"

#include "asset/asset_cache.hpp"
#include "asset/asset_db.hpp"
#include "asset/asset_resolver.hpp"

namespace wmoge {

    Status AssetRefParser::parse_from_tree(IoContext& context, IoTree& tree, Ref<Asset>& asset_ref) {
        if (tree.node_is_empty()) {
            return WG_OK;
        }
        std::string path;
        AssetId     id;
        WG_TREE_READ_AS_OPT(context, tree, "id", id);
        WG_TREE_READ_AS_OPT(context, tree, "path", path);
        if (!path.empty() && id.is_empty()) {
            auto uuid = context.get<AssetResolver*>()->resolve(path);
            if (!uuid) {
                WG_LOG_ERROR("failed to resolve asset " << path);
                return StatusCode::NoAsset;
            }
            id = *uuid;
        }
        if (id) {
            asset_ref = context.get<AssetCache*>()->try_acquire(id);
            if (!asset_ref) {
                WG_LOG_ERROR("failed to fetch from cache asset " << id);
                return StatusCode::NoAsset;
            }
        }
        return WG_OK;
    }

    Status AssetRefParser::parse_from_stream(IoContext& context, IoStream& stream, Ref<Asset>& asset_ref) {
        AssetId asset_id;
        WG_ARCHIVE_READ(context, stream, asset_id);
        if (asset_id) {
            asset_ref = context.get<AssetCache*>()->try_acquire(asset_id);
            if (!asset_ref) {
                WG_LOG_ERROR("failed to fetch from cache asset " << asset_id);
                return StatusCode::NoAsset;
            }
        }
        return WG_OK;
    }
}// namespace wmoge