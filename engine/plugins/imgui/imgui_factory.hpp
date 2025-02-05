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

#include "ui/ui_factory.hpp"

namespace wmoge {

    /**
     * @class ImguiFactory
     * @brief Ui elements factory implementation for imgui
     */
    class ImguiFactory : public UiFactory {
    public:
        ImguiFactory(class ImguiManager* manager);

        Ref<UiLayoutVertical>   make_layout_vertical() override;
        Ref<UiLayoutHorizontal> make_layout_horizontal() override;
        Ref<UiMenuAction>       make_menu_action() override;
        Ref<UiMenuGroup>        make_menu_group() override;
        Ref<UiMenu>             make_menu() override;
        Ref<UiMenuBar>          make_menu_bar() override;
        Ref<UiMainWindow>       make_main_window() override;
        Ref<UiDockWindow>       make_dock_window() override;
        Ref<UiText>             make_text() override;
        Ref<UiTextInput>        make_text_input() override;
        Ref<UiTextInputPopup>   make_text_input_popup() override;
        Ref<UiScrollArea>       make_scroll_area() override;
        Ref<UiSelectable>       make_selectable() override;

    private:
        class ImguiManager* m_manager = nullptr;
    };

}// namespace wmoge