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

#include "io/stream.hpp"
#include "io/tree.hpp"

#include <svector.hpp>
#include <vector>

namespace wmoge {

#ifdef WG_DEBUG
    template<typename T, std::size_t MinCapacity = 4>
    using buffered_vector = std::vector<T>;
#else
    /**
     * @brief Wrapper for ankerl vector with small vector optimization
     */
    template<typename T, std::size_t MinCapacity = 4>
    using buffered_vector = ankerl::svector<T, MinCapacity>;

    template<typename T, std::size_t MinCapacity>
    Status stream_read(IoContext& context, IoStream& stream, buffered_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, stream, size);
        vector.resize(size);
        for (int i = 0; i < size; i++) {
            WG_ARCHIVE_READ(context, stream, vector[i]);
        }
        return WG_OK;
    }

    template<typename T, std::size_t MinCapacity>
    Status stream_write(IoContext& context, IoStream& stream, const buffered_vector<T, MinCapacity>& vector) {
        WG_ARCHIVE_WRITE(context, stream, vector.size());
        for (const auto& entry : vector) {
            WG_ARCHIVE_WRITE(context, stream, entry);
        }
        return WG_OK;
    }

    template<typename T, std::size_t MinCapacity>
    Status tree_read(IoContext& context, IoTree& tree, buffered_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        vector.resize(tree.node_num_children());
        std::size_t element_id = 0;
        tree.node_find_first_child();
        for (; tree.node_is_valid(); tree.node_next_sibling()) {
            WG_TREE_READ(context, tree, vector[element_id]);
            element_id += 1;
        }
        return WG_OK;
    }

    template<typename T, std::size_t MinCapacity>
    Status tree_write(IoContext& context, IoTree& tree, const buffered_vector<T, MinCapacity>& vector) {
        WG_TREE_SEQ(tree, vector.size());
        for (const T& value : vector) {
            WG_CHECKED(tree.node_append_child());
            WG_TREE_WRITE(context, tree, value);
            tree.node_pop();
        }
        return WG_OK;
    }

#endif

}// namespace wmoge