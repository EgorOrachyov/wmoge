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
#include "ui/ui_containers.hpp"

namespace wmoge {

    void imgui_process_context_menu(ImguiProcessor& processor, UiContextMenu& menu);

    void imgui_process_menu(ImguiProcessor& processor, UiMenu& menu);

    void imgui_process_popup(ImguiProcessor& processor, UiPopup& popup);

    void imgui_process_completion_popup(ImguiProcessor& processor, UiCompletionPopup& popup);

    void imgui_process_modal(ImguiProcessor& processor, UiModal& modal);

    void imgui_process_stack_panel(ImguiProcessor& processor, UiStackPanel& panel);

    void imgui_process_scroll_panel(ImguiProcessor& processor, UiScrollPanel& panel);

    void imgui_process_collapsing_panel(ImguiProcessor& processor, UiCollapsingPanel& panel);

}// namespace wmoge