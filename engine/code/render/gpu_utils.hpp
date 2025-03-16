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

#include "core/array_view.hpp"
#include "rdg/rdg_graph.hpp"
#include "rdg/rdg_utils.hpp"
#include "render/gpu_buffer.hpp"

namespace wmoge {

    /**
     * @class GpuUtils
     * @brief Utilities to work with gpu related structures
     */
    class GpuUtils {
    public:
        template<typename T, typename Policy>
        static RdgVertBuffer* import_vert_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer);

        template<typename T, typename Policy>
        static RdgIndexBuffer* import_index_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer);

        template<typename T, typename Policy>
        static RdgStorageBuffer* import_storage_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer);

        template<typename T, typename Policy, typename RdgBufferType>
        static void update_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer, RdgBufferType* rdg_buffer, int start_elem, int count);

        template<typename T, typename Policy, typename RdgBufferType>
        static void update_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer, RdgBufferType* rdg_buffer);
    };

    template<typename T, typename Policy>
    inline RdgVertBuffer* GpuUtils::import_vert_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer) {
        return graph.import_vert_buffer(buffer.get_buffer());
    }

    template<typename T, typename Policy>
    inline RdgIndexBuffer* GpuUtils::import_index_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer) {
        return graph.import_index_buffer(buffer.get_buffer());
    }

    template<typename T, typename Policy>
    inline RdgStorageBuffer* GpuUtils::import_storage_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer) {
        return graph.import_storage_buffer(buffer.get_buffer());
    }

    template<typename T, typename Policy, typename RdgBufferType>
    inline void GpuUtils::update_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer, RdgBufferType* rdg_buffer, int start_elem, int count) {
        if (count == 0) {
            return;
        }

        const std::uint8_t* data   = reinterpret_cast<const std::uint8_t*>(buffer.data());
        const int           offset = start_elem * sizeof(T);
        const std::size_t   size   = count * sizeof(T);

        RdgUtils::update_buffer(graph, buffer.name(), rdg_buffer, offset, {data, size});
    }

    template<typename T, typename Policy, typename RdgBufferType>
    inline void GpuUtils::update_buffer(RdgGraph& graph, GpuBuffer<T, Policy>& buffer, RdgBufferType* rdg_buffer) {
        update_buffer(graph, buffer, rdg_buffer, 0, static_cast<int>(buffer.size()));
    }

}// namespace wmoge