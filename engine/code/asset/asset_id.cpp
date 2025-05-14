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

#include "asset_id.hpp"

#include "asset/asset_resolver.hpp"

#include <algorithm>
#include <cassert>
#include <limits>
#include <string>

namespace wmoge {

    Status tree_read(IoContext& context, IoTree& tree, AssetId& asset_id) {
        if (tree.node_is_empty()) {
            return WG_OK;
        }
        UUID        id;
        std::string path;
        WG_TREE_READ_AS_OPT(context, tree, "id", id);
        WG_TREE_READ_AS_OPT(context, tree, "path", path);
        if (!path.empty() && id.is_null()) {
            auto resolve = context.get<AssetResolver*>()->resolve(path);
            if (!resolve) {
                return StatusCode::NoAsset;
            }
            id = *resolve;
        }
        asset_id = id;
        return WG_OK;
    }

    Status tree_write(IoContext& context, IoTree& tree, const AssetId& asset_id) {
        UUID        id = asset_id.m_uuid;
        std::string path;
        if (context.has<AssetResolver*>()) {
            auto resolve = context.get<AssetResolver*>()->resolve(id);
            if (resolve) {
                path = *resolve;
            }
        }
        WG_TREE_MAP(tree);
        WG_TREE_WRITE_AS(context, tree, "id", id);
        WG_TREE_WRITE_AS(context, tree, "path", path);
        return WG_OK;
    }

    Status stream_read(IoContext& context, IoStream& stream, AssetId& asset_id) {
        WG_ARCHIVE_READ(context, stream, asset_id.m_uuid);
        return WG_OK;
    }

    Status stream_write(IoContext& context, IoStream& stream, const AssetId& asset_id) {
        WG_ARCHIVE_WRITE(context, stream, asset_id.m_uuid);
        return WG_OK;
    }

    AssetId::AssetId(const UUID& uuid) {
        m_uuid = uuid;
    }

}// namespace wmoge