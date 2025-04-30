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

#include "asset/asset.hpp"
#include "asset/asset_import_data.hpp"
#include "math/color.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class IconImportData
     * @brief Options to import an icon from image
     */
    struct IconImportData {
        WG_RTTI_STRUCT(IconImportData);

        std::string image;
        Strid       name;
        Vec2f       uv_pos{0, 0};
        Vec2f       uv_size{1, 1};
        Color4f     tint = Color::WHITE4f;
    };

    WG_RTTI_STRUCT_BEGIN(IconImportData) {
        WG_RTTI_FIELD(image, {});
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(uv_pos, {RttiOptional});
        WG_RTTI_FIELD(uv_size, {RttiOptional});
        WG_RTTI_FIELD(tint, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class IconAtlasImportData
     * @brief Options to import an icons atlas from external sources, such as `png`, `jpeg`, `bmp`, etc.
     */
    class IconAtlasImportData : public AssetImportData {
    public:
        WG_RTTI_CLASS(IconAtlasImportData, AssetImportData);

        std::vector<IconImportData> icons;
        TexCompressionParams        compression{};
        GfxFormat                   format   = GfxFormat::RGBA8;
        int                         channels = 4;
        bool                        srgb     = true;
    };

    WG_RTTI_CLASS_BEGIN(IconAtlasImportData) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(icons, {RttiOptional});
        WG_RTTI_FIELD(compression, {RttiOptional});
        WG_RTTI_FIELD(format, {RttiOptional});
        WG_RTTI_FIELD(channels, {RttiOptional});
        WG_RTTI_FIELD(srgb, {RttiOptional});
    }
    WG_RTTI_END;

}// namespace wmoge