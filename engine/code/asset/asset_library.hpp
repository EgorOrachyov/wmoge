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
#include "asset/asset_artifact.hpp"
#include "core/array_view.hpp"
#include "core/async.hpp"
#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/uuid.hpp"
#include "rtti/type_ref.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class AssetLibraryRecord
     * @brief Information about an asset stored in library 
     */
    struct AssetLibraryRecord {
        RttiRefClass      cls;
        RttiRefClass      loader;
        std::vector<UUID> deps;
        std::vector<UUID> artifacts;
    };

    /**
     * @class AssetLibrary
     * @brief Interface for a library of assets meta information
     */
    class AssetLibrary : public RefCnt {
    public:
        virtual ~AssetLibrary() = default;

        virtual Status resolve_asset(const std::string& asset_name, UUID& asset_id) { return StatusCode::NotImplemented; }
        virtual Status get_asset_info(UUID asset_id, AssetLibraryRecord& asset_info) { return StatusCode::NotImplemented; }
        virtual Status get_artifact_info(UUID artifact_id, AssetArtifact& artifact_info) { return StatusCode::NotImplemented; }
        virtual bool   has_asset(UUID asset_id) const { return false; }
        virtual bool   has_artifact(UUID artifact_id) const { return false; }
        virtual Async  read_artifact(UUID artifact_id, array_view<std::uint8_t> buffer, Ref<RttiObject> artifact) { return Async(); }
    };

}// namespace wmoge