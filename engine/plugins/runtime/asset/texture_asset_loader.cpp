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
#include "grc/texture_manager.hpp"
#include "grc/texture_resize.hpp"
#include "profiler/profiler.hpp"
#include "system/ioc_container.hpp"

namespace wmoge {

    Status Texture2dAssetLoader::fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) {
        Ref<Texture2dImportData> import_data = context.asset_meta.import_data.cast<Texture2dImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << asset_id);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << asset_id);
            return StatusCode::InvalidData;
        }
        request.add_data_file(FILE_TAG, import_data->source_files[0].file);
        return WG_OK;
    }

    Status Texture2dAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Texture2d>& asset) {
        WG_AUTO_PROFILE_ASSET("Texture2dAssetLoader::load_typed");

        Ref<Texture2dImportData> import_data = context.asset_meta.import_data.cast<Texture2dImportData>();
        assert(import_data);

        Ref<Image> source_image = make_ref<Image>();

        if (!source_image->load(result.get_data_file(FILE_TAG), import_data->channels)) {
            WG_LOG_ERROR("failed to load source image " << import_data->source_files[0].file);
            return StatusCode::FailedRead;
        }

        if (!TexResize::resize(import_data->resizing, *source_image)) {
            WG_LOG_ERROR("failed to resize source image " << import_data->source_files[0].file);
            return StatusCode::FailedResize;
        }

        auto gfx_driver      = IocContainer::iresolve_v<GfxDriver>();
        auto texture_manager = IocContainer::iresolve_v<TextureManager>();

        TextureFlags flags;
        flags.set(TextureFlag::Pooled);
        flags.set(TextureFlag::FromDisk);
        flags.set(TextureFlag::Compressed, import_data->compression.format != TexCompressionFormat::Unknown);

        asset = texture_manager->create_2d(flags,
                                           import_data->format,
                                           source_image->get_width(),
                                           source_image->get_height());

        if (!asset) {
            WG_LOG_ERROR("failed to instantiate texture " << asset_id);
            return StatusCode::FailedInstantiate;
        }

        asset->set_id(asset_id);
        asset->set_source_images({source_image});
        asset->set_sampler(gfx_driver->make_sampler(import_data->sampling, SID(import_data->sampling.to_string())));
        asset->set_compression(import_data->compression);

        if (import_data->mipmaps) {
            if (!asset->generate_mips()) {
                WG_LOG_ERROR("failed to gen mip chain for " << asset_id);
                return StatusCode::Error;
            }
        }
        if (import_data->compression.format != TexCompressionFormat::Unknown) {
            if (!asset->generate_compressed_data()) {
                WG_LOG_ERROR("failed to compress data for " << asset_id);
                return StatusCode::Error;
            }
        }
        texture_manager->init(asset.get());

        return WG_OK;
    }

    static const Strid TAG_IMG_RIGHT  = SID("right");
    static const Strid TAG_IMG_LEFT   = SID("left");
    static const Strid TAG_IMG_TOP    = SID("top");
    static const Strid TAG_IMG_BOTTOM = SID("bottom");
    static const Strid TAG_IMG_FRONT  = SID("front");
    static const Strid TAG_IMG_BACK   = SID("back");

    Status TextureCubeAssetLoader::fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) {
        Ref<TextureCubeImportData> import_data = context.asset_meta.import_data.cast<TextureCubeImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << asset_id);
            return StatusCode::InvalidData;
        }
        if (!import_data->has_soruce_files()) {
            WG_LOG_ERROR("no source file " << asset_id);
            return StatusCode::InvalidData;
        }
        // right, left, top, bottom, front, back
        request.add_data_file(TAG_IMG_RIGHT, import_data->source_files[0].file);
        request.add_data_file(TAG_IMG_LEFT, import_data->source_files[1].file);
        request.add_data_file(TAG_IMG_TOP, import_data->source_files[2].file);
        request.add_data_file(TAG_IMG_BOTTOM, import_data->source_files[3].file);
        request.add_data_file(TAG_IMG_FRONT, import_data->source_files[4].file);
        request.add_data_file(TAG_IMG_BACK, import_data->source_files[5].file);
        return WG_OK;
    }

    Status TextureCubeAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<TextureCube>& asset) {
        WG_AUTO_PROFILE_ASSET("TextureCubeAssetLoader::load_typed");

        Ref<TextureCubeImportData> import_data = context.asset_meta.import_data.cast<TextureCubeImportData>();
        assert(import_data);

        std::vector<Ref<Image>> source_images;

        auto load_source = [&](const Strid& tag) {
            auto image = source_images.emplace_back(make_ref<Image>());
            if (!image->load(result.get_data_file(tag), import_data->channels)) {
                WG_LOG_ERROR("failed to load source image " << asset_id << " tag " << tag);
                return false;
            }
            image->set_id(SID(asset_id.str() + "_" + tag.str()));
            return true;
        };

        if (!load_source(TAG_IMG_RIGHT) ||
            !load_source(TAG_IMG_LEFT) ||
            !load_source(TAG_IMG_TOP) ||
            !load_source(TAG_IMG_BOTTOM) ||
            !load_source(TAG_IMG_FRONT) ||
            !load_source(TAG_IMG_BACK)) {
            return StatusCode::FailedRead;
        }

        for (Ref<Image>& source_image : source_images) {
            if (!TexResize::resize(import_data->resizing, *source_image)) {
                WG_LOG_ERROR("failed to resize source image " << source_image->get_name());
                return StatusCode::FailedResize;
            }
        }

        auto gfx_driver      = IocContainer::iresolve_v<GfxDriver>();
        auto texture_manager = IocContainer::iresolve_v<TextureManager>();

        TextureFlags flags;
        flags.set(TextureFlag::Pooled);
        flags.set(TextureFlag::FromDisk);
        flags.set(TextureFlag::Compressed, import_data->compression.format != TexCompressionFormat::Unknown);

        asset = texture_manager->create_cube(flags,
                                             import_data->format,
                                             source_images.front()->get_width(),
                                             source_images.front()->get_height());

        if (!asset) {
            WG_LOG_ERROR("failed to instantiate texture " << asset_id);
            return StatusCode::Error;
        }

        asset->set_id(asset_id);
        asset->set_source_images(source_images);
        asset->set_sampler(gfx_driver->make_sampler(import_data->sampling, SID(import_data->sampling.to_string())));
        asset->set_compression(import_data->compression);

        if (import_data->mipmaps) {
            if (!asset->generate_mips()) {
                WG_LOG_ERROR("failed to gen mip chain for " << asset_id);
                return StatusCode::Error;
            }
        }
        if (import_data->compression.format != TexCompressionFormat::Unknown) {
            if (!asset->generate_compressed_data()) {
                WG_LOG_ERROR("failed to compress data for " << asset_id);
                return StatusCode::Error;
            }
        }
        texture_manager->init(asset.get());

        return WG_OK;
    }

}// namespace wmoge