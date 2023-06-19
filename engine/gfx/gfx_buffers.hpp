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

#ifndef WMOGE_GFX_BUFFERS_HPP
#define WMOGE_GFX_BUFFERS_HPP

#include "gfx/gfx_resource.hpp"

namespace wmoge {

    /**
     * @class GfxBuffer
     * @brief Base class for gfx buffer
     */
    class GfxBuffer : public GfxResource {
    public:
        ~GfxBuffer() override = default;

        [[nodiscard]] int         size() const { return m_size; }
        [[nodiscard]] GfxMemUsage buffer_usage() const { return m_usage; }

    protected:
        int         m_size;
        GfxMemUsage m_usage;
    };

    /**
     * @class GfxVertBuffer
     * @brief Gfx vertex buffer
     */
    class GfxVertBuffer : public GfxBuffer {
    public:
        ~GfxVertBuffer() override = default;
    };

    /**
     * @class GfxIndexBuffer
     * @brief Gfx index buffer
     */
    class GfxIndexBuffer : public GfxBuffer {
    public:
        ~GfxIndexBuffer() override = default;
    };

    /**
     * @class GfxUniformBuffer
     * @brief Gfx uniform buffer
     */
    class GfxUniformBuffer : public GfxBuffer {
    public:
        ~GfxUniformBuffer() override = default;
    };

    /**
     * @class GfxStorageBuffer
     * @brief Gfx storage buffer
     */
    class GfxStorageBuffer : public GfxBuffer {
    public:
        ~GfxStorageBuffer() override = default;
    };

    template<typename Buffer>
    struct GfxBufferSetup {
        Buffer* buffer = nullptr;
        int     offset = 0;
        int     range  = 0;
    };

    using GfxUniformBufferSetup = GfxBufferSetup<GfxUniformBuffer>;
    using GfxStorageBufferSetup = GfxBufferSetup<GfxStorageBuffer>;

}// namespace wmoge

#endif//WMOGE_GFX_BUFFERS_HPP
