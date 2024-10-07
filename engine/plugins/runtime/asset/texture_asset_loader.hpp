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

#include "asset/asset_loader_adapter.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_sampler.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/texture.hpp"

namespace wmoge {

    /**
     * @class Texture2dAssetLoader
     * @brief Loader for 2d textures through stb image library
     */
    class Texture2dAssetLoader final : public AssetLoaderTyped<Texture2d> {
    public:
        WG_RTTI_CLASS(Texture2dAssetLoader, AssetLoader);

        Texture2dAssetLoader()           = default;
        ~Texture2dAssetLoader() override = default;

        Status fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) override;
        Status load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<Texture2d>& asset) override;
    };

    WG_RTTI_CLASS_BEGIN(Texture2dAssetLoader) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class TextureCubeAssetLoader
     * @brief Loader for cube-map textures through stb image library
     */
    class TextureCubeAssetLoader final : public AssetLoaderTyped<TextureCube> {
    public:
        WG_RTTI_CLASS(TextureCubeAssetLoader, AssetLoader);

        TextureCubeAssetLoader()           = default;
        ~TextureCubeAssetLoader() override = default;

        Status fill_request(AssetLoadContext& context, const AssetId& asset_id, AssetLoadRequest& request) override;
        Status load_typed(AssetLoadContext& context, const AssetId& asset_id, const AssetLoadResult& result, Ref<TextureCube>& asset) override;
    };

    WG_RTTI_CLASS_BEGIN(TextureCubeAssetLoader) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge