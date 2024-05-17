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
    Status archive_write(IoContext& context, Archive& archive, const buffered_vector<T, MinCapacity>& vector) {
        WG_ARCHIVE_WRITE(context, archive, vector.size());
        for (const auto& entry : vector) {
            WG_ARCHIVE_WRITE(context, archive, entry);
        }
        return WG_OK;
    }

    template<typename T, std::size_t MinCapacity>
    Status archive_read(IoContext& context, Archive& archive, buffered_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        std::size_t size;
        WG_ARCHIVE_READ(context, archive, size);
        vector.resize(size);
        for (int i = 0; i < size; i++) {
            WG_ARCHIVE_READ(context, archive, vector[i]);
        }
        return WG_OK;
    }

    template<typename T, std::size_t MinCapacity>
    Status yaml_write(IoContext& context, YamlNodeRef node, const buffered_vector<T, MinCapacity>& vector) {
        WG_YAML_SEQ(node);
        for (const T& value : vector) {
            YamlNodeRef child = node.append_child();
            WG_YAML_WRITE(context, child, value);
        }
        return WG_OK;
    }

    template<typename T, std::size_t MinCapacity>
    Status yaml_read(IoContext& context, YamlConstNodeRef node, buffered_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        vector.resize(node.num_children());
        std::size_t element_id = 0;
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            WG_YAML_READ(context, child, vector[element_id]);
            element_id += 1;
        }
        return WG_OK;
    }
#endif

}// namespace wmoge