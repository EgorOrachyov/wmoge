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

#include "asset/asset_importer.hpp"

namespace wmoge {

    /**
     * @class StbimageImageImporter
     * @brief Importer texture2d assets using stb image library
     */
    class StbimageImageImporter : public AssetImporter {
    public:
        WG_RTTI_CLASS(StbimageImageImporter, AssetImporter)

        Status get_file_extensions(std::vector<std::string>& extensions) override;
        Status get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) override;
        Status collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) override;
        Status import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) override;
    };

    WG_RTTI_CLASS_BEGIN(StbimageImageImporter) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge