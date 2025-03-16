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
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiContextMenu
     * @brief
     */
    class UiContextMenu : public UiElement {
    public:
        UiContextMenu() : UiElement(UiElementType::ContextMenu) {}

        using Slot = UiSlot<UiSubElement>;

        UiSlots<Slot> children;
    };

    /**
     * @class UiMenu
     * @brief
     */
    class UiMenu : public UiSubElement {
    public:
        UiMenu() : UiSubElement(UiElementType::Menu) {}

        using Slot = UiSlot<UiSubElement>;

        UiAttribute<std::string> label;
        UiAttribute<bool>        enabled{true};
        UiSlots<Slot>            children;
    };

    /**
     * @class UiPopup
     * @brief
     */
    class UiPopup : public UiElement {
    public:
        UiPopup() : UiElement(UiElementType::Popup) {}

        using Slot = UiSlot<UiSubElement>;

        UiAttribute<std::string> name;
        UiAttribute<bool>        should_show{false};
        UiSlots<Slot>            children;
    };

    /**
     * @class UiCompletionPopup
     * @brief
     */
    class UiCompletionPopup : public UiElement {
    public:
        UiCompletionPopup() : UiElement(UiElementType::CompletionPopup) {}

        using Slot = UiSlot<UiSubElement>;

        UiAttribute<std::string> name;
        UiAttribute<bool>        should_show{false};
        UiSlots<Slot>            children;
    };

    /**
     * @class UiModal
     * @brief
     */
    class UiModal : public UiElement {
    public:
        UiModal() : UiElement(UiElementType::Modal) {}

        using Slot = UiSlot<UiSubElement>;

        UiAttribute<std::string> name;
        UiAttribute<bool>        should_close{false};
        UiSlots<Slot>            children;
    };

    /**
     * @class UiPanel
     * @brief
     */
    class UiPanel : public UiSubElement {
    public:
        using UiSubElement::UiSubElement;
        using Slot = UiSlot<UiSubElement>;

        UiSlots<Slot> children;
    };

    /**
     * @class UiStackPanel
     * @brief
     */
    class UiStackPanel : public UiPanel {
    public:
        UiStackPanel() : UiPanel(UiElementType::StackPanel) {}

        UiAttribute<std::string>   name;
        UiAttribute<UiOrientation> orientation{UiOrientation::Vertical};
    };

    /**
     * @class UiScrollPanel
     * @brief
     */
    class UiScrollPanel : public UiPanel {
    public:
        UiScrollPanel() : UiPanel(UiElementType::ScrollPanel) {}

        UiAttribute<std::string> name;
        UiAttributeOpt<float>    scroll_value_x;
        UiAttributeOpt<float>    scroll_value_y;
        UiAttributeOpt<float>    area_portion_x;
        UiAttributeOpt<float>    area_portion_y;
        UiAttribute<bool>        has_border{false};
        UiAttribute<bool>        allow_resize{false};
        UiAttribute<UiScroll>    scroll_type{UiScroll::Vertical};
    };

}// namespace wmoge