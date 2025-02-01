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

#include "imgui_element.hpp"
#include "ui/ui_element.hpp"
#include "ui/ui_elements.hpp"

namespace wmoge {

    class ImguiMenuAction : public ImguiElementBase<UiMenuAction> {
    public:
        ImguiMenuAction(ImguiManager* manager, std::string name, UiOnClick callback);

        void process(ImguiProcessContext& context) override;
    };

    class ImguiMenuGroup : public ImguiElementBase<UiMenuGroup> {
    public:
        ImguiMenuGroup(ImguiManager* manager);

        void process(ImguiProcessContext& context) override;
    };

    class ImguiMenu : public ImguiElementBase<UiMenu> {
    public:
        ImguiMenu(ImguiManager* manager, std::string name);

        void process(ImguiProcessContext& context) override;
    };

    class ImguiMenuBar : public ImguiElementBase<UiMenuBar> {
    public:
        ImguiMenuBar(ImguiManager* manager);

        void process(ImguiProcessContext& context) override;
    };

    class ImguiMainWindow : public ImguiElementBase<UiMainWindow> {
    public:
        ImguiMainWindow(ImguiManager* manager, std::string name, Ref<UiMenuBar> menu_bar);

        void process(ImguiProcessContext& context) override;

    private:
        std::string m_dock_space_name = "main_window_dock_space";
        bool        m_fullscreen      = true;
        bool        m_padding         = false;
        bool        m_open            = true;
        bool        m_no_background   = true;
    };

    class ImguiDockWindow : public ImguiElementBase<UiDockWindow> {
    public:
        ImguiDockWindow(ImguiManager* manager, std::string name);

        void process(ImguiProcessContext& context) override;
    };

}// namespace wmoge