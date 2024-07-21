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

#include "texture_asset_loader.hpp"

#include "asset/texture_import_data.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/image.hpp"
#include "grc/texture.hpp"
#include "grc/texture_resize.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    Status Texture2dAssetLoader::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& asset) {
        WG_AUTO_PROFILE_ASSET("Texture2dAssetLoader::load");

        Ref<Texture2dImportData> import_data = meta.import_data.cast<Texture2dImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << name);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << name);
            return StatusCode::InvalidData;
        }

        Ref<Image> source_image = make_ref<Image>();

        if (!source_image->load(import_data->source_files[0].file, import_data->channels)) {
            WG_LOG_ERROR("failed to load source image " << import_data->source_files[0].file);
            return StatusCode::FailedRead;
        }

        if (!TexResize::resize(import_data->resizing, *source_image)) {
            WG_LOG_ERROR("failed to resize source image " << import_data->source_files[0].file);
            return StatusCode::FailedResize;
        }

        Ref<Texture2d> texture = make_ref<Texture2d>(
                import_data->format,
                source_image->get_width(),
                source_image->get_height());

        if (!texture) {
            WG_LOG_ERROR("Failed to instantiate texture " << name);
            return StatusCode::FailedInstantiate;
        }

        asset = texture;
        asset->set_name(name);
        asset->set_import_data(meta.import_data);

        texture->set_source_images({source_image});
        texture->set_sampler_from_desc(import_data->sampling);
        texture->set_compression(import_data->compression);

        if (import_data->mipmaps) {
            if (!texture->generate_mips()) {
                WG_LOG_ERROR("failed to gen mip chain for " << name);
                return StatusCode::Error;
            }
        }
        if (import_data->compression.format != TexCompressionFormat::Unknown) {
            if (!texture->generate_compressed_data()) {
                WG_LOG_ERROR("failed to compress data for " << name);
                return StatusCode::Error;
            }
        }
        if (!texture->generate_gfx_resource()) {
            WG_LOG_ERROR("failed create gfx asset for " << name);
            return StatusCode::Error;
        }

        return WG_OK;
    }

    Status TextureCubeAssetLoader::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& asset) {
        WG_AUTO_PROFILE_ASSET("TextureCubeAssetLoader::load");

        Ref<TextureCubeImportData> import_data = meta.import_data.cast<TextureCubeImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << name);
            return StatusCode::InvalidData;
        }
        if (import_data->source_files_size() < 6) {
            WG_LOG_ERROR("not enough source files " << name);
            return StatusCode::InvalidData;
        }

        std::vector<Ref<Image>> source_images;

        auto load_source = [&](const std::string& path) {
            auto image = source_images.emplace_back(make_ref<Image>());
            if (!image->load(path, import_data->channels)) {
                WG_LOG_ERROR("failed to load source image " << path);
                return false;
            }
            image->set_name(SID(path));
            return true;
        };

        // right, left, top, bottom, front, back

        if (!load_source(import_data->source_files[0].file) ||
            !load_source(import_data->source_files[1].file) ||
            !load_source(import_data->source_files[2].file) ||
            !load_source(import_data->source_files[3].file) ||
            !load_source(import_data->source_files[4].file) ||
            !load_source(import_data->source_files[5].file)) {
            return StatusCode::FailedRead;
        }

        for (Ref<Image>& source_image : source_images) {
            if (!TexResize::resize(import_data->resizing, *source_image)) {
                WG_LOG_ERROR("failed to resize source image " << source_image->get_name());
                return StatusCode::FailedResize;
            }
        }

        Ref<TextureCube> texture = make_ref<TextureCube>(
                import_data->format,
                source_images.front()->get_width(),
                source_images.front()->get_height());

        if (!texture) {
            WG_LOG_ERROR("Failed to instantiate texture " << name);
            return StatusCode::Error;
        }

        asset = texture;
        asset->set_name(name);
        asset->set_import_data(meta.import_data);

        texture->set_source_images(source_images);
        texture->set_sampler_from_desc(import_data->sampling);
        texture->set_compression(import_data->compression);

        if (import_data->mipmaps) {
            if (!texture->generate_mips()) {
                WG_LOG_ERROR("failed to gen mip chain for " << name);
                return StatusCode::Error;
            }
        }
        if (import_data->compression.format != TexCompressionFormat::Unknown) {
            if (!texture->generate_compressed_data()) {
                WG_LOG_ERROR("failed to compress data for " << name);
                return StatusCode::Error;
            }
        }
        if (!texture->generate_gfx_resource()) {
            WG_LOG_ERROR("failed create gfx asset for " << name);
            return StatusCode::Error;
        }

        return WG_OK;
    }

}// namespace wmoge