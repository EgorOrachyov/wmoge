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

#include "asset/image.hpp"

namespace wmoge {

    /**
     * @brief Available texture assets sizes for optimized memory usage
    */
    enum GrcTexSizePreset {
        None = 0,
        Size128x128,
        Size256x256,
        Size512x512,
        Size1024x1024,
        Size2048x2048,
        Size4096x4096
    };

    /**
     * @class GrcTexResizeParams
     * @brief Params to resize source texture image content (on import)
    */
    struct GrcTexResizeParams {
        GrcTexSizePreset preset      = GrcTexSizePreset::None;
        bool             auto_adjust = true;
        bool             minify      = true;

        WG_IO_DECLARE(GrcTexResizeParams);
    };

    /**
     * @class GrcTexResize
     * @brief Handles image data resize before texture creation
    */
    class GrcTexResize {
    public:
        /**
         * Resizes provided image according to the params given
         * 
         * @param params Resize options
         * @param image Inout image to be resized
         * @param preset Out preset given image
        */
        static Status resize(const GrcTexResizeParams& params, Image& image);

        static Vec2i            preset_to_size(GrcTexSizePreset preset);
        static GrcTexSizePreset fit_preset(int width, int height);
    };

}// namespace wmoge