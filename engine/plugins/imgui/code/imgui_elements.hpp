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

    // class ImguiLayoutVertical : public ImguiElementBase<UiLayoutVertical> {
    // public:
    //     using ImguiElementBase<UiLayoutVertical>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiLayoutHorizontal : public ImguiElementBase<UiLayoutHorizontal> {
    // public:
    //     using ImguiElementBase<UiLayoutHorizontal>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiMenuAction : public ImguiElementBase<UiMenuAction> {
    // public:
    //     using ImguiElementBase<UiMenuAction>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiMenuGroup : public ImguiElementBase<UiMenuGroup> {
    // public:
    //     using ImguiElementBase<UiMenuGroup>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiMenu : public ImguiElementBase<UiMenu> {
    // public:
    //     using ImguiElementBase<UiMenu>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiMenuBar : public ImguiElementBase<UiMenuBar> {
    // public:
    //     using ImguiElementBase<UiMenuBar>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiMainWindow : public ImguiElementBase<UiMainWindow> {
    // public:
    //     using ImguiElementBase<UiMainWindow>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;

    // private:
    //     std::string m_dock_space_name = "main_window_dock_space";
    //     bool        m_fullscreen      = true;
    // };

    // class ImguiDockWindow : public ImguiElementBase<UiDockWindow> {
    // public:
    //     using ImguiElementBase<UiDockWindow>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiText : public ImguiElementBase<UiText> {
    // public:
    //     using ImguiElementBase<UiText>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiTextInput : public ImguiElementBase<UiTextInput> {
    // public:
    //     using ImguiElementBase<UiTextInput>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiTextInputPopup : public ImguiElementBase<UiTextInputPopup> {
    // public:
    //     using ImguiElementBase<UiTextInputPopup>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiScrollArea : public ImguiElementBase<UiScrollArea> {
    // public:
    //     using ImguiElementBase<UiScrollArea>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiSelectable : public ImguiElementBase<UiSelectable> {
    // public:
    //     using ImguiElementBase<UiSelectable>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

    // class ImguiIconButton : public ImguiElementBase<IconButton> {
    // public:
    //     using ImguiElementBase<IconButton>::ImguiElementBase;
    //     void process(ImguiProcessContext& context) override;
    // };

}// namespace wmoge