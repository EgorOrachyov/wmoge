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

#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_texture.hpp"
#include "grc/shader_param_block.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class RdgPool
     * @brief Pool used to allocate temporary resources for rdg graph execution and reuse them between frames
    */
    class RdgPool {
    public:
        RdgPool(class GfxDriver* driver);

        void gc();

        void                set_frames_before_gc(int frames);
        GfxTextureRef       allocate_texture(const GfxTextureDesc& desc);
        void                release_texture(const GfxTextureRef& texture);
        GfxUniformBufferRef allocate_uniform_buffer(const GfxBufferDesc& desc);
        void                release_uniform_buffer(const GfxUniformBufferRef& buffer);
        GfxStorageBufferRef allocate_storage_buffer(const GfxBufferDesc& desc);
        void                release_storage_buffer(const GfxStorageBufferRef& buffer);
        ShaderParamBlockRef allocate_param_block(const ShaderParamBlockDesc& desc);
        void                release_param_block(const ShaderParamBlockRef& param_block);

    private:
        template<typename ResourceType>
        struct PoolEntry {
            Ref<ResourceType> resource;
            std::size_t       last_frame_used = 0;
            bool              is_allocated    = false;
        };

        using PoolTexture          = PoolEntry<GfxTexture>;
        using PoolUniformBuffer    = PoolEntry<GfxUniformBuffer>;
        using PoolStorageBuffer    = PoolEntry<GfxStorageBuffer>;
        using PoolShaderParamBlock = PoolEntry<ShaderParamBlock>;

        std::vector<PoolTexture>          m_texture_pool;
        std::vector<PoolUniformBuffer>    m_uniform_buffer_pool;
        std::vector<PoolStorageBuffer>    m_storage_buffer_pool;
        std::vector<PoolShaderParamBlock> m_shader_param_block_pool;

        class GfxDriver* m_driver           = nullptr;
        int              m_frames_before_gc = 4;
    };

}// namespace wmoge