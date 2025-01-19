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

#include "asset.hpp"

#include <algorithm>
#include <cassert>
#include <limits>

namespace wmoge {

    Status tree_read(IoContext& context, IoTree& tree, AssetId& id) {
        WG_TREE_READ(context, tree, id.m_name);
        return WG_OK;
    }
    Status tree_write(IoContext& context, IoTree& tree, const AssetId& id) {
        WG_TREE_WRITE(context, tree, id.m_name);
        return WG_OK;
    }
    Status stream_read(IoContext& context, IoStream& stream, AssetId& id) {
        WG_ARCHIVE_READ(context, stream, id.m_name);
        return WG_OK;
    }
    Status stream_write(IoContext& context, IoStream& stream, const AssetId& id) {
        WG_ARCHIVE_WRITE(context, stream, id.m_name);
        return WG_OK;
    }

    AssetId::AssetId(const std::string& id) {
        m_name = SID(id);
    }
    AssetId::AssetId(const Strid& id) {
        m_name = id;
    }

}// namespace wmoge