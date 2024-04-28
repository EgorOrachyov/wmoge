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
#include "gfx/gfx_buffers.hpp"
#include "gfx/vulkan/vk_defs.hpp"
#include "gfx/vulkan/vk_resource.hpp"

namespace wmoge {

    /**
     * @class VKBuffer
     * @brief Base class for vulkan buffers
     */
    class VKBuffer {
    public:
        void                    init(VkDeviceSize size, VkBufferUsageFlags flags, GfxMemUsage usage);
        void                    release();
        void*                   map();
        virtual void            unmap(VkCommandBuffer cmd);
        virtual void            update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, const Ref<Data>& mem);
        virtual class VKDriver& driver() = 0;

        VkDeviceSize  size() const { return m_size; }
        VkBuffer      buffer() const { return m_buffer; }
        VmaAllocation allocation() const { return m_allocation; }

    protected:
        void barrier(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, VkAccessFlags dst_access, VkPipelineStageFlags dst_pipeline_stage);

        VkDeviceSize  m_size               = 0;
        VkBuffer      m_buffer             = VK_NULL_HANDLE;
        VmaAllocation m_allocation         = VK_NULL_HANDLE;
        VkBuffer      m_staging_buffer     = VK_NULL_HANDLE;
        VmaAllocation m_staging_allocation = VK_NULL_HANDLE;
    };

    /**
     * @class VKVertBuffer
     * @brief Gfx vulkan vertex buffer implementation
     */
    class VKVertBuffer final : public VKResource<GfxVertBuffer>, public VKBuffer {
    public:
        VKVertBuffer(class VKDriver& driver);
        ~VKVertBuffer() override;

        void            create(int size, GfxMemUsage usage, const Strid& name);
        void            unmap(VkCommandBuffer cmd) override;
        void            update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, const Ref<Data>& mem) override;
        class VKDriver& driver() override { return m_driver; }
    };

    /**
     * @class VKIndexBuffer
     * @brief Gfx vulkan index buffer implementation
     */
    class VKIndexBuffer final : public VKResource<GfxIndexBuffer>, public VKBuffer {
    public:
        VKIndexBuffer(class VKDriver& driver);
        ~VKIndexBuffer() override;

        void            create(int size, GfxMemUsage usage, const Strid& name);
        void            unmap(VkCommandBuffer cmd) override;
        void            update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, const Ref<Data>& mem) override;
        class VKDriver& driver() override { return m_driver; }
    };

    /**
     * @class VKUniformBuffer
     * @brief Gfx vulkan uniform buffer implementation
     */
    class VKUniformBuffer final : public VKResource<GfxUniformBuffer>, public VKBuffer {
    public:
        VKUniformBuffer(class VKDriver& driver);
        ~VKUniformBuffer() override;

        void            create(int size, GfxMemUsage usage, const Strid& name);
        void            unmap(VkCommandBuffer cmd) override;
        void            update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, const Ref<Data>& mem) override;
        class VKDriver& driver() override { return m_driver; }
    };

    /**
     * @class VKStorageBuffer
     * @brief Gfx vulkan storage buffer implementation
     */
    class VKStorageBuffer final : public VKResource<GfxStorageBuffer>, public VKBuffer {
    public:
        VKStorageBuffer(class VKDriver& driver);
        ~VKStorageBuffer() override;

        void            create(int size, GfxMemUsage usage, const Strid& name);
        void            unmap(VkCommandBuffer cmd) override;
        void            update(VkCommandBuffer cmd, VkDeviceSize offset, VkDeviceSize size, const Ref<Data>& mem) override;
        void            barrier(VkCommandBuffer cmd);
        class VKDriver& driver() override { return m_driver; }
    };

}// namespace wmoge