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

#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "ui/ui_attribute.hpp"
#include "ui/ui_defs.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /** @brief Ui element enum type */
    enum class UiElementType {
        Unknown = 0,
        Separator,
        SeparatorText,
        ToolTip,
        ContextMenu,
        Popup,
        CompletionPopup,
        Modal,
        StackPanel,
        ScrollPanel,
        CollapsingPanel,
        MenuItem,
        Menu,
        MenuBar,
        ToolBar,
        StatusBar,
        MainWindow,
        DockWindow,
        Text,
        TextWrapped,
        TextLink,
        DragInt,
        DragFloat,
        SliderInt,
        SliderFloat,
        InputInt,
        InputFloat,
        InputText,
        InputTextExt,
        Selectable,
        Button,
        CheckBoxButton,
        RadioButton,
        ComboBox,
        ListBox,
        ProgressBar
    };

    /**
     * @class UiElement
     * @brief Base class for all ui elements
     */
    class UiElement : public RefCnt {
    public:
        UiElement(UiElementType type) : type(type) {}

        UiAttribute<Strid>           tag;
        UiAttributeOpt<UiCursorType> cursor;
        UiAttribute<Strid>           sub_style;
        const UiElementType          type = UiElementType::Unknown;
    };

    /**
     * @class UiSubElement
     * @brief Base class for all ui sub-elements which could be nested
     */
    class UiSubElement : public UiElement {
    public:
        UiSubElement(UiElementType type) : UiElement(type) {}
    };

}// namespace wmoge