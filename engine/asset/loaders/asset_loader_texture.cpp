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

#include "asset_loader_texture.hpp"

#include "asset/image.hpp"
#include "asset/texture.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "grc/grc_texture_resize.hpp"
#include "system/engine.hpp"

namespace wmoge {

    Status AssetLoaderTexture2d::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& res) {
        WG_AUTO_PROFILE_ASSET("AssetLoaderTexture2d::load");

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("No import options to load texture " << name);
            return StatusCode::InvalidData;
        }

        Texture2dImportOptions options;
        WG_YAML_READ_AS(meta.import_options->crootref(), "params", options);

        Ref<Image> source_image = make_ref<Image>();

        if (!source_image->load(options.source_file, options.channels)) {
            WG_LOG_ERROR("failed to load source image " << options.source_file);
            return StatusCode::FailedRead;
        }

        if (!GrcTexResize::resize(options.resizing, *source_image)) {
            WG_LOG_ERROR("failed to resize source image " << options.source_file);
            return StatusCode::FailedResize;
        }

        Ref<Texture2d> texture = make_ref<Texture2d>(
                options.format,
                source_image->get_width(),
                source_image->get_height());

        if (!texture) {
            WG_LOG_ERROR("Failed to instantiate texture " << name);
            return StatusCode::FailedInstantiate;
        }

        res = texture;
        res->set_name(name);

        texture->set_source_images({source_image});
        texture->set_sampler_from_desc(options.sampling);
        texture->set_compression(options.compression);

        if (options.mipmaps) {
            if (!texture->generate_mips()) {
                WG_LOG_ERROR("failed to gen mip chain for " << name);
                return StatusCode::Error;
            }
        }
        if (options.compression.format != GrcTexCompressionFormat::Unknown) {
            if (!texture->generate_compressed_data()) {
                WG_LOG_ERROR("failed to compress data for " << name);
                return StatusCode::Error;
            }
        }
        if (!texture->generate_gfx_resource()) {
            WG_LOG_ERROR("failed create gfx asset for " << name);
            return StatusCode::Error;
        }

        return StatusCode::Ok;
    }
    Strid AssetLoaderTexture2d::get_name() {
        return SID("texture_2d");
    }

    Status AssetLoaderTextureCube::load(const Strid& name, const AssetMeta& meta, Ref<Asset>& res) {
        WG_AUTO_PROFILE_ASSET("AssetLoaderTextureCube::load");

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("No import options to load texture " << name);
            return StatusCode::InvalidData;
        }

        TextureCubeImportOptions options;
        WG_YAML_READ_AS(meta.import_options->crootref(), "params", options);

        std::vector<Ref<Image>> source_images;

        auto load_source = [&](const std::string& path) {
            auto image = source_images.emplace_back(make_ref<Image>());
            if (!image->load(path, options.channels)) {
                WG_LOG_ERROR("failed to load source image " << path);
                return false;
            }
            image->set_name(SID(path));
            return true;
        };

        if (!load_source(options.source_files.right) ||
            !load_source(options.source_files.left) ||
            !load_source(options.source_files.top) ||
            !load_source(options.source_files.bottom) ||
            !load_source(options.source_files.front) ||
            !load_source(options.source_files.back)) {
            return StatusCode::FailedRead;
        }

        for (Ref<Image>& source_image : source_images) {
            if (!GrcTexResize::resize(options.resizing, *source_image)) {
                WG_LOG_ERROR("failed to resize source image " << source_image->get_name());
                return StatusCode::FailedResize;
            }
        }

        Ref<TextureCube> texture = make_ref<TextureCube>(
                options.format,
                source_images.front()->get_width(),
                source_images.front()->get_height());

        if (!texture) {
            WG_LOG_ERROR("Failed to instantiate texture " << name);
            return StatusCode::Error;
        }

        res = texture;
        res->set_name(name);

        texture->set_source_images(source_images);
        texture->set_sampler_from_desc(options.sampling);
        texture->set_compression(options.compression);

        if (options.mipmaps) {
            if (!texture->generate_mips()) {
                WG_LOG_ERROR("failed to gen mip chain for " << name);
                return StatusCode::Error;
            }
        }
        if (options.compression.format != GrcTexCompressionFormat::Unknown) {
            if (!texture->generate_compressed_data()) {
                WG_LOG_ERROR("failed to compress data for " << name);
                return StatusCode::Error;
            }
        }
        if (!texture->generate_gfx_resource()) {
            WG_LOG_ERROR("failed create gfx asset for " << name);
            return StatusCode::Error;
        }

        return StatusCode::Ok;
    }
    Strid AssetLoaderTextureCube::get_name() {
        return SID("texture_cube");
    }

}// namespace wmoge