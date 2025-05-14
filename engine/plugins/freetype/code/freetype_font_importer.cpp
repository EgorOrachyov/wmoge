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

#include "freetype_font_importer.hpp"

#include "asset/asset_native_loader.hpp"
#include "freetype_font.hpp"
#include "grc/font.hpp"
#include "grc/font_import_settings.hpp"
#include "grc/texture.hpp"
#include "grc/texture_builder.hpp"
#include "grc/texture_loader.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status FreetypeFontImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"ttf"};
        return WG_OK;
    }

    Status FreetypeFontImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        auto& preset    = presets.emplace_back();
        preset.name     = "default";
        preset.settings = make_ref<FontImportSettings>();
        return WG_OK;
    }

    Status FreetypeFontImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status FreetypeFontImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("FreetypeFontImporter::import");

        Ref<FontImportSettings> font_settings = settings.cast<FontImportSettings>();
        if (!font_settings) {
            WG_LOG_ERROR("failed to get valid font settings for " << path);
            return StatusCode::InvalidData;
        }

        std::vector<std::uint8_t> font_data;
        if (!context.get_file_system()->read_file(path, font_data)) {
            WG_LOG_ERROR("failed to read font file " << path);
            return StatusCode::FailedRead;
        }

        FontDesc    font_desc;
        TextureDesc bitmap_desc;

        FreetypeFontLoader loader;
        if (!loader.load(path, font_desc, bitmap_desc, font_data, font_settings->height, font_settings->glyphs_in_row)) {
            WG_LOG_ERROR("failed to process ttf font " << path);
            return StatusCode::Error;
        }

        const std::string bitmap_path = path + ".bitmap";
        const UUID        bitmap_uuid = context.alloc_asset_uuid(bitmap_path);

        Ref<Texture2d> bitmap = make_ref<Texture2d>(std::move(bitmap_desc));
        bitmap->set_id(bitmap_uuid);

        font_desc.texture = bitmap;
        Ref<Font> font    = make_ref<Font>(font_desc);

        AssetImportAssetInfo bitmap_asset;
        bitmap_asset.uuid   = bitmap_uuid;
        bitmap_asset.path   = bitmap_path;
        bitmap_asset.flags  = {AssetFlag::Hidden};
        bitmap_asset.cls    = Texture2d::get_class_static();
        bitmap_asset.loader = Texture2dLoader::get_class_static();
        bitmap_asset.artifacts.push_back(AssetImportArtifactInfo{bitmap_path, bitmap});

        AssetImportAssetInfo font_asset;
        font_asset.uuid   = context.alloc_asset_uuid(path);
        font_asset.path   = path;
        font_asset.cls    = Font::get_class_static();
        font_asset.loader = AssetNativeLoader::get_class_static();
        font_asset.artifacts.push_back(AssetImportArtifactInfo{path, font});
        font_asset.deps.push_back(bitmap_uuid);

        context.set_main_asset(std::move(font_asset));
        context.add_child_asset(std::move(bitmap_asset));
        context.add_source(path);
        return WG_OK;
    }

}// namespace wmoge