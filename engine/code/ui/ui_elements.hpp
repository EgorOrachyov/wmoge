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

#include "core/array_view.hpp"
#include "grc/icon.hpp"
#include "math/color.hpp"
#include "ui/ui_defs.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_style.hpp"
#include "ui/ui_tooltip.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class
     * @brief
     */
    class UiSeparator : public UiSubElement {};

    /**
     * @class
     * @brief
     */
    class UiSeparatorText : public UiSubElement {
    public:
        UiAttribute<std::string> label;
    };

    /**
     * @class
     * @brief
     */
    class UiContextMenu : public UiElement {
    public:
        using Slot = UiSlot<UiSubElement>;

        UiSlots<Slot> children;
    };

    /**
     * @class
     * @brief
     */
    class UiPopup : public UiElement {
    public:
        using Slot = UiSlot<UiSubElement>;

        UiSlots<Slot> children;
    };

    /**
     * @class
     * @brief
     */
    class UiCompletionPopup : public UiPopup {};

    /**
     * @class
     * @brief
     */
    class UiPanel : public UiSubElement {
    public:
        using Slot = UiSlot<UiSubElement>;

        UiSlots<Slot> children;
    };

    class UiStackPanel : public UiPanel {
    public:
        UiAttribute<UiOrientation> orientation{UiOrientation::Vertical};
    };

    /**
     * @class
     * @brief
     */
    class UiScrollPanel : public UiPanel {
    public:
        UiAttributeOpt<float> scroll_value_x;
        UiAttributeOpt<float> scroll_value_y;
        UiAttribute<UiScroll> scroll_type{UiScroll::Vertical};
    };

    /**
     * @class
     * @brief
     */
    class UiMenuItem : public UiSubElement {
    public:
        using OnClick = std::function<void()>;

        UiAttribute<bool> selected{false};
        UiAttribute<bool> enabled{true};
        UiEvent<OnClick>  on_click;
    };

    /**
     * @class
     * @brief
     */
    class UiMenu : public UiSubElement {
    public:
        using Slot = UiSlot<UiSubElement>;

        UiSlots<Slot> children;
    };

    /**
     * @class
     * @brief
     */
    class UiMenuBar : public UiElement {
    public:
        using Slot = UiSubElement;

        UiSlots<Slot> children;
    };

    /**
     * @class
     * @brief
     */
    class UiToolBar : public UiElement {
    public:
        using Slot = UiSubElement;

        UiSlots<Slot> children;
    };

    /**
     * @class
     * @brief
     */
    class UiWindow : public UiElement {
    public:
        UiSlot<UiMenuBar>          menu_bar;
        UiSlot<UiToolBar>          tool_bar;
        UiSlot<UiPanel>            panel;
        UiAttribute<UiWindowFlags> flags;
        UiAttribute<bool>          is_open{true};
    };

    /**
     * @class
     * @brief
     */
    class UiMainWindow : public UiWindow {};

    /**
     * @class
     * @brief
     */
    class UiDockWindow : public UiWindow {};

    /**
     * @class
     * @brief
     */
    class UiText : public UiSubElement {
    public:
        UiAttribute<std::string> text;
        UiAttributeOpt<Color4f>  text_color;
    };

    /**
     * @class
     * @brief
     */
    class UiTextWrapped : public UiSubElement {
    public:
        UiAttribute<std::string> text;
        UiAttributeOpt<Color4f>  text_color;
    };

    /**
     * @class
     * @brief
     */
    class UiTextLink : public UiSubElement {
    public:
        UiAttribute<std::string> text;
    };

    /**
     * @class
     * @brief
     */
    class UiDragInt : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<int>            value{0};
        UiAttributeOpt<int>         value_min;
        UiAttributeOpt<int>         value_max;
        UiAttributeOpt<float>       speed;
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiDragFloat : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<float>          value{0};
        UiAttributeOpt<float>       value_min;
        UiAttributeOpt<float>       value_max;
        UiAttributeOpt<float>       speed;
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiSliderInt : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<int>            value{0};
        UiAttribute<int>            value_min{0};
        UiAttribute<int>            value_max{0};
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiSliderFloat : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<float>          value{0};
        UiAttribute<float>          value_min{0};
        UiAttribute<float>          value_max{0};
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiInputInt : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<int>         value{0};
        UiAttributeOpt<int>      step;
        UiAttributeOpt<int>      step_fast;
        UiEvent<OnInput>         on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiInputFloat : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<float>          value{0.0f};
        UiAttributeOpt<float>       step;
        UiAttributeOpt<float>       step_fast;
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiInputText : public UiSubElement {
    public:
        using OnInput = std::function<void()>;

        UiAttribute<std::string>    text;
        UiAttributeOpt<std::string> hint;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class
     * @brief
     */
    class UiInputTextExt : public UiSubElement {
    public:
        using OnInput      = std::function<void()>;
        using OnHistory    = std::function<void(UiDir)>;
        using OnCompletion = std::function<void()>;

        UiAttribute<UiTextInputFlags> flags;
        UiAttribute<std::string>      text;
        UiAttributeOpt<std::string>   hint;
        UiEvent<OnInput>              on_input;
        UiEvent<OnHistory>            on_history;
        UiEvent<OnCompletion>         on_completion;
        UiSlot<UiCompletionPopup>     completion_popup;
    };

    /**
     * @class
     * @brief
     */
    class UiSelectable : public UiSubElement {
    public:
        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttributeOpt<Icon>     icon;
        UiAttribute<bool>        selected{false};
        UiAttribute<bool>        disabled{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class
     * @brief
     */
    class UiButton : public UiSubElement {
    public:
        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<Icon>        icon;
        UiAttributeOpt<Vec2f>    icon_scale;
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class
     * @brief
     */
    class UiCheckBoxButton : public UiSubElement {
    public:
        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<bool>        checked{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class
     * @brief
     */
    class UiRadioButton : public UiSubElement {
    public:
        using OnClick = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<bool>        checked{false};
        UiEvent<OnClick>         on_click;
    };

    /**
     * @class
     * @brief
     */
    class UiComboBox : public UiSubElement {
    public:
        UiAttribute<std::string>     label;
        UiAttributeList<std::string> items;
        UiAttributeOpt<int>          current_item;
        UiAttributeOpt<int>          max_popup_items;
    };

    /**
     * @class
     * @brief
     */
    class UiListBox : public UiSubElement {
    public:
        UiAttribute<std::string>     label;
        UiAttributeList<std::string> items;
        UiAttributeOpt<int>          current_item;
        UiAttributeOpt<int>          height_in_items;
    };

    /**
     * @class
     * @brief
     */
    class UiProgressBar : public UiSubElement {
    public:
        UiAttribute<std::string> label;
        UiAttributeOpt<float>    progress;
    };

}// namespace wmoge