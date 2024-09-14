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
#include "asset/asset_manager.hpp"
#include "io/property_tree.hpp"
#include "io/stream.hpp"

#include <cassert>
#include <optional>
#include <type_traits>

namespace wmoge {

    /**
     * @class AssetRef
     * @brief Aux box to store asset ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class AssetRef : public Ref<T> {
    public:
        static_assert(std::is_base_of_v<Asset, T>, "Must be an asset");

        AssetRef() = default;
        AssetRef(Ref<T> ptr) : Ref<T>(std::move(ptr)) {}
    };

    /**
     * @class AssetRefWeak
     * @brief Aux box to store asset ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class AssetRefWeak : public AssetId {
    public:
        static_assert(std::is_base_of_v<Asset, T>, "Must be an asset");

        AssetRefWeak() = default;
        AssetRefWeak(const AssetId& id) : AssetId(id) {}
        AssetRefWeak(const AssetRef<T>& ref) : AssetId(ref ? ref->get_id() : AssetId()) {}
    };

    template<typename T>
    Status tree_read(IoContext& context, IoPropertyTree& tree, AssetRef<T>& ref) {
        AssetId id;
        WG_TREE_READ(context, tree, id);
        Ref<T> ptr = context.get_asset_manager()->find(id).cast<T>();
        if (!ptr) {
            return StatusCode::NoAsset;
        }
        ref = AssetRef<T>(ptr);
        return WG_OK;
    }

    template<typename T>
    Status tree_write(IoContext& context, IoPropertyTree& tree, const AssetRef<T>& ref) {
        assert(ref);
        if (!ref) {
            return StatusCode::NoAsset;
        }
        WG_TREE_WRITE(context, tree, ref->get_id());
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, AssetRef<T>& ref) {
        AssetId id;
        WG_ARCHIVE_READ(context, stream, id);
        Ref<T> ptr = context.get_asset_manager()->load(id).cast<T>();
        if (!ptr) {
            return StatusCode::NoAsset;
        }
        ref = AssetRef<T>(ptr);
        return WG_OK;
    }

    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const AssetRef<T>& ref) {
        assert(ref);
        if (!ref) {
            return StatusCode::NoAsset;
        }
        WG_ARCHIVE_WRITE(context, stream, ref->get_id());
        return WG_OK;
    }

    template<typename T>
    Status tree_read(IoContext& context, IoPropertyTree& tree, AssetRefWeak<T>& ref) {
        AssetId id;
        WG_TREE_READ(context, tree, id);
        ref = AssetRefWeak<T>(id);
        return WG_OK;
    }

    template<typename T>
    Status tree_write(IoContext& context, IoPropertyTree& tree, const AssetRefWeak<T>& ref) {
        AssetId id = ref;
        WG_TREE_WRITE(context, tree, id);
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, AssetRefWeak<T>& ref) {
        AssetId id;
        WG_ARCHIVE_READ(context, stream, id);
        ref = AssetRefWeak<T>(id);
        return WG_OK;
    }

    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const AssetRefWeak<T>& ref) {
        AssetId id = ref;
        WG_ARCHIVE_WRITE(context, stream, id);
        return WG_OK;
    }

}// namespace wmoge