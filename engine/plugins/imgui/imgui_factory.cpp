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

#include "imgui_factory.hpp"

#include "imgui_elements.hpp"

namespace wmoge {

    ImguiFactory::ImguiFactory(ImguiManager* manager) : m_manager(manager) {
    }

    Ref<UiLayoutVertical> ImguiFactory::make_layout_vertical() {
        return make_ref<ImguiLayoutVertical>(m_manager);
    }

    Ref<UiLayoutHorizontal> ImguiFactory::make_layout_horizontal() {
        return make_ref<ImguiLayoutHorizontal>(m_manager);
    }

    Ref<UiMenuAction> ImguiFactory::make_menu_action() {
        return make_ref<ImguiMenuAction>(m_manager);
    }

    Ref<UiMenuGroup> ImguiFactory::make_menu_group() {
        return make_ref<ImguiMenuGroup>(m_manager);
    }

    Ref<UiMenu> ImguiFactory::make_menu() {
        return make_ref<ImguiMenu>(m_manager);
    }

    Ref<UiMenuBar> ImguiFactory::make_menu_bar() {
        return make_ref<ImguiMenuBar>(m_manager);
    }

    Ref<UiMainWindow> ImguiFactory::make_main_window() {
        return make_ref<ImguiMainWindow>(m_manager);
    }

    Ref<UiDockWindow> ImguiFactory::make_dock_window() {
        return make_ref<ImguiDockWindow>(m_manager);
    }

    Ref<UiText> ImguiFactory::make_text() {
        return make_ref<ImguiText>(m_manager);
    }

    Ref<UiTextInput> ImguiFactory::make_text_input() {
        return make_ref<ImguiTextInput>(m_manager);
    }

    Ref<UiTextInputPopup> ImguiFactory::make_text_input_popup() {
        return make_ref<ImguiTextInputPopup>(m_manager);
    }

    Ref<UiScrollArea> ImguiFactory::make_scroll_area() {
        return make_ref<ImguiScrollArea>(m_manager);
    }

    Ref<UiSelectable> ImguiFactory::make_selectable() {
        return make_ref<ImguiSelectable>(m_manager);
    }

}// namespace wmoge