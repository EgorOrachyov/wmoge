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

#include "asset/asset_import_settings.hpp"
#include "gfx/gfx_defs.hpp"
#include "grc/texture.hpp"
#include "grc/texture_compression.hpp"
#include "grc/texture_resize.hpp"

namespace wmoge {

    /**
     * @class TextureImportSettings
     * @brief Base class for texture import settings
     */
    class TextureImportSettings : public AssetImportSettings {
    public:
        WG_RTTI_CLASS(TextureImportSettings, AssetImportSettings)

        GfxFormat            format   = GfxFormat::RGBA8;
        GfxTexSwizz          swizz    = GfxTexSwizz::None;
        int                  channels = 4;
        bool                 mipmaps  = true;
        bool                 srgb     = true;
        TexCompressionParams compression{};
        TexResizeParams      resizing{};
        DefaultSampler       sampling = DefaultSampler::Default;
    };

    WG_RTTI_CLASS_BEGIN(TextureImportSettings) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(format, {RttiOptional});
        WG_RTTI_FIELD(swizz, {RttiOptional});
        WG_RTTI_FIELD(channels, {RttiOptional});
        WG_RTTI_FIELD(mipmaps, {RttiOptional});
        WG_RTTI_FIELD(srgb, {RttiOptional});
        WG_RTTI_FIELD(compression, {RttiOptional});
        WG_RTTI_FIELD(resizing, {RttiOptional});
        WG_RTTI_FIELD(sampling, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class Texture2dImportSettings
     * @brief 2d texture import settings
     */
    class Texture2dImportSettings : public TextureImportSettings {
    public:
        WG_RTTI_CLASS(Texture2dImportSettings, TextureImportSettings)
    };

    WG_RTTI_CLASS_BEGIN(Texture2dImportSettings) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class TextureCubeImportSettings
     * @brief Cube texture import settings
     */
    class TextureCubeImportSettings : public TextureImportSettings {
    public:
        WG_RTTI_CLASS(TextureCubeImportSettings, TextureImportSettings)
    };

    WG_RTTI_CLASS_BEGIN(TextureCubeImportSettings) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge