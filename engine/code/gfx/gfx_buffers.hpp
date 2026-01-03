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

#include "core/data.hpp"
#include "gfx/gfx_resource.hpp"

#include <array>

namespace wmoge {

    /**
     * @class GfxBufferDesc
     * @brief Gfx buffer desc
     */
    struct GfxBufferDesc {
        [[nodiscard]] bool operator==(const GfxBufferDesc& other) const;
        [[nodiscard]] bool is_compatible(const GfxBufferDesc& other) const;

        int           size  = 0;
        GfxMemUsage   usage = GfxMemUsage::GpuLocal;
        GfxBufferType type  = GfxBufferType::Vertex;
    };

    /**
     * @class GfxBuffer
     * @brief Base class for gfx buffer
     */
    class GfxBuffer : public GfxResource {
    public:
        ~GfxBuffer() override = default;

        [[nodiscard]] GfxBufferDesc desc() const;
        [[nodiscard]] int           size() const { return m_size; }
        [[nodiscard]] GfxMemUsage   buffer_usage() const { return m_usage; }
        [[nodiscard]] GfxBufferType buffer_type() const { return m_type; }

    protected:
        int           m_size;
        GfxMemUsage   m_usage;
        GfxBufferType m_type;
    };

    using GfxBufferRef = Ref<GfxBuffer>;

}// namespace wmoge
