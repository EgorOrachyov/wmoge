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
#include "asset/asset_meta.hpp"
#include "core/async.hpp"
#include "core/data.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "io/tree.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class AssetLibrary
     * @brief Interface for a library of assets meta information
     */
    class AssetLibrary {
    public:
        virtual ~AssetLibrary() = default;

        virtual std::string get_name() const                                             = 0;
        virtual bool        has_asset(const AssetId& name)                               = 0;
        virtual Status      find_asset_meta(const AssetId& name, AssetMeta& meta)        = 0;
        virtual Status      find_asset_data_meta(const Strid& name, AssetDataMeta& meta) = 0;
        virtual Async       read_data(const Strid& name, array_view<std::uint8_t> data)  = 0;
    };

}// namespace wmoge