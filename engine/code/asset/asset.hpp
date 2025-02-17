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

#include "core/buffered_vector.hpp"
#include "core/flat_set.hpp"
#include "core/string_id.hpp"
#include "core/uuid.hpp"
#include "core/weak_ref.hpp"
#include "rtti/traits.hpp"

#include <cstddef>
#include <functional>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>
#include <vector>

namespace wmoge {

    /**
     * @class AssetId 
     * @brief Class to track and access asset by its id
    */
    class AssetId {
    public:
        AssetId() = default;
        AssetId(const std::string& id);
        AssetId(const Strid& id);

        bool operator==(const AssetId& other) const { return m_name == other.m_name; }
        bool operator!=(const AssetId& other) const { return m_name != other.m_name; }
        bool operator<(const AssetId& other) const { return m_name < other.m_name; }

        operator bool() const { return is_empty(); }
        operator Strid() const { return sid(); }

        [[nodiscard]] const Strid&       sid() const { return m_name; }
        [[nodiscard]] const std::string& str() const { return m_name.str(); }
        [[nodiscard]] bool               is_empty() const { return m_name.empty(); }
        [[nodiscard]] std::size_t        hash() const { return m_name.hash(); }

        friend Status tree_read(IoContext& context, IoTree& tree, AssetId& id);
        friend Status tree_write(IoContext& context, IoTree& tree, const AssetId& id);
        friend Status stream_read(IoContext& context, IoStream& stream, AssetId& id);
        friend Status stream_write(IoContext& context, IoStream& stream, const AssetId& id);

    private:
        Strid m_name;
    };

    static_assert(std::is_trivially_destructible_v<AssetId>, "id must be trivial as ptr or int");

    inline std::ostream& operator<<(std::ostream& stream, const AssetId& id) {
        stream << id.sid();
        return stream;
    }

    WG_RTTI_DECL(AssetId, "assetid");

    /**
     * @class Asset
     * @brief Base class for any engine asset
    */
    class Asset : public WeakRefCnt<Asset, RttiObject> {
    public:
        WG_RTTI_CLASS(Asset, RttiObject);

        void           set_id(AssetId id) { m_id = id; }
        void           set_uuid(UUID uuid) { m_uuid = uuid; }
        const Strid&   get_name() { return m_id.sid(); }
        const AssetId& get_id() { return m_id; }
        const UUID&    get_uuid() { return m_uuid; }

    private:
        AssetId m_id;
        UUID    m_uuid;
    };

    WG_RTTI_CLASS_BEGIN(Asset) {
        WG_RTTI_META_DATA(RttiUiHint("Base class for any engine asset"));
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(m_id, {RttiNoSaveLoad});
        WG_RTTI_FIELD(m_uuid, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

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