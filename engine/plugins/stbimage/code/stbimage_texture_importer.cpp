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

#include "stbimage_texture_importer.hpp"

#include "grc/image.hpp"
#include "grc/texture_builder.hpp"
#include "grc/texture_import_settings.hpp"
#include "grc/texture_loader.hpp"
#include "io/tree_yaml.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status StbimageTexture2dImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"png", "bmp", "jpg", "jpeg", "gif"};
        return WG_OK;
    }

    Status StbimageTexture2dImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        // default
        {
            auto& preset    = presets.emplace_back();
            preset.name     = "default";
            preset.settings = make_ref<Texture2dImportSettings>();
        }
        // diffuse
        {
            auto& preset    = presets.emplace_back();
            preset.name     = "diffuse";
            preset.settings = make_ref<Texture2dImportSettings>();
        }
        // normal
        {
            auto& preset    = presets.emplace_back();
            preset.name     = "normal";
            preset.settings = make_ref<Texture2dImportSettings>();
        }
        // ao
        {
            auto& preset    = presets.emplace_back();
            preset.name     = "ao";
            preset.settings = make_ref<Texture2dImportSettings>();
        }
        return WG_OK;
    }

    Status StbimageTexture2dImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status StbimageTexture2dImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("StbimageTexture2dImporter::import");

        Ref<Texture2dImportSettings> texture_settings = settings.cast<Texture2dImportSettings>();
        if (!texture_settings) {
            WG_LOG_ERROR("failed to get settings for " << path);
            return StatusCode::InvalidData;
        }

        Ref<Image> source = make_ref<Image>();
        if (!source->load(context.get_file_system(), path, texture_settings->channels)) {
            WG_LOG_ERROR("failed to read image source for " << path);
            return StatusCode::FailedRead;
        }

        TextureDescBuilder builder(path);
        builder.set_image(source, texture_settings->format)
                .set_resize(texture_settings->resizing)
                .set_swizz(texture_settings->swizz)
                .set_sampler(texture_settings->sampling)
                .set_compression(texture_settings->compression)
                .set_flags({TextureFlag::Pooled, TextureFlag::FromDisk});

        TextureDesc desc;
        if (!builder.build_desc_2d(desc)) {
            WG_LOG_ERROR("failed to build texture desc for " << path);
            return StatusCode::Error;
        }

        Ref<Texture2d> texture = make_ref<Texture2d>(std::move(desc));

        context.set_main_asset_simple(texture, path, Texture2dLoader::get_class_static());
        return WG_OK;
    }

    Status StbimageTextureCubeImporter::get_file_extensions(std::vector<std::string>& extensions) {
        extensions = {"texcube"};
        return WG_OK;
    }

    Status StbimageTextureCubeImporter::get_import_presets(const std::string& path, std::vector<AssetImportPreset>& presets) {
        // default
        {
            auto& preset    = presets.emplace_back();
            preset.name     = "default";
            preset.settings = make_ref<TextureCubeImportSettings>();
        }
        return WG_OK;
    }

    Status StbimageTextureCubeImporter::collect_dependencies(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        return WG_OK;
    }

    Status StbimageTextureCubeImporter::import(AssetImportContext& context, const std::string& path, const Ref<AssetImportSettings>& settings) {
        WG_PROFILE_CPU_ASSET("StbimageTextureCubeImporter::import");

        Ref<TextureCubeImportSettings> texture_settings = settings.cast<TextureCubeImportSettings>();
        if (!texture_settings) {
            WG_LOG_ERROR("failed to get settings for " << path);
            return StatusCode::InvalidData;
        }

        TextureCubeFile file;
        {
            IoContext  io_context = context.get_io_context();
            IoYamlTree tree;
            if (!tree.parse_file(context.get_file_system(), path)) {
                WG_LOG_ERROR("failed parse .texcube file " << path);
                return StatusCode::FailedParse;
            }
            if (!tree_read(io_context, tree, file)) {
                WG_LOG_ERROR("failed parse .texcube file " << path);
                return StatusCode::FailedParse;
            }
        }

        std::vector<Ref<Image>> sources;

        auto load_source = [&](const std::string& face_path) {
            auto image = sources.emplace_back(make_ref<Image>());
            if (!image->load(context.get_file_system(), context.resolve_path(face_path), texture_settings->channels)) {
                WG_LOG_ERROR("failed to load source image " << context.resolve_path(face_path) << " for " << path);
                return false;
            }
            return true;
        };

        if (!load_source(file.right) ||
            !load_source(file.left) ||
            !load_source(file.top) ||
            !load_source(file.bottom) ||
            !load_source(file.front) ||
            !load_source(file.back)) {
            return StatusCode::Error;
        }

        TextureDescBuilder builder(path);
        builder.set_images(std::move(sources), texture_settings->format)
                .set_resize(texture_settings->resizing)
                .set_swizz(texture_settings->swizz)
                .set_sampler(texture_settings->sampling)
                .set_compression(texture_settings->compression)
                .set_flags({TextureFlag::Pooled, TextureFlag::FromDisk});

        TextureDesc desc;
        if (!builder.build_desc_cube(desc)) {
            WG_LOG_ERROR("failed to build texture desc for " << path);
            return StatusCode::Error;
        }

        Ref<TextureCube> texture = make_ref<TextureCube>(std::move(desc));

        context.set_main_asset_simple(texture, path, TextureCubeLoader::get_class_static());
        return WG_OK;
    }

}// namespace wmoge