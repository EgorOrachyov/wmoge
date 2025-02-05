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

#include <functional>

namespace wmoge {

    /** @brief Rdg resource usage flag */
    enum class RdgResourceFlag {
        Imported,
        Pooled,
        Allocated,
        NoTransitions
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

        virtual void           allocate(class RdgPool& pool) {}
        virtual void           release(class RdgPool& pool) {}
        virtual GfxResourceRef get_gfx() const { return nullptr; }
        virtual bool           is_texture() const { return false; }
        virtual bool           is_param_block() const { return false; }
        virtual bool           is_buffer() const { return false; }
        virtual bool           is_vertex() const { return false; }
        virtual bool           is_index() const { return false; }
        virtual bool           is_uniform() const { return false; }
        virtual bool           is_storage() const { return false; }

        bool is_pooled() const { return m_flags.get(RdgResourceFlag::Pooled); }
        bool is_allocated() const { return m_flags.get(RdgResourceFlag::Allocated); }
        bool is_transitionable() const { return !m_flags.get(RdgResourceFlag::NoTransitions); }

        [[nodiscard]] const RdgResourceId&    get_id() const { return m_id; }
        [[nodiscard]] const RdgResourceFlags& get_flags() const { return m_flags; }
        [[nodiscard]] const Strid&            get_name() const { return m_name; }

    protected:
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

        void           allocate(class RdgPool& pool) override;
        void           release(class RdgPool& pool) override;
        GfxResourceRef get_gfx() const override { return m_gfx; }
        bool           is_texture() const override { return true; }

        [[nodiscard]] const GfxTextureDesc& get_desc() const { return m_desc; }
        [[nodiscard]] GfxTexture*           get_texture() const { return m_gfx.get(); }
        [[nodiscard]] const GfxTextureRef&  get_texture_ref() const { return m_gfx; }

    private:
        GfxTextureDesc m_desc;
        GfxTextureRef  m_gfx;
    };

    using RdgTextureRef = Ref<RdgTexture>;

    /** @brief Rdg param block resource */
    class RdgParamBlock : public RdgResource {
    public:
        RdgParamBlock(Shader* shader, std::int16_t space_idx, RdgResourceId id, Strid name);

        void         allocate(class RdgPool& pool) override;
        void         release(class RdgPool& pool) override;
        bool         is_param_block() const override { return true; }
        virtual void pack() {}

        [[nodiscard]] ShaderParamBlock*            get_param_block() const { return m_ptr.get(); }
        [[nodiscard]] const Ref<ShaderParamBlock>& get_param_block_ref() const { return m_ptr; }
        [[nodiscard]] Shader*                      get_shader() const { return m_shader; }
        [[nodiscard]] std::int16_t                 get_space_idx() const { return m_space_idx; }

    protected:
        Ref<ShaderParamBlock> m_ptr;
        Shader*               m_shader;
        std::int16_t          m_space_idx;
    };

    using RdgParamBlockRef = Ref<RdgParamBlock>;

    /** @brief Rdg buffer resource */
    class RdgBuffer : public RdgResource {
    public:
        RdgBuffer(const GfxBufferDesc& desc, RdgResourceId id, Strid name);

        bool is_buffer() const override { return true; }

    protected:
        GfxBufferDesc m_desc;
    };

    /** @brief Rdg vertex buffer resource */
    class RdgVertBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;
        RdgVertBuffer(const GfxVertBufferRef& buffer, RdgResourceId id);

        GfxResourceRef get_gfx() const override { return m_gfx; }
        bool           is_vertex() const override { return true; }

        [[nodiscard]] GfxVertBuffer*          get_buffer() const { return m_gfx.get(); }
        [[nodiscard]] const GfxVertBufferRef& get_buffer_ref() const { return m_gfx; }

    private:
        GfxVertBufferRef m_gfx;
    };

    /** @brief Rdg index buffer resource */
    class RdgIndexBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;
        RdgIndexBuffer(const GfxIndexBufferRef& buffer, RdgResourceId id);

        GfxResourceRef get_gfx() const override { return m_gfx; }
        bool           is_index() const override { return true; }

        [[nodiscard]] GfxIndexBuffer*          get_buffer() const { return m_gfx.get(); }
        [[nodiscard]] const GfxIndexBufferRef& get_buffer_ref() const { return m_gfx; }

    private:
        GfxIndexBufferRef m_gfx;
    };

    /** @brief Rdg uniform buffer resource */
    class RdgUniformBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;
        RdgUniformBuffer(const GfxUniformBufferRef& buffer, RdgResourceId id);

        GfxResourceRef get_gfx() const override { return m_gfx; }
        bool           is_uniform() const override { return true; }

        [[nodiscard]] GfxUniformBuffer*          get_buffer() const { return m_gfx.get(); }
        [[nodiscard]] const GfxUniformBufferRef& get_buffer_ref() const { return m_gfx; }

    private:
        GfxUniformBufferRef m_gfx;
    };

    /** @brief Rdg storage buffer resource */
    class RdgStorageBuffer : public RdgBuffer {
    public:
        using RdgBuffer::RdgBuffer;
        RdgStorageBuffer(const GfxStorageBufferRef& buffer, RdgResourceId id);

        GfxResourceRef get_gfx() const override { return m_gfx; }
        bool           is_storage() const override { return true; }

        [[nodiscard]] GfxStorageBuffer*          get_buffer() const { return m_gfx.get(); }
        [[nodiscard]] const GfxStorageBufferRef& get_buffer_ref() const { return m_gfx; }

    private:
        GfxStorageBufferRef m_gfx;
    };

}// namespace wmoge