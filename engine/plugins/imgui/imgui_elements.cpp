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

#include "imgui_elements.hpp"

#include "imgui.h"
#include "imgui_manager.hpp"

namespace wmoge {

    void ImguiLayoutVertical::process(ImguiProcessContext& context) {
        for (const auto& element : m_elements) {
            ImguiElement* imgui_element = dynamic_cast<ImguiElement*>(element.get());
            imgui_element->process(context);
        }
    }

    void ImguiLayoutHorizontal::process(ImguiProcessContext& context) {
        const std::size_t size = m_elements.size();
        std::size_t       i    = 0;

        for (const auto& element : m_elements) {
            ImguiElement* imgui_element = dynamic_cast<ImguiElement*>(element.get());
            imgui_element->process(context);

            if (i + 1 < size) {
                ImGui::SameLine();
                i++;
            }
        }
    }

    void ImguiMenuAction::process(ImguiProcessContext& context) {
        if (ImGui::MenuItem(m_name.c_str(), "", m_selected, m_enabled)) {
            context.add_action([this]() {
                m_on_click();
            });
        }
    }

    void ImguiMenuGroup::process(ImguiProcessContext& context) {
        for (const auto& item : m_items) {
            ImguiElement* imgui_item = dynamic_cast<ImguiElement*>(item.get());
            imgui_item->process(context);
        }
    }

    void ImguiMenu::process(ImguiProcessContext& context) {
        if (ImGui::BeginMenu(m_name.c_str(), m_enabled)) {
            const std::size_t num_groups    = m_groups.size();
            std::size_t       current_group = 0;
            for (const auto& group : m_groups) {
                ImguiElement* imgui_group = dynamic_cast<ImguiElement*>(group.get());
                imgui_group->process(context);
                if (current_group + 1 < num_groups) {
                    ImGui::Separator();
                }
                current_group++;
            }
            ImGui::EndMenu();
        }
    }

    void ImguiMenuBar::process(ImguiProcessContext& context) {
        if (ImGui::BeginMenuBar()) {
            for (const auto& menu : m_menus) {
                ImguiElement* imgui_menu = dynamic_cast<ImguiElement*>(menu.get());
                imgui_menu->process(context);
            }
            ImGui::EndMenuBar();
        }
    }

