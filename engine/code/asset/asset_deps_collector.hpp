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
#include "core/flat_set.hpp"
#include "core/uuid.hpp"
#include "rtti/builtin.hpp"
#include "rtti/struct.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class AssetDepsCollector
     * @brief Collects asset dependencies using rtti information
     */
    class AssetDepsCollector {
    public:
        AssetDepsCollector(Ref<Asset> asset, std::string name);

        Status collect();

        [[nodiscard]] std::vector<UUID> to_vector() const;

    private:
        Status collect(const RttiType* rtti, std::uint8_t* src);
        Status collect_from_optional(const RttiTypeOptional* rtti, std::uint8_t* src);
        Status collect_from_pair(const RttiTypePair* rtti, std::uint8_t* src);
        Status collect_from_struct(const RttiStruct* rtti, std::uint8_t* src);
        Status collect_from_vector(const RttiTypeVector* rtti, std::uint8_t* src);
        Status collect_from_set(const RttiTypeSet* rtti, std::uint8_t* src);
        Status collect_from_map(const RttiTypeMap* rtti, std::uint8_t* src);
        Status collect_from_ref(const RttiTypeRef* rtti, std::uint8_t* src);
        Status collect_from_asset_ref(const RttiTypeAssetRef* rtti, std::uint8_t* src);

    private:
        Ref<Asset>     m_asset;
        std::string    m_name;
        flat_set<UUID> m_asset_refs;
    };

}// namespace wmoge