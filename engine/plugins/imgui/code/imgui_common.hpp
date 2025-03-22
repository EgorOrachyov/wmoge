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
#include "ui/ui_defs.hpp"

#include <imgui.h>

#include <string>

namespace wmoge {

    inline const char* imgui_str(const std::string& s) {
        return s.c_str();
    }

    inline const char* imgui_str(const Strid& s) {
        return s.str().c_str();
    }

    inline ImVec2 imgui_vec2(const Vec2f& v) {
        return ImVec2(v[0], v[1]);
    }

    inline Vec2f imgui_vec2(const ImVec2& v) {
        return Vec2f(v[0], v[1]);
    }

    inline ImVec4 imgui_vec4(const Vec4f& v) {
        return ImVec4(v[0], v[1], v[2], v[3]);
    }

    inline ImVec4 imgui_color4(const Color4f& v) {
        return ImVec4(v[0], v[1], v[2], v[3]);
    }

    inline Color4f imgui_color4(const ImVec4& v) {
        return Color4f(v.x, v.y, v.z, v.w);
    }

    inline ImGuiDir imgui_dir(UiDir dir) {
        switch (dir) {
            case UiDir::None:
                return ImGuiDir_None;
            case UiDir::Left:
                return ImGuiDir_Left;
            case UiDir::Right:
                return ImGuiDir_Right;
            case UiDir::Up:
                return ImGuiDir_Up;
            case UiDir::Down:
                return ImGuiDir_Down;

            default:
                return ImGuiDir_None;
        }
    }

    inline UiDir imgui_dir(ImGuiDir dir) {
        switch (dir) {
            case ImGuiDir_None:
                return UiDir::None;
            case ImGuiDir_Left:
                return UiDir::Left;
            case ImGuiDir_Right:
                return UiDir::Right;
            case ImGuiDir_Up:
                return UiDir::Up;
            case ImGuiDir_Down:
                return UiDir::Down;

            default:
                return UiDir::None;
        }
    }

}// namespace wmoge