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

#include "rdg_resources.hpp"

#include "rdg/rdg_pool.hpp"

namespace wmoge {

    RdgResource::RdgResource(RdgResourceId id, RdgResourceFlags flags, Strid name) {
        m_id    = id;
        m_flags = flags;
        m_name  = name;
    }

    RdgTexture::RdgTexture(const GfxTextureDesc& desc, RdgResourceId id, Strid name)
        : RdgResource(id, RdgResourceFlags{RdgResourceFlag::Pooled}, name) {
        m_desc = desc;
    }

    RdgTexture::RdgTexture(const GfxTextureRef& texture, RdgResourceId id)
        : RdgResource(id, RdgResourceFlags{RdgResourceFlag::Imported}, texture->name()) {
        m_desc = texture->desc();
        m_gfx  = texture;
    }

    void RdgTexture::allocate(RdgPool& pool) {
        m_gfx = pool.allocate_texture(m_desc);
        m_flags.set(RdgResourceFlag::Allocated, true);
    }

    void RdgTexture::release(RdgPool& pool) {
        pool.release_texture(get_texture_ref());
        m_gfx.reset();
        m_flags.set(RdgResourceFlag::Allocated, false);
    }

    RdgParamBlock::RdgParamBlock(Shader* shader, std::int16_t space_idx, RdgResourceId id, Strid name)
        : RdgResource(id, RdgResourceFlags{RdgResourceFlag::Pooled, RdgResourceFlag::NoTransitions}, name) {
        m_shader    = shader;
        m_space_idx = space_idx;
    }

    void RdgParamBlock::allocate(RdgPool& pool) {
        ShaderParamBlockDesc desc;
        desc.shader    = m_shader;
        desc.space_idx = m_space_idx;
        m_ptr          = pool.allocate_param_block(desc);
        m_flags.set(RdgResourceFlag::Allocated, true);
    }

    void RdgParamBlock::release(RdgPool& pool) {
        pool.release_param_block(get_param_block_ref());
        m_ptr.reset();
        m_flags.set(RdgResourceFlag::Allocated, false);
    }

    RdgBuffer::RdgBuffer(const GfxBufferDesc& desc, RdgResourceId id, Strid name)
        : RdgResource(id, RdgResourceFlags{RdgResourceFlag::Pooled}, name) {
        m_desc = desc;
    }

    RdgVertBuffer::RdgVertBuffer(const GfxVertBufferRef& buffer, RdgResourceId id)
        : RdgBuffer(buffer->desc(), id, buffer->name()) {
        m_gfx = buffer;
    }

    RdgIndexBuffer::RdgIndexBuffer(const GfxIndexBufferRef& buffer, RdgResourceId id)
        : RdgBuffer(buffer->desc(), id, buffer->name()) {
        m_gfx = buffer;
    }

    RdgUniformBuffer::RdgUniformBuffer(const GfxUniformBufferRef& buffer, RdgResourceId id)
        : RdgBuffer(buffer->desc(), id, buffer->name()) {
        m_gfx = buffer;
    }

    RdgStorageBuffer::RdgStorageBuffer(const GfxStorageBufferRef& buffer, RdgResourceId id)
        : RdgBuffer(buffer->desc(), id, buffer->name()) {
        m_gfx = buffer;
    }

}// namespace wmoge
