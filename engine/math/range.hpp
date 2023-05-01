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

#ifndef WMOGE_RANGE_HPP
#define WMOGE_RANGE_HPP

#include "math/math_utils.hpp"

namespace wmoge {

    /**
     * @class Range
     * @brief Numeric range to simplify iteration
     */
    class Range {
    public:
        class Iterator {
        public:
            Iterator(int from) : m_current(from) {}
            bool operator!=(const Iterator& other) { return m_current != other.m_current; }
            int  operator*() const { return m_current; }
            void operator++() { m_current += 1; }

        private:
            int m_current = -1;
        };

        Range() = default;
        Range(int from, int to) : m_from(from), m_to(to) {}
        Iterator begin() const { return Iterator(m_from); }
        Iterator end() const { return Iterator(m_to); }

        static Range make_range(int idx, int range_size, int divider) {
            if (divider > range_size) {
                return idx == 0 ? Range(0, range_size) : Range();
            }
            const int slice_step = range_size / divider;
            const int slice_size = slice_step + ((idx + 1) == divider ? range_size % divider : 0);
            return Range(slice_step * idx, slice_step * idx + slice_size);
        }

    private:
        int m_from = -1;
        int m_to   = -1;
    };

}// namespace wmoge

#endif//WMOGE_RANGE_HPP
