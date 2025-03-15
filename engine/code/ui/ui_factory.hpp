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

#include "ui/ui_element.hpp"
#include "ui/ui_elements.hpp"
#include "ui/ui_style.hpp"

namespace wmoge {

    /**
     * @class UiFactory
     * @brief Abstract factory for ui elemenets construction
     */
    class UiFactory {
    public:
        virtual ~UiFactory() = default;

        // virtual Ref<UiLayoutVertical>   make_layout_vertical()   = 0;
        // virtual Ref<UiLayoutHorizontal> make_layout_horizontal() = 0;
        // virtual Ref<UiMenuAction>       make_menu_action()       = 0;
        // virtual Ref<UiMenuGroup>        make_menu_group()        = 0;
        // virtual Ref<UiMenu>             make_menu()              = 0;
        // virtual Ref<UiMenuBar>          make_menu_bar()          = 0;
        // virtual Ref<UiMainWindow>       make_main_window()       = 0;
        // virtual Ref<UiDockWindow>       make_dock_window()       = 0;
        // virtual Ref<UiText>             make_text()              = 0;
        // virtual Ref<UiTextInput>        make_text_input()        = 0;
        // virtual Ref<UiTextInputPopup>   make_text_input_popup()  = 0;
        // virtual Ref<UiScrollArea>       make_scroll_area()       = 0;
        // virtual Ref<UiSelectable>       make_selectable()        = 0;
        // virtual Ref<UiIconButton>       make_icon_button()       = 0;
    };

}// namespace wmoge