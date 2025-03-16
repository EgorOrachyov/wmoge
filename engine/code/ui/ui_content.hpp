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
#include "ui/ui_attribute.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiSeparator
     * @brief
     */
    class UiSeparator : public UiSubElement {
    public:
        UiSeparator() : UiSubElement(UiElementType::Separator) {}
    };

    /**
     * @class UiSeparatorText
     * @brief
     */
    class UiSeparatorText : public UiSubElement {
    public:
        UiSeparatorText() : UiSubElement(UiElementType::SeparatorText) {}

        UiAttribute<std::string> label;
    };

    /**
     * @class UiText
     * @brief
     */
    class UiText : public UiSubElement {
    public:
        UiText() : UiSubElement(UiElementType::Text) {}

        UiAttribute<std::string> text;
        UiAttributeOpt<Color4f>  text_color;
    };

    /**
     * @class UiTextWrapped
     * @brief
     */
    class UiTextWrapped : public UiSubElement {
    public:
        UiTextWrapped() : UiSubElement(UiElementType::TextWrapped) {}

        UiAttribute<std::string> text;
        UiAttributeOpt<Color4f>  text_color;
    };

    /**
     * @class UiTextLink
     * @brief
     */
    class UiTextLink : public UiSubElement {
    public:
        UiTextLink() : UiSubElement(UiElementType::TextLink) {}

        using OnClick = std::function<void()>;

        UiAttribute<std::string>    text;
        UiAttributeOpt<std::string> url;
        UiEvent<OnClick>            on_click;
    };

    /**
     * @class UiProgressBar
     * @brief
     */
    class UiProgressBar : public UiSubElement {
    public:
        UiProgressBar() : UiSubElement(UiElementType::ProgressBar) {}

        UiAttribute<std::string> label;
        UiAttributeOpt<float>    progress;
    };

}// namespace wmoge