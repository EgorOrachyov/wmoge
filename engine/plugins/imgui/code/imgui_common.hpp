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

#include "core/string_id.hpp"
#include "math/vec.hpp"
#include "ui/ui_attribute.hpp"

#include <imgui.h>

#include <string>

namespace wmoge {

    inline const char* imgui_str(const UiAttribute<std::string>& s) {
        return s.get().c_str();
    }

    inline const char* imgui_str(const UiAttribute<Strid>& s) {
        return s.get().str().c_str();
    }

    inline ImVec2 imgui_vec2(const Vec2f& v) {
        return ImVec2(v[0], v[1]);
    }

    inline ImVec4 imgui_vec4(const Vec4f& v) {
        return ImVec4(v[0], v[1], v[2], v[3]);
    }

    inline ImVec4 imgui_color4(const Color4f& v) {
        return ImVec4(v[0], v[1], v[2], v[3]);
    }

}// namespace wmoge