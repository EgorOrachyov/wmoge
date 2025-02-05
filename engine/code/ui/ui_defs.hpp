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

    enum class UiWindowFlag {
        NoBringToFrontOnFocus,
        NoPadding,
        NoBackground,
        NoTitleBar,
        NoCollapse,
        NoScrollbar
    };

    using UiWindowFlags = Mask<UiWindowFlag>;

    enum class UiTextInputFlag {
        CallbackAlways,
        EnterForSubmit,
        EscapeToClear,
        CompletionTab,
        HistoryScroll
    };

    using UiTextInputFlags = Mask<UiTextInputFlag>;

    enum class UiTextInputEventType {
        None,
        Always,
        Completion,
        History,
        Enter
    };

    enum class UiTextInputScrollDir {
        None,
        Up,
        Down
    };

    struct UiTextInputEvent {
        UiTextInputEventType type = UiTextInputEventType::None;
        UiTextInputScrollDir dir  = UiTextInputScrollDir::None;
        std::string          text;
    };

    /** @brief */
    using UiOnTextInput = std::function<void(const UiTextInputEvent&)>;

    /** @brief */
    using UiOnClick = std::function<void()>;

    enum class UiScrollAreaType {
        Horizontal,
        Vertical,
        Mixed
    };

    struct UiHintWidth {
        std::optional<float> scale;
    };

}// namespace wmoge