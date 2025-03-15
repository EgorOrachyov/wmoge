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

#include "icon_atlas_asset_loader.hpp"

#include "asset/asset_manager.hpp"
#include "grc/texture_builder.hpp"
#include "grc/texture_manager.hpp"
#include "icon_atlas_import_data.hpp"
#include "profiler/profiler_cpu.hpp"

namespace wmoge {

    Status IconAtlasAssetLoader::fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) {
        Ref<IconAtlasImportData> import_data = context.asset_meta.import_data.cast<IconAtlasImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << asset_id);
            return StatusCode::InvalidData;
        }
        for (auto& icon : import_data->icons) {
            request.add_data_file(icon.name, icon.image);
        }
        return WG_OK;
    }

    Status IconAtlasAssetLoader::load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<IconAtlas>& asset) {
        WG_PROFILE_CPU_ASSET("IconAtlasAssetLoader::load_typed");

        Ref<IconAtlasImportData> import_data = context.asset_meta.import_data.cast<IconAtlasImportData>();
        if (!import_data) {
            WG_LOG_ERROR("no import data for " << asset_id);
            return StatusCode::InvalidData;
        }

        auto ioc             = context.ioc;
        auto texture_manager = ioc->resolve_value<TextureManager>();

        IconAtlasDesc desc;

        for (auto& icon : import_data->icons) {
            Ref<Image> icon_image = make_ref<Image>();

            if (!icon_image->load(result.get_data_file(icon.name), import_data->channels)) {
                WG_LOG_ERROR("failed to load icon image " << icon.image);
                return StatusCode::Error;
            }

            IconAtlasPage page;

            TextureBuilder builder(icon.name, texture_manager);
            builder
                    .set_image(icon_image, import_data->format)
                    .set_resize(TexResizeParams())
                    .set_swizz(GfxTexSwizz::None)
                    .set_sampler(DefaultSampler::Linear)
                    .set_compression(import_data->compression)
                    .set_flags({TextureFlag::Pooled, TextureFlag::FromDisk});

            if (!builder.build_2d(page.texture)) {
                WG_LOG_ERROR("failed to build texture " << asset_id);
                return StatusCode::Error;
            }

            page.texture->set_id(AssetId(icon.image));
            page.source_images.push_back(icon_image);

            IconInfo info;
            info.name    = icon.name;
            info.id      = static_cast<int>(desc.icons.size());
            info.page_id = static_cast<int>(desc.pages.size());
            info.uv_pos  = icon.uv_pos;
            info.uv_size = icon.uv_size;
            info.tint    = icon.tint;
            info.pixels  = icon.uv_size * icon_image->get_sizef();

            desc.icons_map[icon.name] = info.id;
            desc.icons.push_back(std::move(info));
            desc.pages.push_back(std::move(page));
        }

        asset = make_ref<IconAtlas>();
        asset->set_id(asset_id);
        asset->set_desc(std::move(desc));

        return WG_OK;
    }

}// namespace wmoge