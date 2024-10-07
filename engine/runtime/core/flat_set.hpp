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

#include <robin_hood.hpp>
#include <unordered_set>

namespace wmoge {

#ifdef WG_DEBUG
    template<typename K>
    using flat_set = std::unordered_set<K>;
#else
    /**
     * @brief wrapper for fast robin_hood unordered set
     */
    template<typename K>
    using flat_set = robin_hood::unordered_flat_set<K>;

    template<typename K>
    Status stream_write(IoContext& context, IoStream& stream, const flat_set<K>& set) {
        WG_ARCHIVE_WRITE(context, stream, set.size());
        for (const auto& entry : set) {
            WG_ARCHIVE_WRITE(context, stream, entry);
        }
        return WG_OK;
    }

    template<typename K>
    Status stream_read(IoContext& context, IoStream& stream, flat_set<K>& set) {
        assert(set.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, stream, size);
        for (int i = 0; i < size; i++) {
            K entry;
            WG_ARCHIVE_READ(context, stream, entry);
            set.insert(std::move(entry));
        }
        return WG_OK;
    }

    template<typename K>
    Status tree_read(IoContext& context, IoTree& tree, flat_set<K>& set) {
        assert(set.empty());
        set.reserve(tree.node_num_children());
        tree.node_find_first_child();
        for (; tree.node_is_valid(); tree.node_next_sibling()) {
            K entry;
            WG_TREE_READ(context, tree, entry);
            set.insert(std::move(entry));
        }
        return WG_OK;
    }

    template<typename K>
    Status tree_write(IoContext& context, IoTree& tree, const flat_set<K>& set) {
        WG_TREE_SEQ(tree, set.size());
        for (const auto& entry : set) {
            WG_CHECKED(tree.node_append_child());
            WG_TREE_WRITE(context, tree, entry);
            tree.node_pop();
        }
        return WG_OK;
    }

#endif

}// namespace wmoge