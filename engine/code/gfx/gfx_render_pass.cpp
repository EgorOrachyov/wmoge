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

#include "gfx_render_pass.hpp"

#include "core/crc32.hpp"

#include <cstring>

namespace wmoge {

    GfxRenderPassDesc::GfxRenderPassDesc() {
        std::memset(this, 0, sizeof(GfxRenderPassDesc));
        std::fill(color_target_fmts.begin(), color_target_fmts.end(), GfxFormat::Unknown);
        std::fill(color_target_ops.begin(), color_target_ops.end(), GfxRtOp::LoadStore);
        depth_stencil_fmt = GfxFormat::Unknown;
        depth_op          = GfxRtOp::LoadStore;
        stencil_op        = GfxRtOp::LoadStore;
    }
    bool GfxRenderPassDesc::operator==(const GfxRenderPassDesc& other) const {
        return std::memcmp(this, &other, sizeof(GfxRenderPassDesc)) == 0;
    }
    std::size_t GfxRenderPassDesc::hash() const {
        return static_cast<std::size_t>(Crc32Util::hash(this, sizeof(GfxRenderPassDesc)));
    }

}// namespace wmoge