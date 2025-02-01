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
#include "ui/ui_element.hpp"
#include "ui/ui_style.hpp"

#include <functional>
#include <string>
#include <vector>

namespace wmoge {

    /** @brief */
    using UiOnClick = std::function<void()>;

    /**
     * @class
     * @brief
     */
    class UiMenuItem : public UiElement {
    public:
        [[nodiscard]] const std::string& get_name() const { return m_name; }
        [[nodiscard]] bool               get_enabled() const { return m_enabled; }

    protected:
        std::string m_name;
        bool        m_enabled = true;
    };

    /**
     * @class
     * @brief
     */
    class UiMenuAction : public UiMenuItem {
    public:
        [[nodiscard]] const UiOnClick& get_callback() const { return m_callback; }
        [[nodiscard]] bool             get_selected() const { return m_selected; }

    protected:
        UiOnClick m_callback;
        bool      m_selected = false;
    };

    /**
     * @class
     * @brief
     */
    class UiMenuGroup : public UiElement {
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
    class UiMenuBar : public UiElement {
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
        [[nodiscard]] const std::string&    get_name() const { return m_name; }
        [[nodiscard]] const Ref<UiMenuBar>& get_menu_bar() const { return m_menu_bar; }

        [[nodiscard]] bool has_menu_bar() const { return m_menu_bar; }

    protected:
        std::string    m_name;
        Ref<UiMenuBar> m_menu_bar;
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

}// namespace wmoge