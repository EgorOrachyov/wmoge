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

#include "asset_deps_collector.hpp"

#include "asset/asset_ref.hpp"
#include "core/string_utils.hpp"
#include "rtti/class.hpp"

#include <algorithm>

namespace wmoge {

    AssetDepsCollector::AssetDepsCollector(Ref<Asset> asset, std::string name)
        : m_asset(std::move(asset)),
          m_name(std::move(name)) {
    }

    Status AssetDepsCollector::collect() {
        RttiClass* rtti_class = m_asset->get_class();
        return collect_from_struct(rtti_class, (std::uint8_t*) m_asset.get());
    }

    std::vector<UUID> AssetDepsCollector::to_vector() const {
        std::vector<UUID> uuids(m_asset_refs.size());
        std::copy(m_asset_refs.begin(), m_asset_refs.end(), uuids.begin());
        return std::move(uuids);
    }

    Status AssetDepsCollector::collect(const RttiType* rtti, std::uint8_t* src) {
        switch (rtti->get_archetype()) {
            case RttiArchetype::Optional:
                return collect_from_optional((const RttiTypeOptional*) rtti, src);
            case RttiArchetype::Pair:
                return collect_from_pair((const RttiTypePair*) rtti, src);
            case RttiArchetype::Ref:
                return collect_from_ref((const RttiTypeRef*) rtti, src);
            case RttiArchetype::AssetRef:
                return collect_from_asset_ref((const RttiTypeAssetRef*) rtti, src);
            case RttiArchetype::Vector:
                return collect_from_vector((const RttiTypeVector*) rtti, src);
            case RttiArchetype::Set:
                return collect_from_set((const RttiTypeSet*) rtti, src);
            case RttiArchetype::Map:
                return collect_from_map((const RttiTypeMap*) rtti, src);
            case RttiArchetype::Struct:
            case RttiArchetype::Class:
                return collect_from_struct((const RttiStruct*) rtti, src);

            case RttiArchetype::Fundamental:
            case RttiArchetype::Vec:
            case RttiArchetype::Mask:
            case RttiArchetype::Bitset:
            case RttiArchetype::Function:
            case RttiArchetype::Enum:
            default:
                return WG_OK;
        }
    }

    Status AssetDepsCollector::collect_from_optional(const RttiTypeOptional* rtti, std::uint8_t* src) {
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->visit(src, [&](const void* value) {
            return collect(value_type, (std::uint8_t*) value);
        }));
        return WG_OK;
    }

    Status AssetDepsCollector::collect_from_pair(const RttiTypePair* rtti, std::uint8_t* src) {
        const RttiType* key_type   = rtti->get_key_type();
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->visit(src, [&](const void* key, const void* value) {
            WG_CHECKED(collect(key_type, (std::uint8_t*) key));
            WG_CHECKED(collect(value_type, (std::uint8_t*) value));
            return WG_OK;
        }));
        return WG_OK;
    }

    Status AssetDepsCollector::collect_from_struct(const RttiStruct* rtti, std::uint8_t* src) {
        for (const RttiField& field : rtti->get_fields()) {
            const RttiType* field_type   = field.get_type();
            std::size_t     field_offset = field.get_byte_offset();

            WG_CHECKED(collect(field_type, src + field_offset));
        }
        return WG_OK;
    }

    Status AssetDepsCollector::collect_from_vector(const RttiTypeVector* rtti, std::uint8_t* src) {
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->iterate(src, [&](const void* elem) {
            return collect(value_type, (std::uint8_t*) elem);
        }));
        return WG_OK;
    }

    Status AssetDepsCollector::collect_from_set(const RttiTypeSet* rtti, std::uint8_t* src) {
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->iterate(src, [&](const void* elem) {
            return collect(value_type, (std::uint8_t*) elem);
        }));
        return WG_OK;
    }

    Status AssetDepsCollector::collect_from_map(const RttiTypeMap* rtti, std::uint8_t* src) {
        const RttiType* key_type   = rtti->get_key_type();
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->iterate(src, [&](const void* key, const void* value) {
            WG_CHECKED(collect(key_type, (std::uint8_t*) key));
            WG_CHECKED(collect(value_type, (std::uint8_t*) value));
            return WG_OK;
        }));
        return WG_OK;
    }

    Status AssetDepsCollector::collect_from_ref(const RttiTypeRef* rtti, std::uint8_t* src) {
        const Ref<RttiObject>& as_ptr = *((Ref<RttiObject>*) src);
        if (!as_ptr) {
            return WG_OK;
        }
        return collect(as_ptr->get_class(), (std::uint8_t*) as_ptr.get());
    }

    Status AssetDepsCollector::collect_from_asset_ref(const RttiTypeAssetRef* rtti, std::uint8_t* src) {
        const Ref<RttiObject>& as_ptr = *((Ref<RttiObject>*) src);
        if (!as_ptr) {
            return WG_OK;
        }
        Ref<Asset> asset = as_ptr.cast<Asset>();
        if (asset->get_id()) {
            m_asset_refs.insert(asset->get_id());
        }
        return WG_OK;
    }

}// namespace wmoge