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

#include "stbimage_image_importer.hpp"

#include "asset/asset_native_loader.hpp"
#include "grc/image.hpp"
#include "grc/image_import_settings.hpp"
#include "platform/file_system.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status StbimageImageImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"png", "bmp", "jpg", "jpeg", "gif"};
        return WG_OK;
    }

    Status StbimageImageImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        auto& preset    = presets.emplace_back();
        preset.name     = "default";
        preset.settings = make_ref<ImageImportSettings>();
        return WG_OK;
    }

    Status StbimageImageImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status StbimageImageImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("StbimageImageImporter::import");

        Ref<ImageImportSettings> image_settings = settings.cast<ImageImportSettings>();
        if (!image_settings) {
            WG_LOG_ERROR("failed to get valid image settings for " << path);
            return StatusCode::InvalidData;
        }

        Ref<Image> image = make_ref<Image>();
        if (!image->load(context.get_file_system(), path, image_settings->channels)) {
            WG_LOG_ERROR("failed load image from " << path);
            return StatusCode::FailedRead;
        }

        context.set_main_asset_simple(image, path);
        return WG_OK;
    }

}// namespace wmoge