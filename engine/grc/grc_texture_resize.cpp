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

#include "grc_texture_resize.hpp"

#include "debug/profiler.hpp"
#include "math/math_utils.hpp"

namespace wmoge {

    WG_IO_BEGIN(GrcTexResizeParams)
    WG_IO_FIELD_OPT(preset)
    WG_IO_FIELD_OPT(auto_adjust)
    WG_IO_FIELD_OPT(minify)
    WG_IO_END(GrcTexResizeParams)

    Status GrcTexResize::resize(const GrcTexResizeParams& params, Image& image) {
        WG_AUTO_PROFILE_RENDER("GrcTexResize::resize");

        GrcTexSizePreset preset = params.preset;

        if (params.auto_adjust || preset == GrcTexSizePreset::None) {
            int w = image.get_width();
            int h = image.get_height();

            if (params.minify) {
                w = Math::min(w, h);
                h = Math::min(w, h);
            } else {
                w = Math::max(w, h);
                h = Math::max(w, h);
            }

            preset = fit_preset(w, h);
        }

        assert(preset != GrcTexSizePreset::None);
        const Vec2i size = preset_to_size(preset);

        return image.resize(size.x(), size.y());
    }

    Vec2i GrcTexResize::preset_to_size(GrcTexSizePreset preset) {
        switch (preset) {
            case GrcTexSizePreset::Size128x128:
                return Vec2i(128, 128);
            case GrcTexSizePreset::Size256x256:
                return Vec2i(256, 256);
            case GrcTexSizePreset::Size512x512:
                return Vec2i(512, 512);
            case GrcTexSizePreset::Size1024x1024:
                return Vec2i(1024, 1024);
            case GrcTexSizePreset::Size2048x2048:
                return Vec2i(2048, 2048);
            case GrcTexSizePreset::Size4096x4096:
                return Vec2i(4096, 4096);
            default:
                return Vec2i(0, 0);
        }
    }

    GrcTexSizePreset GrcTexResize::fit_preset(int width, int height) {
        if (width == 0 || height == 0) {
            return GrcTexSizePreset::None;
        }

        GrcTexSizePreset preset = GrcTexSizePreset::Size128x128;
        Vec2i            size   = preset_to_size(preset);

        while (size.x() < width && size.y() < height && preset != GrcTexSizePreset::Size4096x4096) {
            preset = static_cast<GrcTexSizePreset>(static_cast<int>(preset) + 1);
            size   = preset_to_size(preset);
        }

        return preset;
    }

}// namespace wmoge