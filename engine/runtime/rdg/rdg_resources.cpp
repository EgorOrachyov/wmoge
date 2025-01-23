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
        set_gfx(texture);
    }

    void RdgTexture::allocate(RdgPool& pool) {
        m_gfx = pool.allocate_texture(m_desc);
    }

    void RdgTexture::release(RdgPool& pool) {
        pool.release_texture(get_texture_ref());
        m_gfx.reset();
    }

    RdgBuffer::RdgBuffer(const GfxBufferDesc& desc, RdgResourceId id, Strid name)
        : RdgResource(id, RdgResourceFlags{RdgResourceFlag::Pooled}, name) {
        m_desc = desc;
    }

    RdgBuffer::RdgBuffer(const GfxBufferRef& buffer, RdgResourceId id)
        : RdgResource(id, RdgResourceFlags{RdgResourceFlag::Imported}, buffer->name()) {
        m_desc = buffer->desc();
        set_gfx(buffer);
    }

}// namespace wmoge
