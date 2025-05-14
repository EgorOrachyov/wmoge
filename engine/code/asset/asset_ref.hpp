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
#include "core/ref.hpp"
#include "core/uuid.hpp"
#include "io/stream.hpp"
#include "io/tree.hpp"

#include <cassert>
#include <optional>
#include <type_traits>

namespace wmoge {

    /**
     * @class AssetRef
     * @brief Aux box to store asset ref and serialize/deserialize it automatically to and from files
     */
    template<typename T>
    class AssetRef {
    public:
        static_assert(std::is_base_of_v<Asset, T>, "Must be an asset");

        AssetRef() = default;

        AssetRef(Ref<T> ptr)
            : m_asset_ref(std::move(ptr)) {
        }

        operator Ref<T>() const {
            return m_asset_ref;
        }
        operator bool() const {
            return m_asset_ref;
        }
        T* operator->() const {
            return m_asset_ref.get();
        }

        [[nodiscard]] bool          is_not_empty() const { return m_asset_ref; }
        [[nodiscard]] bool          is_empty() const { return !m_asset_ref; }
        [[nodiscard]] const Ref<T>& get_ref() const { return m_asset_ref; }

    private:
        Ref<T> m_asset_ref;
    };

    /**
     * @class AssetRefParser
     * @brief Helper class to parse asset refs
     */
    class AssetRefParser {
    public:
        static Status parse_from_tree(IoContext& context, IoTree& tree, Ref<Asset>& asset_ref);
        static Status parse_from_stream(IoContext& context, IoStream& stream, Ref<Asset>& asset_ref);
    };

    template<typename T>
    Status tree_read(IoContext& context, IoTree& tree, AssetRef<T>& ref) {
        Ref<Asset> asset_ref;
        WG_CHECKED(AssetRefParser::parse_from_tree(context, tree, asset_ref));
        if (asset_ref) {
            Ref<T> asset_ref_t = asset_ref.cast<T>();
            if (!asset_ref_t) {
                return StatusCode::InvalidData;
            }
            ref = std::move(AssetRef<T>(std::move(asset_ref_t)));
        }
        return WG_OK;
    }

    template<typename T>
    Status tree_write(IoContext& context, IoTree& tree, const AssetRef<T>& ref) {
        AssetId asset_id = ref ? ref->get_id() : AssetId();
        WG_TREE_MAP(tree);
        WG_TREE_WRITE_AS(context, tree, "id", asset_id);
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, AssetRef<T>& ref) {
        Ref<Asset> asset_ref;
        WG_CHECKED(AssetRefParser::parse_from_stream(context, stream, asset_ref));
        if (asset_ref) {
            Ref<T> asset_ref_t = asset_ref.cast<T>();
            if (!asset_ref_t) {
                return StatusCode::InvalidData;
            }
            ref = std::move(AssetRef<T>(std::move(asset_ref_t)));
        }
        return WG_OK;
    }

    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const AssetRef<T>& ref) {
        AssetId asset_id = ref ? ref->get_id() : AssetId();
        WG_ARCHIVE_WRITE(context, stream, asset_id);
        return WG_OK;
    }

    template<typename T>
    struct RttiTypeOf<AssetRef<T>, typename std::enable_if<std::is_base_of_v<Asset, T>>::type> {
        static Strid name() {
            return SID(std::string("asset<") + rtti_type<T>()->get_str() + ">");
        }
        static Ref<RttiType> make() {
            return make_ref<RttiTypeRefT<AssetRef<T>, T, RttiTypeAssetRef>>(name());
        }
    };

}// namespace wmoge