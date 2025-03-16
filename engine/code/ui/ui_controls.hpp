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
#include "ui/ui_attribute.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiMenuItem
     * @brief
     */
    class UiMenuItem : public UiSubElement {
    public:
        UiMenuItem() : UiSubElement(UiElementType::MenuItem) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<std::string> shortcut;
        UiAttribute<Icon>        icon;
        UiAttribute<Vec2f>       icon_size{Vec2f(1, 1)};
        UiAttribute<bool>        selected{false};
        UiAttribute<bool>        disabled{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class UiSelectable
     * @brief
     */
    class UiSelectable : public UiSubElement {
    public:
        UiSelectable() : UiSubElement(UiElementType::Selectable) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<Icon>        icon;
        UiAttribute<Vec2f>       icon_size{Vec2f(1, 1)};
        UiAttribute<bool>        selected{false};
        UiAttribute<bool>        disabled{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class UiButton
     * @brief
     */
    class UiButton : public UiSubElement {
    public:
        UiButton() : UiSubElement(UiElementType::Button) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<Icon>        icon;
        UiAttribute<Vec2f>       icon_scale{Vec2f(1, 1)};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class UiCheckBoxButton
     * @brief 
     */
    class UiCheckBoxButton : public UiSubElement {
    public:
        UiCheckBoxButton() : UiSubElement(UiElementType::CheckBoxButton) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<bool>        checked{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class UiRadioButton
     * @brief
     */
    class UiRadioButton : public UiSubElement {
    public:
        UiRadioButton() : UiSubElement(UiElementType::RadioButton) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<bool>        checked{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class UiComboBox
     * @brief
     */
    class UiComboBox : public UiSubElement {
    public:
        UiComboBox() : UiSubElement(UiElementType::ComboBox) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string>     label;
        UiAttributeList<std::string> items;
        UiAttributeOpt<int>          current_item;
        UiAttributeOpt<int>          max_popup_items;
        UiEvent<OnClick>             on_click;
    };

    /**
     * @class UiListBox
     * @brief
     */
    class UiListBox : public UiSubElement {
    public:
        UiListBox() : UiSubElement(UiElementType::ListBox) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string>     label;
        UiAttributeList<std::string> items;
        UiAttributeOpt<int>          current_item;
        UiAttributeOpt<int>          height_in_items;
        UiEvent<OnClick>             on_click;
    };

}// namespace wmoge
