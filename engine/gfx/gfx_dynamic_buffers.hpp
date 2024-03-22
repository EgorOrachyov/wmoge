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

#include "core/fast_vector.hpp"
#include "core/synchronization.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_defs.hpp"
#include "gfx/gfx_resource.hpp"

#include <utility>

namespace wmoge {

    /**
     * @class GfxUniformPool
     * @brief Pool for per-frame small const buffer allocations to configure render passes
     */
    class GfxUniformPool : public GfxResource {
    public:
        GfxUniformPool(const Strid& name);
        ~GfxUniformPool() override = default;

        virtual GfxUniformBufferSetup allocate(int constants_size, const void* mem);
        virtual void                  configure(GfxUniformBufferSetup& setup, int constants_size, const void* mem);
        virtual void                  resycle_allocations();

        template<typename ConstantsStructure>
        GfxUniformBufferSetup allocate(const ConstantsStructure& constants) {
            return allocate(int(sizeof(ConstantsStructure)), &constants);
        }

        template<typename ConstantsStructure>
        GfxUniformBufferSetup configure(GfxUniformBufferSetup& setup, const ConstantsStructure& constants) {
            return configure(setup, int(sizeof(ConstantsStructure)), &constants);
        }

    private:
        struct Bucket {
            std::vector<Ref<GfxUniformBuffer>> buffers;
            int                                next = 0;
        };

        fast_vector<Bucket> m_buckets;

        mutable SpinMutex m_mutex;
    };

    /**
     * @class GfxDynAllocation
     * @brief Allocation of dynamic buffer
     */
    template<typename GfxBufferType, typename CpuPtrType>
    struct GfxDynAllocation {
        GfxBufferType* buffer = nullptr;
        CpuPtrType*    ptr    = nullptr;
        int            offset = 0;
    };

    /**
     * @class GfxDynBuffer
     * @brief Dynamic buffer for frequently changed data
     *
     * Usage pattern:
     *  - Allocate dynamic chunk specifying required size to allocate
     *  - Fill data into obtained pointer which must be rendered
     *  - Flush all allocations
     *  - Use buffer for rendering before recycle is called
     */
    class GfxDynBuffer : public GfxResource {
    public:
        GfxDynBuffer(int size, int alignment, const Strid& name);
        ~GfxDynBuffer() override = default;

        virtual GfxDynAllocation<GfxBuffer, void> allocate_base(int bytes_to_allocate);
        virtual void                              flush();
        virtual void                              recycle();

        template<typename GfxBufferType, typename BufferStructure>
        GfxDynAllocation<GfxBufferType, BufferStructure> allocate_base() {
            const GfxDynAllocation<GfxBuffer, void> chunk = allocate_base(sizeof(BufferStructure));
            return {(GfxBufferType*) chunk.buffer, (BufferStructure*) chunk.ptr, chunk.offset};
        }

        template<typename GfxBufferType, typename ElementStructure>
        GfxDynAllocation<GfxBufferType, ElementStructure> allocate_base_n(int num_elements) {
            const GfxDynAllocation<GfxBuffer, void> chunk = allocate_base(sizeof(ElementStructure) * num_elements);
            return {(GfxBufferType*) chunk.buffer, (ElementStructure*) chunk.ptr, chunk.offset};
        }

    protected:
        virtual Ref<GfxBuffer> make_buffer(int size, const Strid& name)   = 0;
        virtual void*          map_buffer(const Ref<GfxBuffer>& buffer)   = 0;
        virtual void           unmap_buffer(const Ref<GfxBuffer>& buffer) = 0;

    protected:
        struct Chunk {
            Ref<GfxBuffer> buffer;
            void*          mapping = nullptr;
            int            offset  = 0;
        };

        fast_vector<Chunk> m_chunks;
        int                m_current_chunk      = 0;
        int                m_alignment          = 0;
        int                m_default_chunk_size = 0;

        mutable SpinMutex m_mutex;
    };

    /**
     * @class GfxDynVertBuffer
     * @brief Dynamic vertex buffer for immediate and frequently changed geometry
     */
    class GfxDynVertBuffer : public GfxDynBuffer {
    public:
        GfxDynVertBuffer(int size, int alignment, const Strid& name);
        ~GfxDynVertBuffer() override = default;

        GfxDynAllocation<GfxVertBuffer, void> allocate(int bytes_to_allocate) {
            auto allocation = allocate_base(bytes_to_allocate);
            return {(GfxVertBuffer*) allocation.buffer, allocation.ptr, allocation.offset};
        }

        template<typename BufferStructure>
        GfxDynAllocation<GfxVertBuffer, BufferStructure> allocate() {
            return allocate_base<GfxVertBuffer, BufferStructure>();
        }

        template<typename ElementStructure>
        GfxDynAllocation<GfxVertBuffer, ElementStructure> allocate_n(int num_elements) {
            return allocate_base_n<GfxVertBuffer, ElementStructure>(num_elements);
        }

    protected:
        Ref<GfxBuffer> make_buffer(int size, const Strid& name) override;
        void*          map_buffer(const Ref<GfxBuffer>& buffer) override;
        void           unmap_buffer(const Ref<GfxBuffer>& buffer) override;
    };

    /**
     * @class GfxDynIndexBuffer
     * @brief Dynamic index buffer for immediate and frequently changed geometry
     */
    class GfxDynIndexBuffer : public GfxDynBuffer {
    public:
        GfxDynIndexBuffer(int size, int alignment, const Strid& name);
        ~GfxDynIndexBuffer() override = default;

        template<typename BufferStructure>
        GfxDynAllocation<GfxIndexBuffer, BufferStructure> allocate() {
            return allocate_base<GfxIndexBuffer, BufferStructure>();
        }

        template<typename ElementStructure>
        GfxDynAllocation<GfxIndexBuffer, ElementStructure> allocate_n(int num_elements) {
            return allocate_base_n<GfxIndexBuffer, ElementStructure>(num_elements);
        }

    protected:
        Ref<GfxBuffer> make_buffer(int size, const Strid& name) override;
        void*          map_buffer(const Ref<GfxBuffer>& buffer) override;
        void           unmap_buffer(const Ref<GfxBuffer>& buffer) override;
    };

    /**
     * @class GfxDynUniformBuffer
     * @brief Dynamic uniform buffer for constants allocation for rendering
     */
    class GfxDynUniformBuffer : public GfxDynBuffer {
    public:
        GfxDynUniformBuffer(int size, int alignment, const Strid& name);
        ~GfxDynUniformBuffer() override = default;

        template<typename BufferStructure>
        GfxDynAllocation<GfxUniformBuffer, BufferStructure> allocate() {
            return allocate_base<GfxUniformBuffer, BufferStructure>();
        }

        template<typename ElementStructure>
        GfxDynAllocation<GfxUniformBuffer, ElementStructure> allocate_n(int num_elements) {
            return allocate_base_n<GfxUniformBuffer, ElementStructure>(num_elements);
        }

    protected:
        Ref<GfxBuffer> make_buffer(int size, const Strid& name) override;
        void*          map_buffer(const Ref<GfxBuffer>& buffer) override;
        void           unmap_buffer(const Ref<GfxBuffer>& buffer) override;
    };

}// namespace wmoge