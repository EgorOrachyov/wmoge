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

#include "grc/icon.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiMenuItem
     * @brief Ui menu item which can be placed inside a menu
     */
    class UiMenuItem : public UiElement {
    public:
        WG_RTTI_CLASS(UiMenuItem, UiElement);

        UiMenuItem() : UiElement(UiElementType::MenuItem) {}

        using OnClick = std::function<void()>;

        std::string label;
        std::string shortcut;
        Icon        icon;
        Vec2f       icon_size{Vec2f(1, 1)};
        bool        selected{false};
        bool        disabled{false};
        OnClick     on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiMenuItem) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(shortcut, {});
        WG_RTTI_FIELD(icon_size, {});
        WG_RTTI_FIELD(selected, {});
        WG_RTTI_FIELD(disabled, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiSelectable
     * @brief Ui selectable element for selection in menus and lists
     */
    class UiSelectable : public UiElement {
    public:
        WG_RTTI_CLASS(UiSelectable, UiElement);

        UiSelectable() : UiElement(UiElementType::Selectable) {}

        using OnClick = std::function<void()>;

        std::string label;
        Icon        icon;
        Vec2f       icon_size{Vec2f(1, 1)};
        bool        selected{false};
        bool        disabled{false};
        OnClick     on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiSelectable) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(icon, {});
        WG_RTTI_FIELD(icon_size, {});
        WG_RTTI_FIELD(selected, {});
        WG_RTTI_FIELD(disabled, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiButton
     * @brief Ui button element which can be clicked
     */
    class UiButton : public UiElement {
    public:
        WG_RTTI_CLASS(UiButton, UiElement);

        UiButton() : UiElement(UiElementType::Button) {}

        using OnClick = std::function<void()>;

        std::string label;
        Icon        icon;
        Vec2f       icon_size{Vec2f(1, 1)};
        OnClick     on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiButton) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(icon, {});
        WG_RTTI_FIELD(icon_size, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiCheckBoxButton
     * @brief Ui check box button which can be checked or unchecked
     */
    class UiCheckBoxButton : public UiElement {
    public:
        WG_RTTI_CLASS(UiCheckBoxButton, UiElement);

        UiCheckBoxButton() : UiElement(UiElementType::CheckBoxButton) {}

        using OnClick = std::function<void()>;

        std::string label;
        bool        checked{false};
        OnClick     on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiCheckBoxButton) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(checked, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiRadioButton
     * @brief Ui radio button
     */
    class UiRadioButton : public UiElement {
    public:
        WG_RTTI_CLASS(UiRadioButton, UiElement);

        UiRadioButton() : UiElement(UiElementType::RadioButton) {}

        using OnClick = std::function<void()>;

        std::string label;
        bool        checked{false};
        OnClick     on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiRadioButton) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(checked, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiComboBox
     * @brief Ui combo box with items which can be dropped down
     */
    class UiComboBox : public UiElement {
    public:
        WG_RTTI_CLASS(UiComboBox, UiElement);

        UiComboBox() : UiElement(UiElementType::ComboBox) {}

        using OnClick = std::function<void()>;

        std::string              label;
        std::vector<std::string> items;
        std::optional<int>       current_item;
        std::optional<int>       max_popup_items;
        OnClick                  on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiComboBox) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(items, {});
        WG_RTTI_FIELD(current_item, {});
        WG_RTTI_FIELD(max_popup_items, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiListBox
     * @brief Ui list box with items shown in a multi-line box
     */
    class UiListBox : public UiElement {
    public:
        WG_RTTI_CLASS(UiListBox, UiElement);

        UiListBox() : UiElement(UiElementType::ListBox) {}

        using OnClick = std::function<void()>;

        std::string              label;
        std::vector<std::string> items;
        std::optional<int>       current_item;
        std::optional<int>       height_in_items;
        OnClick                  on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiListBox) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(items, {});
        WG_RTTI_FIELD(current_item, {});
        WG_RTTI_FIELD(height_in_items, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

}// namespace wmoge
