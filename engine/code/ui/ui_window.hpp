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
        WG_RTTI_CLASS(UiWindow, UiElement);

        using UiElement::UiElement;

        std::string       title;
        Ref<UiMenuBar>    menu_bar;
        Ref<UiToolBar>    tool_bar;
        Ref<UiStatusBar>  status_bar;
        Ref<UiSubElement> content;
        UiWindowFlags     flags;
        bool              is_open{true};
    };

    WG_RTTI_CLASS_BEGIN(UiWindow) {
        WG_RTTI_FIELD(title, {});
        WG_RTTI_FIELD(menu_bar, {});
        WG_RTTI_FIELD(tool_bar, {});
        WG_RTTI_FIELD(status_bar, {});
        WG_RTTI_FIELD(content, {});
        WG_RTTI_FIELD(flags, {});
        WG_RTTI_FIELD(is_open, {});
    }
    WG_RTTI_END;

    /**
     * @class UiMainWindow
     * @brief
     */
    class UiMainWindow : public UiWindow {
    public:
        WG_RTTI_CLASS(UiMainWindow, UiWindow);

        UiMainWindow() : UiWindow(UiElementType::MainWindow) {}
    };

    WG_RTTI_CLASS_BEGIN(UiMainWindow) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class UiDockWindow
     * @brief
     */
    class UiDockWindow : public UiWindow {
    public:
        WG_RTTI_CLASS(UiDockWindow, UiWindow);

        UiDockWindow() : UiWindow(UiElementType::DockWindow) {}
    };

    WG_RTTI_CLASS_BEGIN(UiDockWindow) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class UiDockSpace
     * @brief
     */
    class UiDockSpace : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiDockSpace, UiSubElement);

        UiDockSpace() : UiSubElement(UiElementType::DockSpace) {}

        using Slot = Ref<UiDockWindow>;

        std::string       name;
        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiDockSpace) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

}// namespace wmoge