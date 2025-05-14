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

#include "asset/asset_id.hpp"
#include "core/uuid.hpp"
#include "rtti/traits.hpp"

#include <cstddef>
#include <functional>
#include <ostream>
#include <string>
#include <type_traits>

namespace wmoge {

    /**
     * @class AssetId 
     * @brief Class to track and access asset by its uuid
    */
    class AssetId {
    public:
        AssetId() = default;
        AssetId(const UUID& uuid);

        bool operator==(const AssetId& other) const { return m_uuid == other.m_uuid; }
        bool operator!=(const AssetId& other) const { return m_uuid != other.m_uuid; }

        operator bool() const { return !is_empty(); }
        operator UUID() const { return m_uuid; }

        [[nodiscard]] UUID        uuid() const { return m_uuid; }
        [[nodiscard]] std::string to_str() const { return m_uuid.to_str(); }
        [[nodiscard]] bool        is_empty() const { return m_uuid.is_null(); }
        [[nodiscard]] std::size_t hash() const { return m_uuid.hash(); }

        friend Status tree_read(IoContext& context, IoTree& tree, AssetId& asset_id);
        friend Status tree_write(IoContext& context, IoTree& tree, const AssetId& asset_id);
        friend Status stream_read(IoContext& context, IoStream& stream, AssetId& asset_id);
        friend Status stream_write(IoContext& context, IoStream& stream, const AssetId& asset_id);

    private:
        UUID m_uuid;
    };

    static_assert(std::is_trivially_destructible_v<AssetId>, "id must be trivial as ptr or int");

    inline std::ostream& operator<<(std::ostream& stream, const AssetId& id) {
        stream << id.uuid();
        return stream;
    }

    WG_RTTI_DECL(AssetId, "assetid");

}// namespace wmoge

namespace std {

    template<>
    struct hash<wmoge::AssetId> {
    public:
        std::size_t operator()(const wmoge::AssetId& id) const {
            return id.hash();
        }
    };

}// namespace std