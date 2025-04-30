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
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

        const bool has_menu_bar   = window.menu_bar;
        const bool has_tool_bar   = window.tool_bar;
        const bool has_status_bar = window.status_bar;
        const bool has_content    = !window.children.empty();
        const auto flags          = window.flags;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        window_flags |= ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoTitleBar;

        if (has_menu_bar) {
            window_flags |= ImGuiWindowFlags_MenuBar;
        }

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin(imgui_str(window.title), &window.is_open, window_flags);

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PopStyleVar();
        }

        ImGui::PopStyleVar(2);

        if (has_menu_bar) {
            processor.process(window.menu_bar.get());
        }
        if (has_tool_bar) {
            processor.process(window.tool_bar.get());
        }
        if (has_content) {
            processor.process(window.children);
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

        const bool has_menu_bar   = window.menu_bar;
        const bool has_tool_bar   = window.tool_bar;
        const bool has_status_bar = window.status_bar;
        const bool has_content    = !window.children.empty();
        const auto flags          = window.flags;

        if (flags.get(UiWindowFlag::NoBringToFrontOnFocus)) {
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
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

        ImGui::Begin(imgui_str(window.title), &window.is_open, window_flags);

        if (flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PopStyleVar();
        }

        if (has_menu_bar) {
            processor.process(window.menu_bar.get());
        }
        if (has_tool_bar) {
            processor.process(window.tool_bar.get());
        }
        if (has_content) {
            processor.process(window.children);
        }
        if (has_status_bar) {
            processor.process(window.status_bar.get());
        }

        ImGui::End();
    }

    void imgui_process_dock_space(ImguiProcessor& processor, UiDockSpace& dock_space) {
        std::optional<ImGuiID> dockspace_id;

        if (processor.get_manager()->is_docking_enable()) {
            ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            dockspace_id = ImGui::GetID(imgui_str(dock_space.name));
            ImGui::DockSpace(*dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        for (const auto& child_window : dock_space.children) {
            if (dockspace_id) {
                ImGui::SetNextWindowDockID(*dockspace_id, ImGuiCond_FirstUseEver);
            }
            processor.process(child_window.get());
        }
    }

}// namespace wmoge