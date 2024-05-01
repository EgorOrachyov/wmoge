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

#include "gfx_pipeline.hpp"

#include "core/crc32.hpp"

#include <cstring>

namespace wmoge {

    GfxPsoStateGraphics::GfxPsoStateGraphics() {
        std::memset(this, 0, sizeof(GfxPsoStateGraphics));
        pass              = nullptr;
        program           = nullptr;
        layout            = nullptr;
        vert_format       = nullptr;
        prim_type         = GfxPrimType::Triangles;
        poly_mode         = GfxPolyMode::Fill;
        cull_mode         = GfxPolyCullMode::Disabled;
        front_face        = GfxPolyFrontFace::CounterClockwise;
        depth_enable      = false;
        depth_write       = true;
        depth_func        = GfxCompFunc::Less;
        stencil_enable    = false;
        stencil_wmask     = 0;
        stencil_rvalue    = 0;
        stencil_cmask     = 0;
        stencil_comp_func = GfxCompFunc::Never;
        stencil_sfail     = GfxOp::Keep;
        stencil_dfail     = GfxOp::Keep;
        stencil_dpass     = GfxOp::Keep;
        blending          = false;
    }
    bool GfxPsoStateGraphics::operator==(const GfxPsoStateGraphics& other) const {
        return std::memcmp(this, &other, sizeof(GfxPsoStateGraphics)) == 0;
    }
    std::size_t GfxPsoStateGraphics::hash() const {
        return static_cast<std::size_t>(Crc32::hash(this, sizeof(GfxPsoStateGraphics)));
    }

    GfxPsoStateCompute::GfxPsoStateCompute() {
        std::memset(this, 0, sizeof(GfxPsoStateCompute));
        program = nullptr;
        layout  = nullptr;
    }

    bool GfxPsoStateCompute::operator==(const GfxPsoStateCompute& other) const {
        return std::memcmp(this, &other, sizeof(GfxPsoStateCompute)) == 0;
    }
    std::size_t GfxPsoStateCompute::hash() const {
        return static_cast<std::size_t>(Crc32::hash(this, sizeof(GfxPsoStateCompute)));
    }

}// namespace wmoge