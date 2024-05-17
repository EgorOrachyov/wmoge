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

#include "color.hpp"

namespace wmoge {

    Color4f Color::WHITE4f  = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
    Color4f Color::BLACK4f  = Color4f(0.0f, 0.0f, 0.0f, 1.0f);
    Color4f Color::RED4f    = Color4f(1.0f, 0.0f, 0.0f, 1.0f);
    Color4f Color::GREEN4f  = Color4f(0.0f, 1.0f, 0.0f, 1.0f);
    Color4f Color::BLUE4f   = Color4f(0.0f, 0.0f, 1.0f, 1.0f);
    Color4f Color::YELLOW4f = Color4f(1.0f, 1.0f, 0.0f, 1.0f);

    Color3f Color::WHITE3f  = Color3f(1.0f, 1.0f, 1.0f);
    Color3f Color::BLACK3f  = Color3f(0.0f, 0.0f, 0.0f);
    Color3f Color::RED3f    = Color3f(1.0f, 0.0f, 0.0f);
    Color3f Color::GREEN3f  = Color3f(0.0f, 1.0f, 0.0f);
    Color3f Color::BLUE3f   = Color3f(0.0f, 0.0f, 1.0f);
    Color3f Color::YELLOW3f = Color3f(1.0f, 1.0f, 0.0f);

    Color4f Color::from_hex4(unsigned int rgba) {
        const float r255 = 1.0f / 255.0f;

        unsigned char r = (rgba >> 24u) & 0xff;
        unsigned char g = (rgba >> 16u) & 0xff;
        unsigned char b = (rgba >> 8u) & 0xff;
        unsigned char a = (rgba >> 0u) & 0xff;

        return Color4f(float(r) * r255, float(g) * r255, float(b) * r255, float(a) * r255);
    }

}// namespace wmoge
