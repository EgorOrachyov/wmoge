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

#include "ui/ui_content.hpp"
#include "ui/ui_element.hpp"

namespace wmoge {

    /**
     * @class UiContextMenu
     * @brief
     */
    class UiContextMenu : public UiElement {
    public:
        WG_RTTI_CLASS(UiContextMenu, UiElement);

        UiContextMenu() : UiElement(UiElementType::ContextMenu) {}

        using Slot = Ref<UiSubElement>;

        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiContextMenu) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

    /**
     * @class UiMenu
     * @brief
     */
    class UiMenu : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiMenu, UiSubElement);

        UiMenu() : UiSubElement(UiElementType::Menu) {}

        using Slot = Ref<UiSubElement>;

        std::string       name;
        bool              enabled{true};
        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiMenu) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(enabled, {});
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

    /**
     * @class UiPopup
     * @brief
     */
    class UiPopup : public UiElement {
    public:
        WG_RTTI_CLASS(UiPopup, UiElement);

        UiPopup() : UiElement(UiElementType::Popup) {}

        using Slot = Ref<UiSubElement>;

        std::string       name;
        bool              should_show{false};
        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiPopup) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(should_show, {});
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

    /**
     * @class UiCompletionPopup
     * @brief
     */
    class UiCompletionPopup : public UiElement {
    public:
        WG_RTTI_CLASS(UiCompletionPopup, UiElement);

        UiCompletionPopup() : UiElement(UiElementType::CompletionPopup) {}

        using Slot = Ref<UiSubElement>;

        std::string       name;
        bool              should_show{false};
        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiCompletionPopup) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(should_show, {});
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

    /**
     * @class UiModal
     * @brief
     */
    class UiModal : public UiElement {
    public:
        WG_RTTI_CLASS(UiModal, UiElement);

        UiModal() : UiElement(UiElementType::Modal) {}

        using Slot = Ref<UiSubElement>;

        std::string       name;
        bool              should_show{false};
        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiModal) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(should_show, {});
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

    /**
     * @class UiPanel
     * @brief
     */
    class UiPanel : public UiSubElement {
    public:
        WG_RTTI_CLASS(UiPanel, UiSubElement);

        using UiSubElement::UiSubElement;
        using Slot = Ref<UiSubElement>;

        std::vector<Slot> children;
    };

    WG_RTTI_CLASS_BEGIN(UiPanel) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(children, {});
    }
    WG_RTTI_END;

    /**
     * @class UiStackPanel
     * @brief
     */
    class UiStackPanel : public UiPanel {
    public:
        WG_RTTI_CLASS(UiStackPanel, UiPanel);

        UiStackPanel() : UiPanel(UiElementType::StackPanel) {}

        std::string   name;
        UiOrientation orientation{UiOrientation::Vertical};
    };

    WG_RTTI_CLASS_BEGIN(UiStackPanel) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(orientation, {});
    }
    WG_RTTI_END;

    /**
     * @class UiScrollPanel
     * @brief
     */
    class UiScrollPanel : public UiPanel {
    public:
        WG_RTTI_CLASS(UiScrollPanel, UiPanel);

        UiScrollPanel() : UiPanel(UiElementType::ScrollPanel) {}

        std::string          name;
        std::optional<float> scroll_value_x;
        std::optional<float> scroll_value_y;
        std::optional<float> area_portion_x;
        std::optional<float> area_portion_y;
        bool                 has_border{false};
        bool                 allow_resize{false};
        UiScroll             scroll_type{UiScroll::Vertical};
    };

    WG_RTTI_CLASS_BEGIN(UiScrollPanel) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(name, {});
        WG_RTTI_FIELD(scroll_value_x, {});
        WG_RTTI_FIELD(scroll_value_y, {});
        WG_RTTI_FIELD(area_portion_x, {});
        WG_RTTI_FIELD(area_portion_y, {});
        WG_RTTI_FIELD(has_border, {});
        WG_RTTI_FIELD(allow_resize, {});
        WG_RTTI_FIELD(scroll_type, {});
    }
    WG_RTTI_END;

    /**
     * @class UiCollapsingPanel
     * @brief
     */
    class UiCollapsingPanel : public UiPanel {
    public:
        WG_RTTI_CLASS(UiCollapsingPanel, UiPanel);

        UiCollapsingPanel() : UiPanel(UiElementType::CollapsingPanel) {}

        Ref<UiText> header;
        bool        default_open{true};
    };

    WG_RTTI_CLASS_BEGIN(UiCollapsingPanel) {
        WG_RTTI_FACTORY();
        WG_RTTI_FIELD(header, {});
        WG_RTTI_FIELD(default_open, {});
    }
    WG_RTTI_END;

}// namespace wmoge