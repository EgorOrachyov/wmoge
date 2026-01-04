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

#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiTableColumn
     * @brief Ui table column config
     */
    class UiTableColumn : public UiElement {
    public:
        WG_RTTI_CLASS(UiTableColumn, UiElement);

        UiTableColumn() : UiElement(UiElementType::TableColumn) {}

        std::string        title;
        UiTableColumnFlags flags;
    };

    WG_RTTI_CLASS_BEGIN(UiTableColumn) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(title, {});
        WG_RTTI_FIELD(flags, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class UiTableTreeRow
     * @brief Ui table tree row element
     */
    class UiTableTreeRow : public UiElement {
    public:
        WG_RTTI_CLASS(UiTableTreeRow, UiElement);

        UiTableTreeRow() : UiElement(UiElementType::TableTreeRow) {}

        std::vector<Ref<UiElement>> sub_rows;
        UiTableTreeRowFlags         flags;
    };

    WG_RTTI_CLASS_BEGIN(UiTableTreeRow) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(sub_rows, {RttiOptional});
        WG_RTTI_FIELD(flags, {RttiOptional});
    }
    WG_RTTI_END;

    /**
     * @class UiTableTree
     * @brief Ui table tree view
     */
    class UiTableTree : public UiElement {
    public:
        WG_RTTI_CLASS(UiTableTree, UiElement);

        UiTableTree() : UiElement(UiElementType::TableTree) {}

        using OnRowMoved    = std::function<void(int)>;
        using OnRowSelected = std::function<void(int)>;

        std::vector<Ref<UiTableTreeRow>> rows;
        std::vector<Ref<UiTableColumn>>  columns;
        std::string                      title;
        UiTableFlags                     flags;
        OnRowMoved                       on_row_moved;
        OnRowMoved                       on_row_selected;
    };

    WG_RTTI_CLASS_BEGIN(UiTableTree) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(rows, {RttiOptional});
        WG_RTTI_FIELD(columns, {RttiOptional});
        WG_RTTI_FIELD(title, {RttiOptional});
        WG_RTTI_FIELD(flags, {RttiOptional});
        WG_RTTI_FIELD(on_row_moved, {RttiNoSaveLoad});
        WG_RTTI_FIELD(on_row_selected, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

}// namespace wmoge