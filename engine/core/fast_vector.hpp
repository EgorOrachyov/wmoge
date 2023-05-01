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
    Archive& operator<<(Archive& archive, const fast_vector<T, MinCapacity>& vector) {
        archive << static_cast<int>(vector.size());
        for (const auto& entry : vector) {
            archive << entry;
        }
        return archive;
    }

    template<typename T, std::size_t MinCapacity>
    Archive& operator>>(Archive& archive, fast_vector<T, MinCapacity>& vector) {
        assert(vector.empty());
        int size;
        archive >> size;
        vector.resize(size);
        for (int i = 0; i < size; i++) {
            archive >> vector[i];
        }
        return archive;
    }
#endif

}// namespace wmoge

#endif//WMOGE_FAST_VECTOR_HPP
