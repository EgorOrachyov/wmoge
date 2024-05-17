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
#include "core/data.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "io/yaml.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class AssetPak
     * @brief Interface for the package of the assets on disc
     *
     * AssetPak abstracts access to the assets on disk. It provides ability
     * to load a particular asset meta file from a asset name, and allows
     * to read a raw data using path.
     *
     * Internally asset pack can be represented as a wrapper for a file system
     * asset directory, or it can manage a compressed pak of assets on a disk.
     */
    class AssetPak {
    public:
        virtual ~AssetPak()                                                = default;
        virtual std::string get_name() const                               = 0;
        virtual Status      get_meta(const AssetId& name, AssetMeta& meta) = 0;
    };

}// namespace wmoge