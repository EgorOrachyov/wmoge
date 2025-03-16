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

#include "ui/ui_attribute.hpp"
#include "ui/ui_bars.hpp"
#include "ui/ui_containers.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiWindow
     * @brief
     */
    class UiWindow : public UiElement {
    public:
        using UiElement::UiElement;

        UiAttribute<std::string>   title;
        UiSlot<UiMenuBar>          menu_bar;
        UiSlot<UiToolBar>          tool_bar;
        UiSlot<UiStatusBar>        status_bar;
        UiSlot<UiPanel>            panel;
        UiAttribute<UiWindowFlags> flags;
        UiAttribute<bool>          is_open{true};
        UiAttribute<bool>          is_fullscreen{true};
    };

    /**
     * @class UiMainWindow
     * @brief
     */
    class UiMainWindow : public UiWindow {
    public:
        UiMainWindow() : UiWindow(UiElementType::MainWindow) {}
    };

    /**
     * @class UiDockWindow
     * @brief
     */
    class UiDockWindow : public UiWindow {
    public:
        UiDockWindow() : UiWindow(UiElementType::DockWindow) {}
    };

}// namespace wmoge