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

#include "asset/asset_import_data.hpp"

namespace wmoge {

    /**
     * @class TextureImportData
     * @brief Base class for import data to import textures for GPU rendering
     */
    class TextureImportData : public AssetImportData {
    public:
        WG_RTTI_CLASS(TextureImportData, AssetImportData);

        TextureImportData()           = default;
        ~TextureImportData() override = default;

        GfxSamplerDesc          sampling{};
        GrcTexCompressionParams compression{};
        GrcTexResizeParams      resizing{};
        GfxFormat               format   = GfxFormat::RGBA8;
        int                     channels = 4;
        bool                    mipmaps  = true;
        bool                    srgb     = true;
    };

    WG_RTTI_CLASS_BEGIN(TextureImportData) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(sampling, {RttiOptional});
        WG_RTTI_FIELD(compression, {RttiOptional});
        WG_RTTI_FIELD(resizing, {RttiOptional});
        WG_RTTI_FIELD(format, {RttiOptional});
        WG_RTTI_FIELD(channels, {RttiOptional});
        WG_RTTI_FIELD(mipmaps, {RttiOptional});
        WG_RTTI_FIELD(srgb, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class Texture2dImportOptions
     * @brief Options to import 2d-texture from a source file
     */
    class Texture2dImportData : public TextureImportData {
    public:
        WG_RTTI_CLASS(Texture2dImportData, TextureImportData);

        Texture2dImportData()           = default;
        ~Texture2dImportData() override = default;
    };

    WG_RTTI_CLASS_BEGIN(Texture2dImportData) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class TextureCubeImportOptions
     * @brief Options to import a cube-map texture from source files
     */
    class TextureCubeImportData : public TextureImportData {
    public:
        WG_RTTI_CLASS(TextureCubeImportData, TextureImportData);

        TextureCubeImportData()           = default;
        ~TextureCubeImportData() override = default;
    };

    WG_RTTI_CLASS_BEGIN(TextureCubeImportData) {
        WG_RTTI_META_DATA();
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

}// namespace wmoge