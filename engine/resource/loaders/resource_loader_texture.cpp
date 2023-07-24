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

#include "resource_loader_texture.hpp"

#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "gfx/gfx_ctx.hpp"
#include "gfx/gfx_driver.hpp"
#include "resource/image.hpp"
#include "resource/texture.hpp"

namespace wmoge {

    bool ResourceLoaderTexture2d::load(const StringId& name, const ResourceMeta& meta, Ref<Resource>& res) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderTexture2d::load");

        Ref<Texture2d> texture = meta.cls->instantiate().cast<Texture2d>();

        if (!texture) {
            WG_LOG_ERROR("Failed to instantiate texture " << name);
            return false;
        }

        res = texture;

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("No import options to load texture " << name);
            return false;
        }

        Texture2dImportOptions options;
        WG_YAML_READ_AS(meta.import_options->crootref(), "params", options);

        Ref<Image> source_image = make_ref<Image>();

        if (!source_image->load(options.source_file, options.channels)) {
            WG_LOG_ERROR("failed to load source image " << options.source_file);
            return false;
        }

        std::vector<Ref<Image>> mips;

        if (options.mipmaps) {
            if (!source_image->generate_mip_chain(mips)) {
                WG_LOG_ERROR("failed to gen mip chain for " << name);
                return false;
            }
        } else {
            mips.push_back(source_image);
        }

        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();

        Ref<GfxTexture> gfx_texture = gfx_driver->make_texture_2d(source_image->get_width(), source_image->get_height(), int(mips.size()), options.format, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, name);
        Ref<GfxSampler> gfx_sampler = gfx_driver->make_sampler(options.sampling, SID(options.sampling.to_str()));

        for (int i = 0; i < int(mips.size()); i++) {
            auto& mip = mips[i];
            gfx_ctx->update_texture_2d(gfx_texture, i, Rect2i(0, 0, mip->get_width(), mip->get_height()), mip->get_pixel_data());
        }

        texture->create(gfx_texture, gfx_sampler);

        return true;
    }
    StringId ResourceLoaderTexture2d::get_name() {
        return SID("texture_2d");
    }

    bool ResourceLoaderTextureCube::load(const StringId& name, const ResourceMeta& meta, Ref<Resource>& res) {
        WG_AUTO_PROFILE_RESOURCE("ResourceLoaderTextureCube::load");

        Ref<TextureCube> texture = meta.cls->instantiate().cast<TextureCube>();

        if (!texture) {
            WG_LOG_ERROR("Failed to instantiate texture " << name);
            return false;
        }

        res = texture;

        if (!meta.import_options.has_value()) {
            WG_LOG_ERROR("No import options to load texture " << name);
            return false;
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
            return true;
        };

        if (!load_source(options.source_files.right) ||
            !load_source(options.source_files.left) ||
            !load_source(options.source_files.top) ||
            !load_source(options.source_files.bottom) ||
            !load_source(options.source_files.front) ||
            !load_source(options.source_files.back)) {
            return false;
        }

        std::vector<Ref<Image>> mips;

        for (auto& source_image : source_images) {
            if (options.mipmaps) {
                std::vector<Ref<Image>> face_mips;

                if (!source_image->generate_mip_chain(face_mips)) {
                    WG_LOG_ERROR("failed to gen mip chain for " << name);
                    return false;
                }

                for (auto& face_mip : face_mips) {
                    mips.push_back(std::move(face_mip));
                }
            } else {
                mips.push_back(source_image);
            }
        }

        Engine*    engine     = Engine::instance();
        GfxDriver* gfx_driver = engine->gfx_driver();
        GfxCtx*    gfx_ctx    = engine->gfx_ctx();

        Ref<GfxTexture> gfx_texture = gfx_driver->make_texture_cube(source_images[0]->get_width(), source_images[0]->get_height(), int(mips.size()) / 6, options.format, {GfxTexUsageFlag::Sampling}, GfxMemUsage::GpuLocal, name);
        Ref<GfxSampler> gfx_sampler = gfx_driver->make_sampler(options.sampling, SID(options.sampling.to_str()));

        for (int face = 0; face < 6; face++) {
            for (int i = 0; i < int(mips.size()); i++) {
                auto& mip = mips[face * gfx_texture->mips_count() + i];
                gfx_ctx->update_texture_cube(gfx_texture, i, face, Rect2i(0, 0, mip->get_width(), mip->get_height()), mip->get_pixel_data());
            }
        }

        texture->create(gfx_texture, gfx_sampler);

        return true;
    }
    StringId ResourceLoaderTextureCube::get_name() {
        return SID("texture_cube");
    }

}// namespace wmoge