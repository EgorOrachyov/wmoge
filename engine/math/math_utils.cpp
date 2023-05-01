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

#include "math_utils.hpp"

#ifndef _WMOGE_MATH_PI
    #define _WMOGE_MATH_PI 3.14159265359
#endif

#ifndef _WMOGE_MATH_PI_2
    #define _WMOGE_MATH_PI_2 1.57079632679
#endif

#ifndef _WMOGE_MATH_PI_4
    #define _WMOGE_MATH_PI_4 0.78539816339
#endif

#ifndef _WMOGE_MATH_SQRT2
    #define _WMOGE_MATH_SQRT2 1.41421356237
#endif

#ifndef _WMOGE_MATH_E
    #define _WMOGE_MATH_E 2.71828182846
#endif

namespace wmoge {

    const float Math::BIG_NUMBER_FLOAT32   = 3.4e+38f;
    const float Math::SMALL_NUMBER_FLOAT32 = 1.e-8f;

    const float Math::THRESH_FLOAT32           = 0.0001f;
    const float Math::THRESH_POINT_ON_PLANE    = 0.10f;
    const float Math::THRESH_POINTS_ARE_SAME   = 0.00002f;
    const float Math::THRESH_ZERO_NORM_SQUARED = 0.0001f;

    const float Math::THRESH_COMPARE_FLOAT32 = 0.0001f;
    const float Math::THRESH_COMPARE_FLOAT64 = 0.000001f;

    const double Math::PI         = _WMOGE_MATH_PI;
    const double Math::HALF_PI    = _WMOGE_MATH_PI_2;
    const double Math::QUARTER_PI = _WMOGE_MATH_PI_4;
    const double Math::SQRT2      = _WMOGE_MATH_SQRT2;
    const double Math::E          = _WMOGE_MATH_E;

    const float Math::PIf         = static_cast<float>(_WMOGE_MATH_PI);
    const float Math::HALF_PIf    = static_cast<float>(_WMOGE_MATH_PI_2);
    const float Math::QUARTER_PIf = static_cast<float>(_WMOGE_MATH_PI_4);
    const float Math::SQRT2f      = static_cast<float>(_WMOGE_MATH_SQRT2);
    const float Math::Ef          = static_cast<float>(_WMOGE_MATH_E);

}// namespace wmoge