    void ImguiMainWindow::process(ImguiProcessContext& context) {
        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags   window_flags    = ImGuiWindowFlags_NoDocking;

        if (has_menu_bar()) {
            window_flags |= ImGuiWindowFlags_MenuBar;
        }
        if (m_fullscreen) {
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
        if (m_window_flags.get(UiWindowFlag::NoBackground)) {
            window_flags |= ImGuiWindowFlags_NoBackground;
            dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
        }
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        if (m_window_flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin(m_name.c_str(), &m_open, window_flags);

        if (m_window_flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PopStyleVar();
        }
        if (m_fullscreen) {
            ImGui::PopStyleVar(2);
        }
        if (m_manager->is_docking_enable()) {
            ImGuiID dockspace_id = ImGui::GetID(m_dock_space_name.c_str());
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }
        if (has_menu_bar()) {
            ImguiElement* imgui_menu_bar = dynamic_cast<ImguiElement*>(m_menu_bar.get());
            imgui_menu_bar->process(context);
        }
        if (has_area()) {
            ImguiElement* imgui_area = dynamic_cast<ImguiElement*>(m_area.get());
            imgui_area->process(context);
        }

        ImGui::End();
    }

    void ImguiDockWindow::process(ImguiProcessContext& context) {
        if (!m_open) {
            return;
        }

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

        if (m_window_flags.get(UiWindowFlag::NoBringToFrontOnFocus)) {
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        }
        if (m_window_flags.get(UiWindowFlag::NoBackground)) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }
        if (m_window_flags.get(UiWindowFlag::NoTitleBar)) {
            window_flags |= ImGuiWindowFlags_NoTitleBar;
        }
        if (m_window_flags.get(UiWindowFlag::NoCollapse)) {
            window_flags |= ImGuiWindowFlags_NoCollapse;
        }
        if (m_window_flags.get(UiWindowFlag::NoScrollbar)) {
            window_flags |= ImGuiWindowFlags_NoScrollbar;
        }

        if (m_window_flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin(m_name.c_str(), &m_open, window_flags);

        if (m_window_flags.get(UiWindowFlag::NoPadding)) {
            ImGui::PopStyleVar();
        }

        if (has_menu_bar()) {
            ImguiElement* imgui_menu_bar = dynamic_cast<ImguiElement*>(m_menu_bar.get());
            imgui_menu_bar->process(context);
        }
        if (has_area()) {
            ImguiElement* imgui_area = dynamic_cast<ImguiElement*>(m_area.get());
            imgui_area->process(context);
        }

        ImGui::End();
    }

    void ImguiText::process(ImguiProcessContext& context) {
        const bool has_color = m_text_color.has_value();

        if (has_color) {
            const auto& color = *m_text_color;
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(color[0], color[1], color[2], color[3]));
        }

        ImGui::TextUnformatted(m_text.c_str());

        if (has_color) {
            ImGui::PopStyleColor();
        }
    }

    struct ImguiTextInputCallbackData {
        ImguiProcessContext* context;
        ImguiTextInput*      input;
    };

    static int imgui_text_input_callback(ImGuiInputTextCallbackData* data) {
        ImguiTextInputCallbackData* user_data = (ImguiTextInputCallbackData*) data->UserData;

        std::string text(data->Buf, data->BufTextLen);
        user_data->input->set_text(std::move(text));

        switch (data->EventFlag) {
            case ImGuiInputTextFlags_CallbackCompletion: {
                user_data->context->add_action([i = user_data->input]() {
                    UiTextInputEvent event;
                    event.type = UiTextInputEventType::Completion;
                    event.text = i->get_text();
                    i->get_on_text_input()(event);
                });
            } break;
            case ImGuiInputTextFlags_CallbackHistory: {
                UiTextInputScrollDir dir = UiTextInputScrollDir::None;
                if (data->EventKey == ImGuiKey_UpArrow) {
                    dir = UiTextInputScrollDir::Up;
                }
                if (data->EventKey == ImGuiKey_DownArrow) {
                    dir = UiTextInputScrollDir::Down;
                }

                user_data->context->add_action([i = user_data->input, dir]() {
                    UiTextInputEvent event;
                    event.type = UiTextInputEventType::History;
                    event.dir  = dir;
                    event.text = i->get_text();
                    i->get_on_text_input()(event);
                });
            } break;
            case ImGuiInputTextFlags_CallbackAlways: {
                user_data->context->add_action([i = user_data->input]() {
                    UiTextInputEvent event;
                    event.type = UiTextInputEventType::Always;
                    event.text = i->get_text();
                    i->get_on_text_input()(event);
                });
            } break;

            default:
                break;
        }

        return 0;
    }

    void ImguiTextInput::process(ImguiProcessContext& context) {
        ImguiTextInputCallbackData data{&context, this};
        ImGuiInputTextFlags        input_text_flags = ImGuiInputTextFlags_None;

        if (m_text_flags.get(UiTextInputFlag::CallbackAlways)) {
            input_text_flags |= ImGuiInputTextFlags_CallbackAlways;
        }
        if (m_text_flags.get(UiTextInputFlag::CompletionTab)) {
            input_text_flags |= ImGuiInputTextFlags_CallbackCompletion;
        }
        if (m_text_flags.get(UiTextInputFlag::HistoryScroll)) {
            input_text_flags |= ImGuiInputTextFlags_CallbackHistory;
        }
        if (m_text_flags.get(UiTextInputFlag::EscapeToClear)) {
            input_text_flags |= ImGuiInputTextFlags_EscapeClearsAll;
        }
        if (m_text_flags.get(UiTextInputFlag::EnterForSubmit)) {
            input_text_flags |= ImGuiInputTextFlags_EnterReturnsTrue;
        }

        m_input_buffer_capacity = Math::max(m_input_buffer_capacity, (int) (m_text.length() + 1));
        m_input_buffer.resize(m_input_buffer_capacity, '\0');
        std::memcpy(m_input_buffer.data(), m_text.data(), m_text.length() + 1);

        bool apply_width = false;

        if (m_hint_width.scale) {
            const ImVec2 size   = ImGui::CalcTextSize(m_name.c_str());
            const ImVec2 region = ImGui::GetContentRegionAvail();

            if (region.x * *m_hint_width.scale > size.x) {
                float width = region.x * *m_hint_width.scale - size.x;
                ImGui::PushItemWidth(width);
                apply_width = true;
            }
        }

        if (ImGui::InputText(m_name.c_str(), m_input_buffer.data(), m_input_buffer.size(), input_text_flags, &imgui_text_input_callback, (void*) &data)) {
            m_text = m_input_buffer.data();

            context.add_action([this]() {
                UiTextInputEvent event;
                event.text = m_text;
                event.type = UiTextInputEventType::Enter;
                get_on_text_input()(event);
            });
        }

        if (apply_width) {
            ImGui::PopItemWidth();
        }

        if (m_popup) {
            if (m_popup->get_enabled()) {
                ImGuiPopupFlags flags = ImGuiPopupFlags_None;
                ImGui::OpenPopup(m_popup->get_name().c_str(), flags);
            }

            const ImVec2 popup_size = ImVec2(
                    ImGui::GetItemRectSize().x,
                    ImGui::GetItemRectSize().y * m_popup->get_lines_mult());

            const ImVec2 popup_pos = ImVec2(
                    ImGui::GetItemRectMin().x,
                    ImGui::GetItemRectMin().y - popup_size.y);

            ImGui::SetNextWindowSize(popup_size);
            ImGui::SetNextWindowPos(popup_pos);

            ImguiElement* imgui_popup = dynamic_cast<ImguiElement*>(m_popup.get());
            imgui_popup->process(context);
        }
    }

    void ImguiTextInputPopup::process(ImguiProcessContext& context) {
        ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing;

        if (ImGui::BeginPopup(m_name.c_str(), flags)) {
            ImGui::PushAllowKeyboardFocus(false);
            for (auto& element : m_elements) {
                ImguiElement* imgui_element = dynamic_cast<ImguiElement*>(element.get());
                imgui_element->process(context);
            }
            if (!get_enabled()) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopAllowKeyboardFocus();
            ImGui::EndPopup();
        }
    }

    void ImguiScrollArea::process(ImguiProcessContext& context) {
        ImGuiChildFlags  child_flags  = ImGuiChildFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
        ImVec2           size(0, 0);

        if (m_scroll_type == UiScrollAreaType::Horizontal) {
            window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        }
        if (m_scroll_type == UiScrollAreaType::Vertical) {
            window_flags |= ImGuiChildFlags_None;
        }
        if (m_scroll_type == UiScrollAreaType::Mixed) {
            window_flags |= ImGuiWindowFlags_HorizontalScrollbar;
        }

        child_flags |= ImGuiChildFlags_Borders;

        // todo: layouts
        const float footer_height_to_reserve = ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;

        if (ImGui::BeginChild(m_name.c_str(), ImVec2(0, -footer_height_to_reserve), child_flags, window_flags)) {
            for (auto& element : m_elements) {
                ImguiElement* imgui_element = dynamic_cast<ImguiElement*>(element.get());
                imgui_element->process(context);
            }
            if (m_scroll) {
                ImGui::SetScrollHereY(*m_scroll);
                m_scroll = std::nullopt;
            }
        }
        ImGui::EndChild();
    }

    void ImguiSelectable::process(ImguiProcessContext& context) {
        if (ImGui::Selectable(m_name.c_str())) {
            m_on_click();
        }
    }

}// namespace wmoge