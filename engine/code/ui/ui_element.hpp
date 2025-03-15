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
#include "core/simple_id.hpp"
#include "core/status.hpp"
#include "ui/ui_attribute.hpp"
#include "ui/ui_defs.hpp"

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
        StackPanel,
        ScrollPanel,
        MenuItem,
        Menu,
        MenuBar,
        ToolBar,
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

    /** @brief Ui element id */
    using UiElementId = SimpleId<>;

    /**
     * @class UiUserData
     * @brief Base class for user data which can be attached to any ui element
     */
    class UiUserData : public RefCnt {};

    /**
     * @class UiElement
     * @brief Base class for all ui elements
     */
    class UiElement : public RefCnt {
    public:
        UiAttribute<Strid>           tag;
        UiAttributeOpt<UiCursorType> cursor;
        UiAttribute<Ref<UiUserData>> user_data;
        UiElementId                  id;
        UiElementType                type = UiElementType::Unknown;
    };

    /**
     * @class UiSubElement
     * @brief Base class for all ui sub-elements which could be nested
     */
    class UiSubElement : public UiElement {
    public:
        ~UiSubElement() override = default;
    };

}// namespace wmoge