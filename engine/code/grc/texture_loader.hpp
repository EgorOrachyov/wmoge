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

#include "asset/asset_loader.hpp"

namespace wmoge {

    /**
     * @class Texture2dLoader
     * @brief Asset loader for runtime texture2d objects for gpu rendering
     */
    class Texture2dLoader : public AssetLoader {
    public:
        WG_RTTI_CLASS(Texture2dLoader, AssetLoader);

        Status load(AssetLoadContext& context, const UUID& asset_id, Ref<Asset>& asset) override;
    };

    WG_RTTI_CLASS_BEGIN(Texture2dLoader) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class TextureCubeLoader
     * @brief Asset loader for runtime textureCube objects for gpu rendering
     */
    class TextureCubeLoader : public AssetLoader {
    public:
        WG_RTTI_CLASS(TextureCubeLoader, AssetLoader);

        Status load(AssetLoadContext& context, const UUID& asset_id, Ref<Asset>& asset) override;
    };

    WG_RTTI_CLASS_BEGIN(TextureCubeLoader) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge