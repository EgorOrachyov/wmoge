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

#include "io/archive.hpp"
#include "io/yaml.hpp"

#include <robin_hood.hpp>
#include <unordered_map>

namespace wmoge {

#ifdef WG_DEBUG
    template<typename K, typename V>
    using flat_map = std::unordered_map<K, V>;
#else
    /**
     * @brief wrapper for fast robin_hood unordered map
     */
    template<typename K, typename V>
    using flat_map = robin_hood::unordered_flat_map<K, V>;

    template<typename K, typename V>
    Status archive_write(IoContext& context, Archive& archive, const flat_map<K, V>& map) {
        WG_ARCHIVE_WRITE(context, archive, map.size());
        for (const auto& entry : map) {
            WG_ARCHIVE_WRITE(context, archive, entry.first);
            WG_ARCHIVE_WRITE(context, archive, entry.second);
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status archive_read(IoContext& context, Archive& archive, flat_map<K, V>& map) {
        assert(map.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, archive, size);
        for (int i = 0; i < size; i++) {
            robin_hood::pair<K, V> entry;
            WG_ARCHIVE_READ(context, archive, entry.first);
            WG_ARCHIVE_READ(context, archive, entry.second);
            map.insert(std::move(entry));
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, flat_map<K, V>& map) {
        assert(map.empty());
        map.reserve(node.num_children());
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            robin_hood::pair<K, V> entry;
            WG_YAML_READ(context, child, entry);
            map.insert(std::move(entry));
        }
        return WG_OK;
    }

    template<typename K, typename V>
    Status yaml_write(IoContext& context, YamlNodeRef node, const flat_map<K, V>& map) {
        WG_YAML_SEQ(node);
        for (const auto& entry : map) {
            YamlNodeRef entry_child = node.append_child();
            WG_YAML_WRITE(context, entry_child, entry);
        }
        return WG_OK;
    }

#endif

}// namespace wmoge