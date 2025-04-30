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

#include "grc/image.hpp"
#include "rtti/traits.hpp"

namespace wmoge {

    /**
     * @brief Available texture assets sizes for optimized memory usage
    */
    enum TexSizePreset {
        None = 0,
        Size32x32,
        Size64x64,
        Size128x128,
        Size256x256,
        Size512x512,
        Size1024x1024,
        Size2048x2048,
        Size4096x4096
    };

    /**
     * @class TexResizeParams
     * @brief Params to resize source texture image content (on import)
    */
    struct TexResizeParams {
        WG_RTTI_STRUCT(TexResizeParams);

        TexSizePreset preset      = TexSizePreset::None;
        bool          auto_adjust = true;
        bool          minify      = true;
    };

    WG_RTTI_STRUCT_BEGIN(TexResizeParams) {
        WG_RTTI_FIELD(preset, {RttiOptional});
        WG_RTTI_FIELD(auto_adjust, {RttiOptional});
        WG_RTTI_FIELD(minify, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class TexResize
     * @brief Handles image data resize before texture creation
    */
    class TexResize {
    public:
        /**
         * Resizes provided image according to the params given
         * 
         * @param params Resize options
         * @param image Inout image to be resized
         * @param preset Out preset given image
        */
        static Status resize(const TexResizeParams& params, Image& image);

        static Vec2i         preset_to_size(TexSizePreset preset);
        static TexSizePreset fit_preset(int width, int height);
    };

}// namespace wmoge