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

        int         size  = 0;
        GfxMemUsage usage = GfxMemUsage::GpuLocal;
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

    protected:
        int         m_size;
        GfxMemUsage m_usage;
    };

    using GfxBufferRef = Ref<GfxBuffer>;

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

    using GfxUniformBufferRef = Ref<GfxUniformBuffer>;

    /**
     * @class GfxStorageBuffer
     * @brief Gfx storage buffer
     */
    class GfxStorageBuffer : public GfxBuffer {
    public:
        ~GfxStorageBuffer() override = default;
    };

    using GfxStorageBufferRef = Ref<GfxStorageBuffer>;

    /**
     * @brief Setup to bind a particular buffer range
     * @tparam Buffer
     */
    template<typename Buffer>
    struct GfxBufferSetup {
        Buffer* buffer = nullptr;
        int     offset = 0;
        int     range  = 0;
    };

    /**
     * @class GfxVertBuffersSetup
     * @brief Setup to bind an vertex buffers
     */
    struct GfxVertBuffersSetup {
        GfxVertBuffer* buffers[GfxLimits::MAX_VERT_BUFFERS] = {0};
        int            offsets[GfxLimits::MAX_VERT_BUFFERS] = {0};

        bool operator==(const GfxVertBuffersSetup& other) const {
            for (int i = 0; i < GfxLimits::MAX_VERT_BUFFERS; i++) {
                if (buffers[i] != other.buffers[i]) {
                    return false;
                }
                if (offsets[i] != other.offsets[i]) {
                    return false;
                }
            }
            return true;
        }
    };

    /**
     * @class GfxIndexBufferSetup
     * @brief Setup to bind an index buffer
     */
    struct GfxIndexBufferSetup {
        GfxIndexBuffer* buffer     = nullptr;
        int             offset     = 0;
        GfxIndexType    index_type = GfxIndexType::Uint32;

        bool operator==(const GfxIndexBufferSetup& other) const {
            return buffer == other.buffer &&
                   offset == other.offset &&
                   index_type == other.index_type;
        }
    };

    /** @brief Setup to bind uniform buffer */
    using GfxUniformBufferSetup = GfxBufferSetup<GfxUniformBuffer>;

    /** @brief Setup to bind storage buffer */
    using GfxStorageBufferSetup = GfxBufferSetup<GfxStorageBuffer>;

}// namespace wmoge