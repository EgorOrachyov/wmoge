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

#include "gfx_buffers.hpp"

namespace wmoge {

    bool GfxBufferDesc::operator==(const GfxBufferDesc& other) const {
        return size == other.size &&
               usage == other.usage;
    }

    bool GfxBufferDesc::is_compatible(const GfxBufferDesc& other) const {
        return *this == other;
    }

    GfxBufferDesc GfxBuffer::desc() const {
        GfxBufferDesc d;
        d.size  = m_size;
        d.usage = m_usage;
        return d;
    }

    WG_IO_BEGIN(GfxVertStream)
    WG_IO_FIELD(attribs)
    WG_IO_FIELD(buffer)
    WG_IO_FIELD(offset)
    WG_IO_FIELD(size)
    WG_IO_FIELD(stride)
    WG_IO_END(GfxVertStream)

    WG_IO_BEGIN(GfxIndexStream)
    WG_IO_FIELD(index_type)
    WG_IO_FIELD(buffer)
    WG_IO_FIELD(offset)
    WG_IO_FIELD(size)
    WG_IO_END(GfxIndexStream)

}// namespace wmoge