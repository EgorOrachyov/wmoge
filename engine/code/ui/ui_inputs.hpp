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

#include "ui/ui_containers.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiDragInt
     * @brief Ui int field with drag option
     */
    class UiDragInt : public UiElement {
    public:
        WG_RTTI_CLASS(UiDragInt, UiElement);

        UiDragInt() : UiElement(UiElementType::DragInt) {}

        using OnInput = std::function<void()>;

        std::string                label;
        int                        value{0};
        std::optional<int>         value_min;
        std::optional<int>         value_max;
        std::optional<float>       speed;
        std::optional<std::string> format;
        OnInput                    on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiDragInt) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(value, {});
        WG_RTTI_FIELD(value_min, {});
        WG_RTTI_FIELD(value_max, {});
        WG_RTTI_FIELD(speed, {});
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiDragFloat
     * @brief Ui float field with drag option
     */
    class UiDragFloat : public UiElement {
    public:
        WG_RTTI_CLASS(UiDragFloat, UiElement);

        UiDragFloat() : UiElement(UiElementType::DragFloat) {}

        using OnInput = std::function<void()>;

        std::string                label;
        float                      value{0};
        std::optional<float>       value_min;
        std::optional<float>       value_max;
        std::optional<float>       speed;
        std::optional<std::string> format;
        OnInput                    on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiDragFloat) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(value, {});
        WG_RTTI_FIELD(value_min, {});
        WG_RTTI_FIELD(value_max, {});
        WG_RTTI_FIELD(speed, {});
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiSliderInt
     * @brief Ui int slider with bar for selection
     */
    class UiSliderInt : public UiElement {
    public:
        WG_RTTI_CLASS(UiSliderInt, UiElement);

        UiSliderInt() : UiElement(UiElementType::SliderInt) {}

        using OnInput = std::function<void()>;

        std::string                label;
        int                        value{0};
        int                        value_min{0};
        int                        value_max{0};
        std::optional<std::string> format;
        OnInput                    on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiSliderInt) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(value, {});
        WG_RTTI_FIELD(value_min, {});
        WG_RTTI_FIELD(value_max, {});
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiSliderFloat
     * @brief Ui float slider with bar for selection
     */
    class UiSliderFloat : public UiElement {
    public:
        WG_RTTI_CLASS(UiSliderFloat, UiElement);

        UiSliderFloat() : UiElement(UiElementType::SliderFloat) {}

        using OnInput = std::function<void()>;

        std::string                label;
        float                      value{0};
        float                      value_min{0};
        float                      value_max{0};
        std::optional<std::string> format;
        OnInput                    on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiSliderFloat) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(value, {});
        WG_RTTI_FIELD(value_min, {});
        WG_RTTI_FIELD(value_max, {});
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiInputInt
     * @brief Ui int input field with step editing
     */
    class UiInputInt : public UiElement {
    public:
        WG_RTTI_CLASS(UiInputInt, UiElement);

        UiInputInt() : UiElement(UiElementType::InputInt) {}

        using OnInput = std::function<void()>;

        std::string        label;
        int                value{0};
        std::optional<int> step;
        std::optional<int> step_fast;
        OnInput            on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiInputInt) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(value, {});
        WG_RTTI_FIELD(step, {});
        WG_RTTI_FIELD(step_fast, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiInputFloat
     * @brief Ui float input field with step editing
     */
    class UiInputFloat : public UiElement {
    public:
        WG_RTTI_CLASS(UiInputFloat, UiElement);

        UiInputFloat() : UiElement(UiElementType::InputFloat) {}

        using OnInput = std::function<void()>;

        std::string                label;
        float                      value{0.0f};
        std::optional<float>       step;
        std::optional<float>       step_fast;
        std::optional<std::string> format;
        OnInput                    on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiInputFloat) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(value, {});
        WG_RTTI_FIELD(step, {});
        WG_RTTI_FIELD(step_fast, {});
        WG_RTTI_FIELD(format, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiInputText
     * @brief Ui text input field
     */
    class UiInputText : public UiElement {
    public:
        WG_RTTI_CLASS(UiInputText, UiElement);

        UiInputText() : UiElement(UiElementType::InputText) {}

        using OnInput = std::function<void()>;

        std::string                text;
        std::optional<std::string> hint;
        OnInput                    on_input;
    };

    WG_RTTI_CLASS_BEGIN(UiInputText) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(text, {});
        WG_RTTI_FIELD(hint, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiInputTextExt
     * @brief Ui extended text input field with rich callbacks and completion
     */
    class UiInputTextExt : public UiElement {
    public:
        WG_RTTI_CLASS(UiInputTextExt, UiElement);

        UiInputTextExt() : UiElement(UiElementType::InputTextExt) {}

        using OnInput      = std::function<void()>;
        using OnEnter      = std::function<void()>;
        using OnHistory    = std::function<void()>;
        using OnCompletion = std::function<void()>;

        std::string                text;
        std::optional<std::string> hint;
        bool                       esc_to_clear{true};
        OnInput                    on_input;
        OnEnter                    on_enter;
        OnHistory                  on_history_prev;
        OnHistory                  on_history_next;
        OnCompletion               on_completion;
        Ref<UiCompletionPopup>     completion_popup;
        int                        completion_popup_lines{10};
        bool                       completion_popup_bottom{false};
    };

    WG_RTTI_CLASS_BEGIN(UiInputTextExt) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(text, {});
        WG_RTTI_FIELD(hint, {});
        WG_RTTI_FIELD(esc_to_clear, {});
        WG_RTTI_FIELD(on_input, {RttiNoSaveLoad});
        WG_RTTI_FIELD(on_enter, {RttiNoSaveLoad});
        WG_RTTI_FIELD(on_history_prev, {RttiNoSaveLoad});
        WG_RTTI_FIELD(on_history_next, {RttiNoSaveLoad});
        WG_RTTI_FIELD(on_completion, {RttiNoSaveLoad});
        WG_RTTI_FIELD(completion_popup, {});
        WG_RTTI_FIELD(completion_popup_lines, {});
        WG_RTTI_FIELD(completion_popup_bottom, {});
    }
    WG_RTTI_END;

}// namespace wmoge