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
#include "system/engine.hpp"

#include <cassert>
#include <cstring>
#include <type_traits>
#include <vector>

namespace wmoge {

    /**
     * @class GfxVector
     * @brief Device typed vector with managed buffer accessible for rendering
     */
    template<typename T, typename Storage>
    class GfxVector {
    public:
        GfxVector() = default;
        GfxVector(const Strid& name) : m_name(name) {}
        GfxVector(const GfxVector&) = delete;
        GfxVector(GfxVector&&)      = delete;
        ~GfxVector()                = default;

        T&       operator[](std::size_t index) { return m_data[index]; }
        const T& operator[](std::size_t index) const { return m_data[index]; }

        void push_back(const T& element);
        void resize(std::size_t size);
        void flush(GfxCmdList& cmd_list);
        void clear();
        void free();
        void set_name(const Strid& name);

        [[nodiscard]] const std::vector<T>& get_data() const { return m_data; }
        [[nodiscard]] T*                    get_mem() { return m_data.data(); }
        [[nodiscard]] const Ref<Storage>&   get_buffer() const { return m_buffer; }
        [[nodiscard]] std::size_t           get_size() const { return m_data.size(); }
        [[nodiscard]] bool                  is_empty() const { return m_data.empty(); }

    private:
        std::size_t buffer_capacity() const;
        void        buffer_reserve(std::size_t elements);

    private:
        std::vector<T> m_data;
        Ref<Storage>   m_buffer;
        Strid          m_name;
    };

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::push_back(const T& element) {
        m_data.push_back(element);
        buffer_reserve(m_data.size());
    }

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::resize(std::size_t size) {
        m_data.resize(size);
        buffer_reserve(size);
    }

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::flush(GfxCmdList& cmd_list) {
        if (!m_data.empty()) {
            void* ptr = nullptr;

            if constexpr (std::is_same_v<Storage, GfxVertBuffer>) {
                ptr = cmd_list.map_vert_buffer(m_buffer);
            }
            if constexpr (std::is_same_v<Storage, GfxIndexBuffer>) {
                ptr = cmd_list.map_index_buffer(m_buffer);
            }
            if constexpr (std::is_same_v<Storage, GfxStorageBuffer>) {
                ptr = cmd_list.map_storage_buffer(m_buffer);
            }

            assert(ptr);

            std::memcpy(ptr, m_data.data(), sizeof(T) * get_size());

            if constexpr (std::is_same_v<Storage, GfxVertBuffer>) {
                cmd_list.unmap_vert_buffer(m_buffer);
            }
            if constexpr (std::is_same_v<Storage, GfxIndexBuffer>) {
                cmd_list.unmap_index_buffer(m_buffer);
            }
            if constexpr (std::is_same_v<Storage, GfxStorageBuffer>) {
                cmd_list.unmap_storage_buffer(m_buffer);
            }
        }
    }

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::clear() {
        m_data.clear();
    }

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::free() {
        m_data.clear();
        m_buffer.reset();
    }

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::set_name(const Strid& name) {
        m_name = name;
    }

    template<typename T, typename Storage>
    std::size_t GfxVector<T, Storage>::buffer_capacity() const {
        return m_buffer ? m_buffer->size() / sizeof(T) : 0;
    }

    template<typename T, typename Storage>
    void GfxVector<T, Storage>::buffer_reserve(std::size_t elements) {
        const std::size_t capacity = buffer_capacity();

        if (capacity < elements) {
            const std::size_t new_capacity = Math::ge_pow2_val(elements);
            const int         new_size     = int(sizeof(T) * new_capacity);

            const Strid buffer_name = SID(m_name.str() + " cap=" + StringUtils::from_int(int(new_capacity)));

            if constexpr (std::is_same_v<Storage, GfxVertBuffer>) {
                m_buffer = Engine::instance()->gfx_driver()->make_vert_buffer(new_size, GfxMemUsage::GpuLocal, buffer_name);
            }
            if constexpr (std::is_same_v<Storage, GfxIndexBuffer>) {
                m_buffer = Engine::instance()->gfx_driver()->make_index_buffer(new_size, GfxMemUsage::GpuLocal, buffer_name);
            }
            if constexpr (std::is_same_v<Storage, GfxStorageBuffer>) {
                m_buffer = Engine::instance()->gfx_driver()->make_storage_buffer(new_size, GfxMemUsage::GpuLocal, buffer_name);
            }

            assert(m_buffer);
        }
    }

}// namespace wmoge