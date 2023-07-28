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

#ifndef WMOGE_FAST_VECTOR_HPP
#define WMOGE_FAST_VECTOR_HPP

#include "io/archive.hpp"
#include "io/yaml.hpp"

#include <svector.hpp>
#include <vector>

namespace wmoge {

#ifdef WG_DEBUG
    template<typename T, std::size_t MinCapacity = 4>
    using fast_vector = std::vector<T>;
#else
    /**
     * @class fast_vector
     * @brief Wrapper for ankerl vector with small vector optimization
     */
    template<typename T, std::size_t MinCapacity = 4>
    using fast_vector = ankerl::svector<T, MinCapacity>;

    template<typename T, std::size_t MinCapacity>
    bool archive_write(Archive& archive, const fast_vector<T, MinCapacity>& vector) {
        if (!archive_write(archive, vector.size())) {
            return false;
        }
        for (const auto& entry : vector) {
            if (!archive_write(archive, entry)) {
                return false;
            }
        }
        return true;
    }

    template<typename T, std::size_t MinCapacity>
    bool archive_read(Archive& archive, fast_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        std::size_t size;
        if (!archive_read(archive, size)) {
            return false;
        }
        vector.resize(size);
        for (std::size_t i = 0; i < size; i++) {
            if (!archive_read(archive, vector[i])) {
                return false;
            }
        }
        return true;
    }

    template<typename T, std::size_t MinCapacity>
    bool yaml_write(YamlNodeRef node, const fast_vector<T, MinCapacity>& vector) {
        WG_YAML_SEQ(node);
        for (const auto& value : vector) {
            if (!yaml_write(node.append_child(), value)) {
                return false;
            }
        }
        return true;
    }

    template<typename T, std::size_t MinCapacity>
    bool yaml_read(const YamlConstNodeRef& node, fast_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        vector.resize(node.num_children());
        std::size_t element_id = 0;
        for (auto child = node.first_child(); child.valid(); child = child.next_sibling()) {
            if (!yaml_read(node, vector[element_id++])) {
                return false;
            }
        }
        return true;
    }
#endif

}// namespace wmoge

#endif//WMOGE_FAST_VECTOR_HPP
