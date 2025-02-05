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

#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class TPlane
     * @brief 3d-space plane
     *
     * @tparam T Type of value
     */
    template<typename T>
    class TPlane {
    public:
        TPlane() = default;

        TPlane(const TVecN<T, 3>& in_norm, T in_d) {
            norm = in_norm;
            d    = in_d;
        }

        TPlane(const TVecN<T, 3>& dir, const TVecN<T, 3>& pos) {
            // dot(x - pos, norm) == 0
            // dot(x,norm) - dot(pos,norm) == 0
            // dot(x,norm) - d == 0
            norm = dir.normalized();
            d    = TVecN<T, 3>::dot(pos, norm);
        }

        TPlane(const TVecN<T, 3>& x, const TVecN<T, 3>& y, const TVecN<T, 3>& z) {
            //      |
            //     x|_____z
            //     /
            //   y/   plane, facing top
            TVecN<T, 3> pos = x;
            TVecN<T, 3> dir = TVecN<T, 3>::cross(y - x, z - x);
            norm            = dir.normalized();
            d               = TVecN<T, 3>::dot(pos, norm);
        }

        T dot(const TVecN<T, 3>& x) const {
            return TVecN<T, 3>::dot(x, norm) - d;
        }

        bool check(const TVecN<T, 3>& x) {
            return TVecN<T, 3>::dot(x, norm) - d >= static_cast<T>(0);
        }
        bool check_strict(const TVecN<T, 3>& x) {
            return TVecN<T, 3>::dot(x, norm) - d > static_cast<T>(0);
        }

    public:
        TVecN<T, 3> norm;
        T           d;
    };

    using Planef = TPlane<float>;

}// namespace wmoge