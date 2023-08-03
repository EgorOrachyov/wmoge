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

#ifndef WMOGE_FAST_MAP_HPP
#define WMOGE_FAST_MAP_HPP

#include "io/archive.hpp"

#include <robin_hood.hpp>
#include <unordered_map>

namespace wmoge {

#ifdef WG_DEBUG
    template<typename K, typename V>
    using fast_map = std::unordered_map<K, V>;
#else
    /**
     * @brief wrapper for fast robin_hood unordered map
     */
    template<typename K, typename V>
    using fast_map = robin_hood::unordered_flat_map<K, V>;

    template<typename K, typename V>
    bool archive_write(Archive& archive, const fast_map<K, V>& map) {
        archive << static_cast<int>(map.size());
        for (const auto& entry : map) {
            archive << entry.first;
            archive << entry.second;
        }
        return true;
    }

    template<typename K, typename V>
    bool archive_read(Archive& archive, fast_map<K, V>& map) {
        assert(map.empty());
        int size;
        archive >> size;
        for (int i = 0; i < size; i++) {
            robin_hood::pair<K, V> entry;
            archive >> entry.first;
            archive >> entry.second;
            map.insert(std::move(entry));
        }
        return true;
    }
#endif

}// namespace wmoge

#endif//WMOGE_FAST_MAP_HPP
