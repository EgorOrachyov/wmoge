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

#include "core/array_view.hpp"
#include "math/color.hpp"
#include "ui/ui_defs.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_style.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace wmoge {

    /**
     * @class
     * @brief
     */
    class UiArea : public UiSubElement {
    public:
        void add_element(Ref<UiSubElement> element);

        [[nodiscard]] array_view<const Ref<UiSubElement>> get_elements() const { return m_elements; }

    protected:
        std::vector<Ref<UiSubElement>> m_elements;
    };

    class UiLayout : public UiArea {
    };

    class UiLayoutVertical : public UiLayout {
    };

    class UiLayoutHorizontal : public UiLayout {
    };

    /**
     * @class
     * @brief
     */
    class UiMenuItem : public UiSubElement {
    public:
    protected:
    };

    /**
     * @class
     * @brief
     */
    class UiMenuAction : public UiMenuItem {
    public:
        void set_on_click(UiOnClick callback) { m_on_click = std::move(callback); }
        void set_selected(bool selected) { m_selected = selected; }

        [[nodiscard]] const UiOnClick& get_in_click() const { return m_on_click; }
        [[nodiscard]] bool             get_selected() const { return m_selected; }

    protected:
        UiOnClick m_on_click;
        bool      m_selected = false;
    };

    /**
     * @class
     * @brief
     */
    class UiMenuGroup : public UiSubElement {
    public:
        void add_item(Ref<UiMenuItem> item);

        [[nodiscard]] array_view<const Ref<UiMenuItem>> get_items() const { return m_items; }

    protected:
        std::vector<Ref<UiMenuItem>> m_items;
    };

    /**
     * @class
     * @brief
     */
    class UiMenu : public UiMenuItem {
    public:
        void add_group(Ref<UiMenuGroup> group);

        [[nodiscard]] array_view<const Ref<UiMenuGroup>> get_groups() const { return m_groups; }

    protected:
        std::vector<Ref<UiMenuGroup>> m_groups;
    };

    /**
     * @class
     * @brief
     */
    class UiMenuBar : public UiSubElement {
    public:
        void add_menu(Ref<UiMenu> menu);

        [[nodiscard]] array_view<const Ref<UiMenu>> get_menus() const { return m_menus; }

    protected:
        std::vector<Ref<UiMenu>> m_menus;
    };

    /**
     * @class
     * @brief
     */
    class UiWindow : public UiElement {
    public:
        void set_menu_bar(Ref<UiMenuBar> menu_bar) { m_menu_bar = std::move(menu_bar); }
        void set_area(Ref<UiArea> area) { m_area = std::move(area); }
        void set_window_flags(UiWindowFlags flags) { m_window_flags = std::move(flags); }

        [[nodiscard]] const Ref<UiMenuBar>& get_menu_bar() const { return m_menu_bar; }
        [[nodiscard]] const Ref<UiArea>&    get_area() const { return m_area; }

        [[nodiscard]] bool has_menu_bar() const { return m_menu_bar; }
        [[nodiscard]] bool has_area() const { return m_area; }

    protected:
        Ref<UiMenuBar> m_menu_bar;
        Ref<UiArea>    m_area;
        UiWindowFlags  m_window_flags;
        bool           m_open = true;
    };

    /**
     * @class
     * @brief
     */
    class UiMainWindow : public UiWindow {
    public:
    protected:
    };

    /**
     * @class
     * @brief
     */
    class UiDockWindow : public UiWindow {
    public:
    protected:
    };

    /**
     * @class
     * @brief
     */
    class UiText : public UiSubElement {
    public:
        void set_text(std::string text) { m_text = std::move(text); }
        void set_text_color(std::optional<Color4f> color) { m_text_color = color; }

        [[nodiscard]] const std::string& get_text() const { return m_text; }

    protected:
        std::string            m_text;
        std::optional<Color4f> m_text_color;
    };

    class UiTextInputPopup : public UiArea {
    public:
        [[nodiscard]] float get_lines_mult() const { return m_lines_mult; }

    protected:
        float m_lines_mult = 6.0f;
    };

    /**
     * @class
     * @brief
     */
    class UiTextInput : public UiSubElement {
    public:
        void set_text(std::string text) { m_text = std::move(text); }
        void set_popup(Ref<UiTextInputPopup> popup) { m_popup = std::move(popup); }
        void set_text_flags(UiTextInputFlags flags) { m_text_flags = std::move(flags); }
        void set_on_text_input(UiOnTextInput callback) { m_on_text_input = std::move(callback); }

        [[nodiscard]] const std::string&           get_text() const { return m_text; }
        [[nodiscard]] UiTextInputFlags             get_text_flags() const { return m_text_flags; }
        [[nodiscard]] const UiOnTextInput&         get_on_text_input() const { return m_on_text_input; }
        [[nodiscard]] const Ref<UiTextInputPopup>& get_popup() const { return m_popup; }

    protected:
        std::string           m_text;
        std::vector<char>     m_input_buffer;
        Ref<UiTextInputPopup> m_popup;
        UiTextInputFlags      m_text_flags;
        UiOnTextInput         m_on_text_input;
        int                   m_input_buffer_capacity = 256;
    };

    /**
     * @class
     * @brief
     */
    class UiScrollArea : public UiArea {
    public:
        void set_scroll_type(UiScrollAreaType type) { m_scroll_type = type; }
        void set_scroll(std::optional<float> scroll) { m_scroll = scroll; }

        [[nodiscard]] std::optional<float> get_scroll() const { return m_scroll; }
        [[nodiscard]] UiScrollAreaType     get_scroll_type() const { return m_scroll_type; }

    protected:
        std::optional<float> m_scroll;
        UiScrollAreaType     m_scroll_type = UiScrollAreaType::Vertical;
    };

    class UiSelectable : public UiSubElement {
    public:
        void set_on_click(UiOnClick callback) { m_on_click = std::move(callback); }

        [[nodiscard]] const UiOnClick& get_in_click() const { return m_on_click; }

    protected:
        UiOnClick m_on_click;
    };

}// namespace wmoge