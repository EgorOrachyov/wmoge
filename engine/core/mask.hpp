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

#ifndef WMOGE_MASK_HPP
#define WMOGE_MASK_HPP

#include <bitset>
#include <initializer_list>

namespace wmoge {

    /**
     * @class Mask
     * @brief Typed mask to test safely enum flags
     *
     * @tparam T type of flags
     * @tparam size flags count (default is 32)
     */
    template<typename T, int size = 32>
    struct Mask {
        Mask() = default;

        Mask(const std::initializer_list<T>& elements) {
            for (auto e : elements) set(e);
        }

        bool get(T flag) const { return bits.test(static_cast<int>(flag)); }
        void set(T flag, bool value = true) { bits.set(static_cast<int>(flag), value); }

        std::bitset<size> bits;
    };

    template<typename Stream, typename T, int size>
    Stream& operator<<(Stream& stream, const Mask<T, size>& mask) {
        stream << mask.bits;
        return stream;
    }

}// namespace wmoge

#endif//WMOGE_MASK_HPP
