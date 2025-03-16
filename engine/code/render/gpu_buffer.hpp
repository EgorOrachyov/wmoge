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

#include "core/string_utils.hpp"
#include "gfx/gfx_buffers.hpp"
#include "gfx/gfx_cmd_list.hpp"
#include "gfx/gfx_driver.hpp"
#include "math/math_utils.hpp"

#include <cassert>
#include <cstring>
#include <type_traits>
#include <vector>

namespace wmoge {

    /** @brief Policy for data duplicated on CPU side */
    template<typename T>
    struct GpuHostData {
        static constexpr bool ENABLE = true;

        array_view<const std::uint8_t> to_buffer() const {
            return {static_cast<std::uint8_t*>(elements.data()), elements.size() * sizeof(T)};
        }

        std::vector<T> elements;
    };

    /** @brief Policy with no CPU data */
    struct GpuHostDataEmpty {
        static constexpr bool ENABLE = false;
    };

    /** @brief Policy to allocate vertex buffer */
    struct GpuMemVert {
        using BufferType = GfxVertBuffer;

        void make(GfxDriver* driver, int size, GfxMemUsage mem_usage, const Strid& name) {
            buffer = driver->make_vert_buffer(size, mem_usage, name);
        }
        void update(GfxCmdList* cmd_list, const array_view<const std::uint8_t>& data) {
            cmd_list->update_vert_buffer(buffer, 0, static_cast<int>(data.size()), data);
        }

        GfxVertBufferRef buffer;
    };

    /** @brief Policy to allocate index buffer */
    struct GpuMemIndex {
        using BufferType = GfxIndexBuffer;

        void make(GfxDriver* driver, int size, GfxMemUsage mem_usage, const Strid& name) {
            buffer = driver->make_index_buffer(size, mem_usage, name);
        }
        void update(GfxCmdList* cmd_list, const array_view<const std::uint8_t>& data) {
            cmd_list->update_index_buffer(buffer, 0, static_cast<int>(data.size()), data);
        }

        GfxIndexBufferRef buffer;
    };

    /** @brief Policy to allocate storage buffer */
    struct GpuMemStorage {
        using BufferType = GfxStorageBuffer;

        void make(GfxDriver* driver, int size, GfxMemUsage mem_usage, const Strid& name) {
            buffer = driver->make_storage_buffer(size, mem_usage, name);
        }
        void update(GfxCmdList* cmd_list, const array_view<const std::uint8_t>& data) {
            cmd_list->update_storage_buffer(buffer, 0, static_cast<int>(data.size()), data);
        }

        GfxStorageBufferRef buffer;
    };

    /** @brief Policy for host-cached vertex buffer */
    template<typename T>
    struct GpuPolicyHostVert {
        static constexpr GfxMemUsage MEM_USAGE = GfxMemUsage::GpuLocal;
        using HostData                         = GpuHostData<T>;
        using MemStorage                       = GpuMemVert;
    };

    /** @brief Policy for host-cached index buffer */
    template<typename T>
    struct GpuPolicyHostIndex {
        static constexpr GfxMemUsage MEM_USAGE = GfxMemUsage::GpuLocal;
        using HostData                         = GpuHostData<T>;
        using MemStorage                       = GpuMemIndex;
    };

    /** @brief Policy for host-cached storage buffer */
    template<typename T>
    struct GpuPolicyHostStorage {
        static constexpr GfxMemUsage MEM_USAGE = GfxMemUsage::GpuLocal;
        using HostData                         = GpuHostData<T>;
        using MemStorage                       = GpuMemStorage;
    };

    /** @brief General purpose typed gpu buffer */
    template<typename T, typename Policy>
    class GpuBuffer {
    public:
        static constexpr GfxMemUsage MEM_USAGE = Policy::MEM_USAGE;
        using HostData                         = typename Policy::HostData;
        using MemStorage                       = typename Policy::MemStorage;
        using BufferType                       = typename MemStorage::BufferType;

        GpuBuffer(const Strid& name) : m_name(name) {}

        GpuBuffer()  = default;
        ~GpuBuffer() = default;

        GpuBuffer(const GpuBuffer&)            = delete;
        GpuBuffer& operator=(const GpuBuffer&) = delete;

        GpuBuffer(GpuBuffer&&)            = delete;
        GpuBuffer& operator=(GpuBuffer&&) = delete;

        T&       operator[](std::size_t index) { return m_data.elements[index]; }
        const T& operator[](std::size_t index) const { return m_data.elements[index]; }

        void push_back(const T& element);
        void resize(std::size_t size);
        void clear();
        void free();
        void set_name(const Strid& name);

        void reserve(GfxDriver* driver);
        void flush(const GfxCmdListRef& cmd_list);

        [[nodiscard]] const Ref<BufferType>& get_buffer() const { return m_buffer.buffer; }
        [[nodiscard]] std::size_t            size() const { return m_data.elements.size(); }
        [[nodiscard]] bool                   empty() const { return m_data.elements.empty(); }
        [[nodiscard]] const T*               data() const { return m_data.elements.data(); }
        [[nodiscard]] const Strid&           name() const { return m_name; }
        [[nodiscard]] std::size_t            buffer_capacity() const;

    private:
        HostData   m_data;
        MemStorage m_buffer;
        Strid      m_name;
    };

    template<typename T>
    using GpuVertBuffer = GpuBuffer<T, GpuPolicyHostVert<T>>;

    template<typename T>
    using GpuIndexBuffer = GpuBuffer<T, GpuPolicyHostIndex<T>>;

    template<typename T>
    using GpuStorageBuffer = GpuBuffer<T, GpuPolicyHostStorage<T>>;

    template<typename T, typename Policy>
    void GpuBuffer<T, Policy>::push_back(const T& element) {
        m_data.elements.push_back(element);
    }

    template<typename T, typename Policy>
    void GpuBuffer<T, Policy>::resize(std::size_t size) {
        m_data.elements.resize(size);
    }

    template<typename T, typename Policy>
    inline void GpuBuffer<T, Policy>::reserve(GfxDriver* driver) {
        if (buffer_capacity() >= size()) {
            return;
        }
        const std::size_t new_capacity = Math::ge_pow2_val(size());
        const int         new_size     = static_cast<int>(sizeof(T) * new_capacity);
        const Strid       buffer_name  = SIDDBG(m_name.str() + " cap=" + StringUtils::from_int(static_cast<int>(new_capacity)));

        m_buffer.make(driver, new_size, MEM_USAGE, buffer_name);
    }

    template<typename T, typename Policy>
    void GpuBuffer<T, Policy>::flush(const GfxCmdListRef& cmd_list) {
        if (empty()) {
            return;
        }
        m_buffer.update(cmd_list, m_data.to_buffer());
    }

    template<typename T, typename Policy>
    void GpuBuffer<T, Policy>::clear() {
        m_data.elements.clear();
    }

    template<typename T, typename Policy>
    void GpuBuffer<T, Policy>::free() {
        m_data.elements.clear();
        m_buffer.buffer.reset();
    }

    template<typename T, typename Policy>
    void GpuBuffer<T, Policy>::set_name(const Strid& name) {
        m_name = name;
    }

    template<typename T, typename Policy>
    std::size_t GpuBuffer<T, Policy>::buffer_capacity() const {
        return m_buffer.buffer ? m_buffer.buffer->size() / sizeof(T) : 0;
    }

}// namespace wmoge