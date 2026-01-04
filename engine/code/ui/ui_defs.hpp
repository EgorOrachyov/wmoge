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

#include "core/mask.hpp"

#include <functional>
#include <string>

namespace wmoge {

    /** @brief Type of ui binding update */
    enum class UiBindingType {
        ToTarget,
        ToTargetOnce,
        ToSource,
    };

    /** @brief Cursor shown over ui element */
    enum class UiCursorType {
        Arrow = 0,
        TextInput,
        ResizeAll,
        ResizeNS,
        ResizeEW,
        ResizeNESW,
        ResizeNWSE,
        Hand,
        NotAllowed
    };

    /** @brief General ui direction value */
    enum class UiDir {
        None  = 0,
        Left  = 1,
        Right = 2,
        Up    = 3,
        Down  = 4
    };

    /** @brief Layout orientation */
    enum class UiOrientation {
        Horizontal,
        Vertical
    };

    /** @brief Type of scroll areas */
    enum class UiScroll {
        Vertical,
        Horizontal,
        Mixed
    };

    /** @brief Flag for window element */
    enum class UiWindowFlag {
        NoBringToFrontOnFocus,
        NoPadding,
        NoTitleBar,
        NoCollapse,
        NoScrollbar
    };

    /** @brief Window element flags */
    using UiWindowFlags = Mask<UiWindowFlag>;

    /** @brief Flag for table element */
    enum class UiTableFlag {
        Resizable,
        RowBg,
        BordersInnerH,
        BordersOuterH,
        BordersInnerV,
        BordersOuterV,
        BordersH,
        BordersV,
        BordersInner,
        BordersOuter,
        Borders,
        ScrollX,
        ScrollY,
    };

    /** @brief Table element flags */
    using UiTableFlags = Mask<UiTableFlag>;

    /** @brief Table column element flag */
    enum class UiTableColumnFlag {
        Disabled,
        DefaultHide,
        DefaultSort,
        WidthStretch,
        WidthFixed,
        NoResize,
        NoReorder,
        NoHide,
        NoClip,
        NoSort,
    };

    /** @brief Table column element flags */
    using UiTableColumnFlags = Mask<UiTableColumnFlag>;

    /** @brief Flag for table row element */
    enum class UiTableTreeRowFlag {
        Leaf,
        Bullet,
        LabelSpanAllColumns,
    };

    /** @brief Table row element flags */
    using UiTableTreeRowFlags = Mask<UiTableTreeRowFlag>;

    /** @brief Name of param in ui style */
    enum class UiParam {
        Alpha = 0,
        DisabledAlpha,
        WindowPaddingX,
        WindowPaddingY,
        WindowRounding,
        WindowBorderSize,
        WindowMinSizeX,
        WindowMinSizeY,
        WindowTitleAlignX,
        WindowTitleAlignY,
        ChildRounding,
        ChildBorderSize,
        PopupRounding,
        PopupBorderSize,
        FramePaddingX,
        FramePaddingY,
        FrameRounding,
        FrameBorderSize,
        ItemSpacingX,
        ItemSpacingY,
        ItemInnerSpacingX,
        ItemInnerSpacingY,
        IndentSpacing,
        CellPaddingX,
        CellPaddingY,
        ScrollbarSize,
        ScrollbarRounding,
        GrabMinSize,
        GrabRounding,
        TabRounding,
        TabBorderSize,
        TabBarBorderSize,
        TabBarOverlineSize,
        TableAngledHeadersAngle,
        TableAngledHeadersTextAlignX,
        TableAngledHeadersTextAlignY,
        ButtonTextAlignX,
        ButtonTextAlignY,
        SelectableTextAlignX,
        SelectableTextAlignY,
        SeparatorTextBorderSize,
        SeparatorTextAlignX,
        SeparatorTextAlignY,
        SeparatorTextPaddingX,
        SeparatorTextPaddingY,
        DockingSeparatorSize,
        Count
    };

    /** @brief Name of color param in ui stype */
    enum class UiColor {
        Text = 0,
        TextDisabled,
        WindowBg,
        ChildBg,
        PopupBg,
        Border,
        BorderShadow,
        FrameBg,
        FrameBgHovered,
        FrameBgActive,
        TitleBg,
        TitleBgActive,
        TitleBgCollapsed,
        MenuBarBg,
        MenuBarBorder,
        ScrollbarBg,
        ScrollbarGrab,
        ScrollbarGrabHovered,
        ScrollbarGrabActive,
        CheckMark,
        SliderGrab,
        SliderGrabActive,
        Button,
        ButtonHovered,
        ButtonActive,
        Header,
        HeaderHovered,
        HeaderActive,
        CollapsingHeader,
        CollapsingHeaderHovered,
        CollapsingHeaderActive,
        Separator,
        SeparatorHovered,
        SeparatorActive,
        ResizeGrip,
        ResizeGripHovered,
        ResizeGripActive,
        TabHovered,
        Tab,
        TabSelected,
        TabSelectedOverline,
        TabDimmed,
        TabDimmedSelected,
        TabDimmedSelectedOverline,
        DockingPreview,
        DockingEmptyBg,
        PlotLines,
        PlotLinesHovered,
        PlotHistogram,
        PlotHistogramHovered,
        TableHeaderBg,
        TableBorderStrong,
        TableBorderLight,
        TableRowBg,
        TableRowBgAlt,
        TextLink,
        TextSelectedBg,
        DragDropTarget,
        NavCursor,
        NavWindowingHighlight,
        NavWindowingDimBg,
        ModalWindowDimBg,
        Count
    };

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
        DockSpace,
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
        ProgressBar,
        TableColumn,
        TableTreeRow,
        TableTree,
    };

}// namespace wmoge