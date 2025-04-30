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

#include "core/date_time.hpp"
#include "core/sha256.hpp"
#include "rtti/traits.hpp"

#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class AssetSourceFile
     * @brief Represent source file info of an asset
    */
    struct AssetSourceFile {
        WG_RTTI_STRUCT(AssetSourceFile);

        std::string file;
        Strid       file_tag;
        Sha256      file_hash;
        DateTime    timestamp;
    };

    WG_RTTI_STRUCT_BEGIN(AssetSourceFile) {
        WG_RTTI_FIELD(file, {RttiOptional, RttiUiHint("Path to original file on disk")});
        WG_RTTI_FIELD(file_tag, {RttiOptional, RttiUiHint("Optional tag to hint file usage")});
        WG_RTTI_FIELD(file_hash, {RttiOptional, RttiUiHint("File content hash")});
        WG_RTTI_FIELD(timestamp, {RttiOptional, RttiUiHint("Time when file was imported")});
    }
    WG_RTTI_END;

    /**
     * @class AssetImportData
     * @brief Data and options for importing an asset
    */
    class AssetImportData : public RttiObject {
    public:
        WG_RTTI_CLASS(AssetImportData, RttiObject);

        AssetImportData()           = default;
        ~AssetImportData() override = default;

        [[nodiscard]] bool has_soruce_files() const { return !source_files.empty(); }
        [[nodiscard]] int  source_files_size() const { return int(source_files.size()); }

        std::vector<AssetSourceFile> source_files;
    };

    WG_RTTI_CLASS_BEGIN(AssetImportData) {
        WG_RTTI_META_DATA({RttiUiName("Asset import data"), RttiUiHint("Asset manager assosiated data to import asset")});
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(source_files, {RttiOptional});
        WG_RTTI_METHOD(has_soruce_files, {}, {});
        WG_RTTI_METHOD(source_files_size, {}, {});
    }
    WG_RTTI_END;

}// namespace wmoge