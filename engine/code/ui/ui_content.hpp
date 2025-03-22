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

#include "math/color.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiSeparator
     * @brief
     */
    class UiSeparator : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiSeparator, UiSubElement);

        UiSeparator() : UiSubElement(UiElementType::Separator) {}
    };

    WG_RTTI_CLASS_BEGIN(UiSeparator) {
        WG_RTTI_FACTORY();
    }
    WG_RTTI_END;

    /**
     * @class UiSeparatorText
     * @brief
     */
    class UiSeparatorText : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiSeparatorText, UiSubElement);

        UiSeparatorText() : UiSubElement(UiElementType::SeparatorText) {}

        std::string label;
    };

    WG_RTTI_CLASS_BEGIN(UiSeparatorText) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
    }
    WG_RTTI_END;

    /**
     * @class UiText
     * @brief
     */
    class UiText : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiText, UiSubElement);

        UiText() : UiSubElement(UiElementType::Text) {}

        std::string text;
    };

    WG_RTTI_CLASS_BEGIN(UiText) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(text, {});
    }
    WG_RTTI_END;

    /**
     * @class UiTextWrapped
     * @brief
     */
    class UiTextWrapped : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiTextWrapped, UiSubElement);

        UiTextWrapped() : UiSubElement(UiElementType::TextWrapped) {}

        std::string text;
    };

    WG_RTTI_CLASS_BEGIN(UiTextWrapped) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(text, {});
    }
    WG_RTTI_END;

    /**
     * @class UiTextLink
     * @brief
     */
    class UiTextLink : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiTextLink, UiSubElement);

        UiTextLink() : UiSubElement(UiElementType::TextLink) {}

        using OnClick = std::function<void()>;

        std::string                text;
        std::optional<std::string> url;
        OnClick                    on_click;
    };

    WG_RTTI_CLASS_BEGIN(UiTextLink) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(text, {});
        WG_RTTI_FIELD(url, {});
        WG_RTTI_FIELD(on_click, {RttiNoSaveLoad});
    }
    WG_RTTI_END;

    /**
     * @class UiProgressBar
     * @brief
     */
    class UiProgressBar : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiProgressBar, UiSubElement);

        UiProgressBar() : UiSubElement(UiElementType::ProgressBar) {}

        std::string          label;
        std::optional<float> progress;
    };

    WG_RTTI_CLASS_BEGIN(UiProgressBar) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(label, {});
        WG_RTTI_FIELD(progress, {});
    }
    WG_RTTI_END;

}// namespace wmoge