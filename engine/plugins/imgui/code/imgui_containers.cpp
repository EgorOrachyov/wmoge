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

#include "imgui_containers.hpp"

namespace wmoge {

    void imgui_process_context_menu(ImguiProcessor& processor, UiContextMenu& menu) {
        if (ImGui::BeginPopupContextItem()) {
            processor.process(menu.children);
            ImGui::EndPopup();
        }
    }

    void imgui_process_menu(ImguiProcessor& processor, UiMenu& menu) {
        if (ImGui::BeginMenu(imgui_str(menu.label), menu.enabled.get())) {
            processor.process(menu.children);
            ImGui::EndMenu();
        }
    }

    void imgui_process_popup(ImguiProcessor& processor, UiPopup& popup) {
        ImGuiWindowFlags flags = ImGuiWindowFlags_None;

        if (ImGui::BeginPopup(imgui_str(popup.name), flags)) {
            processor.process(popup.children);

            if (!popup.should_show) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void imgui_process_completion_popup(ImguiProcessor& processor, UiCompletionPopup& popup) {
        ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing;

        if (ImGui::BeginPopup(imgui_str(popup.name), flags)) {
            ImGui::PushAllowKeyboardFocus(false);

            processor.process(popup.children);

            if (!popup.should_show) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::PopAllowKeyboardFocus();
            ImGui::EndPopup();
        }
    }

    void imgui_process_modal(ImguiProcessor& processor, UiModal& modal) {
        if (ImGui::BeginPopupModal(imgui_str(modal.name))) {
            processor.process(modal.children);
            ImGui::EndPopup();
        }
    }

    void imgui_process_stack_panel(ImguiProcessor& processor, UiStackPanel& panel) {
        ImGui::BeginGroup();

        const auto&       children = panel.children.get();
        const std::size_t count    = children.size();

        for (std::size_t i = 0; i < count; i++) {
            processor.process(children[i].get());

            if (i + 1 != count && panel.orientation == UiOrientation::Horizontal) {
                ImGui::SameLine();
            }
        }

        ImGui::EndGroup();
    }

    void imgui_process_scroll_panel(ImguiProcessor& processor, UiScrollPanel& panel) {
        ImGuiChildFlags  child_flags  = ImGuiChildFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        if (panel.scroll_type == UiScroll::Vertical) {
            window_flags |= ImGuiChildFlags_None;
            child_flags |= ImGuiChildFlags_AutoResizeX;
        }
        if (panel.scroll_type == UiScroll::Horizontal) {
            window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
            child_flags |= ImGuiChildFlags_AutoResizeY;
        }
        if (panel.scroll_type == UiScroll::Mixed) {
            window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        }

        if (panel.has_border.get()) {
            child_flags |= ImGuiChildFlags_Borders;
        }
        if (panel.allow_resize.get()) {
            if (panel.scroll_type == UiScroll::Vertical) {
                child_flags |= ImGuiChildFlags_ResizeX;
            }
            if (panel.scroll_type == UiScroll::Horizontal) {
                child_flags |= ImGuiChildFlags_ResizeY;
            }
            if (panel.scroll_type == UiScroll::Mixed) {
                child_flags |= ImGuiChildFlags_ResizeX;
                child_flags |= ImGuiChildFlags_ResizeY;
            }
        }

        ImVec2 size(0, 0);

        if (panel.area_portion_y.has_value() && (panel.scroll_type == UiScroll::Vertical || panel.scroll_type == UiScroll::Mixed)) {
            size.y = ImGui::GetContentRegionAvail().y * panel.area_portion_y.value();
        }
        if (panel.area_portion_x.has_value() && (panel.scroll_type == UiScroll::Horizontal || panel.scroll_type == UiScroll::Mixed)) {
            size.x = ImGui::GetContentRegionAvail().x * panel.area_portion_x.value();
        }

        if (ImGui::BeginChild(imgui_str(panel.name), size, child_flags, window_flags)) {
            processor.process(panel.children);

            if (panel.scroll_value_x.has_value()) {
                ImGui::SetScrollHereX(panel.scroll_value_x.value());
                panel.scroll_value_x.reset();
            }
            if (panel.scroll_value_y.has_value()) {
                ImGui::SetScrollHereY(panel.scroll_value_y.value());
                panel.scroll_value_y.reset();
            }
        }
        ImGui::EndChild();
    }

    void imgui_process_collapsing_panel(ImguiProcessor& processor, UiCollapsingPanel& panel) {
        ImGuiTreeNodeFlags flags =
                ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_NoAutoOpenOnLog;

        if (panel.default_open) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        ImGui::PushStyleColor(ImGuiCol_Header, imgui_color4(processor.get_color(UiColor::CollapsingHeader)));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, imgui_color4(processor.get_color(UiColor::CollapsingHeaderHovered)));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, imgui_color4(processor.get_color(UiColor::CollapsingHeaderActive)));

        const bool is_open = ImGui::TreeNodeEx("##header", flags);

        ImGui::PopStyleColor(3);

        if (panel.header.has_value()) {
            ImGui::SameLine();
            processor.process(panel.header.get());
        }

        if (is_open) {
            processor.process(panel.children);
            ImGui::TreePop();
        }
    }

}// namespace wmoge