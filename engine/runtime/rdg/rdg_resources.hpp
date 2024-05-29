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

namespace wmoge {

    /** @brief Rdg resource usage flag */
    enum class RDGResourceFlag {
        Imported,
        Pooled,
        Allocated,
    };

    /** @brief Rdg resource flags mask */
    using RDGResourceFlags = Mask<RDGResourceFlag>;

    /** @brief Rdg resource id */
    using RDGResourceId = SimpleId<std::uint32_t>;

    /**
     * @class RDGResource
     * @brief Rdg resource base class 
     */
    class RDGResource : public RefCnt {
    public:
        RDGResource(RDGResourceId id, RDGResourceFlags flags, Strid name);
        ~RDGResource() override = default;

        virtual Status allocate(class RDGPool& pool) { return StatusCode::NotImplemented; }
        virtual Status release(class RDGPool& pool) { return StatusCode::NotImplemented; }

        void set_gfx(GfxResourceRef gfx) { m_gfx = std::move(gfx); }
        bool needs_allocation() const { return m_flags.get(RDGResourceFlag::Pooled); }

        [[nodiscard]] const GfxResourceRef&   get_gfx() const { return m_gfx; }
        [[nodiscard]] const RDGResourceId&    get_id() const { return m_id; }
        [[nodiscard]] const RDGResourceFlags& get_flags() const { return m_flags; }
        [[nodiscard]] const Strid&            get_name() const { return m_name; }

    private:
        GfxResourceRef   m_gfx;
        RDGResourceId    m_id;
        RDGResourceFlags m_flags;
        Strid            m_name;
    };

    using RDGResourceRef = Ref<RDGResource>;

    /** @brief Rdg texture resource */
    class RDGTexture : public RDGResource {
    public:
        RDGTexture(const GfxTextureDesc& desc, RDGResourceId id, Strid name);
        RDGTexture(const GfxTextureRef& texture, RDGResourceId id);
        ~RDGTexture() override = default;

    private:
        GfxTextureDesc m_desc;
    };

    /** @brief Rdg buffer resource */
    class RDGBuffer : public RDGResource {
    public:
        RDGBuffer(const GfxBufferDesc& desc, RDGResourceId id, Strid name);
        RDGBuffer(const GfxBufferRef& buffer, RDGResourceId id);
        ~RDGBuffer() override = default;

    private:
        GfxBufferDesc m_desc;
    };

    /** @brief Rdg uniform buffer resource */
    class RDGUniformBuffer : public RDGBuffer {
    public:
        using RDGBuffer::RDGBuffer;
    };

    /** @brief Rdg storage buffer resource */
    class RDGStorageBuffer : public RDGBuffer {
    public:
        using RDGBuffer::RDGBuffer;
    };

}// namespace wmoge