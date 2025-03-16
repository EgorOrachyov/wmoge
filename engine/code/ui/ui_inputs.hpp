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
#include "ui/ui_containers.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiDragInt
     * @brief
     */
    class UiDragInt : public UiSubElement {
    public:
        UiDragInt() : UiSubElement(UiElementType::DragInt) {}

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
     * @class UiDragFloat
     * @brief
     */
    class UiDragFloat : public UiSubElement {
    public:
        UiDragFloat() : UiSubElement(UiElementType::DragFloat) {}

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
     * @class UiSliderInt
     * @brief 
     */
    class UiSliderInt : public UiSubElement {
    public:
        UiSliderInt() : UiSubElement(UiElementType::SliderInt) {}

        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<int>            value{0};
        UiAttribute<int>            value_min{0};
        UiAttribute<int>            value_max{0};
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class UiSliderFloat
     * @brief
     */
    class UiSliderFloat : public UiSubElement {
    public:
        UiSliderFloat() : UiSubElement(UiElementType::SliderFloat) {}

        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<float>          value{0};
        UiAttribute<float>          value_min{0};
        UiAttribute<float>          value_max{0};
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class UiInputInt
     * @brief
     */
    class UiInputInt : public UiSubElement {
    public:
        UiInputInt() : UiSubElement(UiElementType::InputInt) {}

        using OnInput = std::function<void()>;

        UiAttribute<std::string> label;
        UiAttribute<int>         value{0};
        UiAttributeOpt<int>      step;
        UiAttributeOpt<int>      step_fast;
        UiEvent<OnInput>         on_input;
    };

    /**
     * @class UiInputFloat
     * @brief
     */
    class UiInputFloat : public UiSubElement {
    public:
        UiInputFloat() : UiSubElement(UiElementType::InputFloat) {}

        using OnInput = std::function<void()>;

        UiAttribute<std::string>    label;
        UiAttribute<float>          value{0.0f};
        UiAttributeOpt<float>       step;
        UiAttributeOpt<float>       step_fast;
        UiAttributeOpt<std::string> format;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class UiInputText
     * @brief
     */
    class UiInputText : public UiSubElement {
    public:
        UiInputText() : UiSubElement(UiElementType::InputText) {}

        using OnInput = std::function<void()>;

        UiAttribute<std::string>    text;
        UiAttributeOpt<std::string> hint;
        UiEvent<OnInput>            on_input;
    };

    /**
     * @class UiInputTextExt
     * @brief
     */
    class UiInputTextExt : public UiSubElement {
    public:
        UiInputTextExt() : UiSubElement(UiElementType::InputTextExt) {}

        using OnInput      = std::function<void()>;
        using OnEnter      = std::function<void()>;
        using OnHistory    = std::function<void()>;
        using OnCompletion = std::function<void()>;

        UiAttribute<UiTextInputFlags> flags;
        UiAttribute<std::string>      text;
        UiAttributeOpt<std::string>   hint;
        UiAttribute<bool>             esc_to_clear{true};
        UiEvent<OnInput>              on_input;
        UiEvent<OnEnter>              on_enter;
        UiEvent<OnHistory>            on_history_prev;
        UiEvent<OnHistory>            on_history_next;
        UiEvent<OnCompletion>         on_completion;
        UiSlot<UiCompletionPopup>     completion_popup;
        UiAttribute<int>              completion_popup_lines{10};
        UiAttribute<bool>             completion_popup_bottom{false};
    };

}// namespace wmoge