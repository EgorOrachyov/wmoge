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

#include "imgui_common.hpp"
#include "imgui_process.hpp"
#include "ui/ui_inputs.hpp"

namespace wmoge {

    void imgui_process_drag_int(ImguiProcessor& processor, UiDragInt& element);

    void imgui_process_drag_float(ImguiProcessor& processor, UiDragFloat& element);

    void imgui_process_slider_int(ImguiProcessor& processor, UiSliderInt& element);

    void imgui_process_slider_float(ImguiProcessor& processor, UiSliderFloat& element);

    void imgui_process_input_int(ImguiProcessor& processor, UiInputInt& element);

    void imgui_process_input_float(ImguiProcessor& processor, UiInputFloat& element);

    void imgui_process_input_text(ImguiProcessor& processor, UiInputText& element);

    void imgui_process_input_text_ext(ImguiProcessor& processor, UiInputTextExt& element);

}// namespace wmoge