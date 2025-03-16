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

#include "imgui_window.hpp"

#include "imgui_manager.hpp"

namespace wmoge {

    void imgui_process_main_window(ImguiProcessor& processor, UiMainWindow& window) {
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_NoDocking;

        const bool has_menu_bar   = window.menu_bar.has_value();
        const bool has_tool_bar   = window.tool_bar.has_value();
        const bool has_status_bar = window.status_bar.has_value();
        const bool has_panel      = window.panel.has_value();
        const auto flags          = window.flags.get();

        if (has_menu_bar) {
            window_flags |= ImGuiWindowFlags_MenuBar;
        }
        if (window.is_fullscreen) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            window_flags |= ImGuiWindowFlags_NoNavFocus;
        }
        if (flags.get(UiWindowFlag::NoBackground)) {
            window_flags |= ImGuiWindowFlags_NoBackground;
            dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
        }
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin(window.title.get().c_str(), window.is_open.get_ptr(), window_flags);

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PopStyleVar();
        }
        if (window.is_fullscreen) {
            ImGui::PopStyleVar(2);
        }
        if (processor.get_manager()->is_docking_enable()) {
            ImGuiID dockspace_id = ImGui::GetID(window.title.get().c_str());
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (has_menu_bar) {
            processor.process(window.menu_bar.get());
        }
        if (has_tool_bar) {
            processor.process(window.tool_bar.get());
        }
        if (has_panel) {
            processor.process(window.panel.get());
        }
        if (has_status_bar) {
            processor.process(window.status_bar.get());
        }

        ImGui::End();
    }

    void imgui_process_dock_window(ImguiProcessor& processor, UiDockWindow& window) {
        if (!window.is_open) {
            return;
        }

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        const bool has_menu_bar   = window.menu_bar.has_value();
        const bool has_tool_bar   = window.tool_bar.has_value();
        const bool has_status_bar = window.status_bar.has_value();
        const bool has_panel      = window.panel.has_value();
        const auto flags          = window.flags.get();

        if (flags.get(UiWindowFlag::NoBringToFrontOnFocus)) {
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        }
        if (flags.get(UiWindowFlag::NoBackground)) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }
        if (flags.get(UiWindowFlag::NoTitleBar)) {
            window_flags |= ImGuiWindowFlags_NoTitleBar;
        }
        if (flags.get(UiWindowFlag::NoCollapse)) {
            window_flags |= ImGuiWindowFlags_NoCollapse;
        }
        if (flags.get(UiWindowFlag::NoScrollbar)) {
            window_flags |= ImGuiWindowFlags_NoScrollbar;
        }

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin(imgui_str(window.title), window.is_open.get_ptr(), window_flags);

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PopStyleVar();
        }

        if (has_menu_bar) {
            processor.process(window.menu_bar.get());
        }
        if (has_tool_bar) {
            processor.process(window.tool_bar.get());
        }
        if (has_panel) {
            processor.process(window.panel.get());
        }
        if (has_status_bar) {
            processor.process(window.status_bar.get());
        }

        ImGui::End();
    }

}// namespace wmoge