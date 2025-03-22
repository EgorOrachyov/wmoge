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

#include "_rtti.hpp"

#include "ui/ui_bars.hpp"
#include "ui/ui_bindable.hpp"
#include "ui/ui_containers.hpp"
#include "ui/ui_content.hpp"
#include "ui/ui_controls.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_inputs.hpp"
#include "ui/ui_markup.hpp"
#include "ui/ui_style.hpp"
#include "ui/ui_tooltip.hpp"
#include "ui/ui_views.hpp"
#include "ui/ui_window.hpp"

namespace wmoge {

    void rtti_ui() {
        rtti_type<UiColorPalette>();
        rtti_type<UiColorSlot>();
        rtti_type<UiFontSlot>();
        rtti_type<UiStyleDesc>();
        rtti_type<UiStyle>();
        rtti_type<UiElement>();
        rtti_type<UiSubElement>();
        rtti_type<UiWindow>();
        rtti_type<UiMainWindow>();
        rtti_type<UiDockWindow>();
        rtti_type<UiDockSpace>();
        rtti_type<UiToolTip>();
        rtti_type<UiMenuBar>();
        rtti_type<UiToolBar>();
        rtti_type<UiStatusBar>();
        rtti_type<UiDragInt>();
        rtti_type<UiDragFloat>();
        rtti_type<UiSliderInt>();
        rtti_type<UiSliderFloat>();
        rtti_type<UiInputInt>();
        rtti_type<UiInputFloat>();
        rtti_type<UiInputText>();
        rtti_type<UiInputTextExt>();
        rtti_type<UiSeparator>();
        rtti_type<UiSeparatorText>();
        rtti_type<UiText>();
        rtti_type<UiTextWrapped>();
        rtti_type<UiTextLink>();
        rtti_type<UiProgressBar>();
        rtti_type<UiContextMenu>();
        rtti_type<UiMenu>();
        rtti_type<UiPopup>();
        rtti_type<UiCompletionPopup>();
        rtti_type<UiModal>();
        rtti_type<UiPanel>();
        rtti_type<UiStackPanel>();
        rtti_type<UiScrollPanel>();
        rtti_type<UiCollapsingPanel>();
        rtti_type<UiMenuItem>();
        rtti_type<UiSelectable>();
        rtti_type<UiButton>();
        rtti_type<UiCheckBoxButton>();
        rtti_type<UiRadioButton>();
        rtti_type<UiComboBox>();
        rtti_type<UiListBox>();
        rtti_type<UiMarkup>();
        rtti_type<UiBindable>();
    }

}// namespace wmoge