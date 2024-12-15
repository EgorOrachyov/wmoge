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

#include "core/mask.hpp"
#include "core/ref.hpp"
#include "core/simple_id.hpp"
#include "core/string_id.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/shader.hpp"
#include "grc/shader_param_block.hpp"

namespace wmoge {

    /** @brief Rdg resource usage flag */
    enum class RdgResourceFlag {
        Imported,
        Pooled,
        Allocated,
    };

    /** @brief Rdg resource flags mask */
    using RdgResourceFlags = Mask<RdgResourceFlag>;

    /** @brief Rdg resource id */
    using RdgResourceId = SimpleId<std::uint32_t>;

    /**
     * @class RdgResource
     * @brief Rdg resource base class 
     */
    class RdgResource : public RefCnt {
    public:
        RdgResource(RdgResourceId id, RdgResourceFlags flags, Strid name);
        ~RdgResource() override = default;

        virtual Status allocate(class RdgPool& pool) { return StatusCode::NotImplemented; }
        virtual Status release(class RdgPool& pool) { return StatusCode::NotImplemented; }

        void set_gfx(GfxResourceRef gfx) { m_gfx = std::move(gfx); }
        bool needs_allocation() const { return m_flags.get(RdgResourceFlag::Pooled); }

        [[nodiscard]] const GfxResourceRef&   get_gfx() const { return m_gfx; }
        [[nodiscard]] const RdgResourceId&    get_id() const { return m_id; }
        [[nodiscard]] const RdgResourceFlags& get_flags() const { return m_flags; }
        [[nodiscard]] const Strid&            get_name() const { return m_name; }

    private:
        GfxResourceRef   m_gfx;
        RdgResourceId    m_id;
        RdgResourceFlags m_flags;
        Strid            m_name;
    };

    using RdgResourceRef = Ref<RdgResource>;

    /** @brief Rdg texture resource */
    class RdgTexture : public RdgResource {
    public:
        RdgTexture(const GfxTextureDesc& desc, RdgResourceId id, Strid name);
        RdgTexture(const GfxTextureRef& texture, RdgResourceId id);
        ~RdgTexture() override = default;

        [[nodiscard]] const GfxTextureDesc& get_desc() const { return m_desc; }

    private:
        GfxTextureDesc m_desc;
    };

    /** @brief Rdg buffer resource */
    class RdgBuffer : public RdgResource {
    public:
        RdgBuffer(const GfxBufferDesc& desc, RdgResourceId id, Strid name);
        RdgBuffer(const GfxBufferRef& buffer, RdgResourceId id);
        ~RdgBuffer() override = default;

    private:
        GfxBufferDesc m_desc;
    };

    /** @brief Rdg vertex buffer resource */
    class RdgVertBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;

        [[nodiscard]] GfxVertBuffer* get_buffer() const { return static_cast<GfxVertBuffer*>(get_gfx().get()); }
    };

    /** @brief Rdg index buffer resource */
    class RdgIndexBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;

        [[nodiscard]] GfxIndexBuffer* get_buffer() const { return static_cast<GfxIndexBuffer*>(get_gfx().get()); }
    };

    /** @brief Rdg uniform buffer resource */
    class RdgUniformBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;

        [[nodiscard]] GfxUniformBuffer* get_buffer() const { return static_cast<GfxUniformBuffer*>(get_gfx().get()); }
    };

    /** @brief Rdg storage buffer resource */
    class RdgStorageBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;

        [[nodiscard]] GfxStorageBuffer* get_buffer() const { return static_cast<GfxStorageBuffer*>(get_gfx().get()); }
    };

}// namespace wmoge