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

#include "imgui_process.hpp"

#include "imgui_bars.hpp"
#include "imgui_containers.hpp"
#include "imgui_content.hpp"
#include "imgui_controls.hpp"
#include "imgui_inputs.hpp"
#include "imgui_manager.hpp"
#include "imgui_window.hpp"

#include <cstring>

namespace wmoge {

    ImguiProcessor::ImguiProcessor(ImguiManager* manager)
        : m_manager(manager),
          m_input_buffer(256, '\0') {
    }

    void ImguiProcessor::process(UiElement* element) {
        if (!element) {
            return;
        }

        const UiElementType type = element->type;

        switch (type) {
            case UiElementType::MainWindow:
                imgui_process_main_window(*this, *static_cast<UiMainWindow*>(element));
                break;
            case UiElementType::DockWindow:
                imgui_process_dock_window(*this, *static_cast<UiDockWindow*>(element));
                break;

            case UiElementType::ContextMenu:
                imgui_process_context_menu(*this, *static_cast<UiContextMenu*>(element));
                break;
            case UiElementType::Menu:
                imgui_process_menu(*this, *static_cast<UiMenu*>(element));
                break;
            case UiElementType::Popup:
                imgui_process_popup(*this, *static_cast<UiPopup*>(element));
                break;
            case UiElementType::CompletionPopup:
                imgui_process_completion_popup(*this, *static_cast<UiCompletionPopup*>(element));
                break;
            case UiElementType::Modal:
                imgui_process_modal(*this, *static_cast<UiModal*>(element));
                break;
            case UiElementType::StackPanel:
                imgui_process_stack_panel(*this, *static_cast<UiStackPanel*>(element));
                break;
            case UiElementType::ScrollPanel:
                imgui_process_scroll_panel(*this, *static_cast<UiScrollPanel*>(element));
                break;

            case UiElementType::MenuBar:
                imgui_process_menu_bar(*this, *static_cast<UiMenuBar*>(element));
                break;
            case UiElementType::ToolBar:
                imgui_process_tool_bar(*this, *static_cast<UiToolBar*>(element));
                break;
            case UiElementType::StatusBar:
                imgui_process_status_bar(*this, *static_cast<UiStatusBar*>(element));
                break;

            case UiElementType::Separator:
                imgui_process_separator(*this, *static_cast<UiSeparator*>(element));
                break;
            case UiElementType::SeparatorText:
                imgui_process_separator_text(*this, *static_cast<UiSeparatorText*>(element));
                break;
            case UiElementType::Text:
                imgui_process_text(*this, *static_cast<UiText*>(element));
                break;
            case UiElementType::TextWrapped:
                imgui_process_text_wrapped(*this, *static_cast<UiTextWrapped*>(element));
                break;
            case UiElementType::TextLink:
                imgui_process_text_link(*this, *static_cast<UiTextLink*>(element));
                break;
            case UiElementType::ProgressBar:
                imgui_process_progress_bar(*this, *static_cast<UiProgressBar*>(element));
                break;

            case UiElementType::MenuItem:
                imgui_process_menu_item(*this, *static_cast<UiMenuItem*>(element));
                break;
            case UiElementType::Selectable:
                imgui_process_selectable(*this, *static_cast<UiSelectable*>(element));
                break;
            case UiElementType::Button:
                imgui_process_button(*this, *static_cast<UiButton*>(element));
                break;
            case UiElementType::CheckBoxButton:
                imgui_process_check_box_button(*this, *static_cast<UiCheckBoxButton*>(element));
                break;
            case UiElementType::RadioButton:
                imgui_process_radio_button(*this, *static_cast<UiRadioButton*>(element));
                break;
            case UiElementType::ComboBox:
                imgui_process_combo_box(*this, *static_cast<UiComboBox*>(element));
                break;
            case UiElementType::ListBox:
                imgui_process_list_box(*this, *static_cast<UiListBox*>(element));
                break;

            case UiElementType::DragInt:
                imgui_process_drag_int(*this, *static_cast<UiDragInt*>(element));
                break;
            case UiElementType::DragFloat:
                imgui_process_drag_float(*this, *static_cast<UiDragFloat*>(element));
                break;
            case UiElementType::SliderInt:
                imgui_process_slider_int(*this, *static_cast<UiSliderInt*>(element));
                break;
            case UiElementType::SliderFloat:
                imgui_process_slider_float(*this, *static_cast<UiSliderFloat*>(element));
                break;
            case UiElementType::InputInt:
                imgui_process_input_int(*this, *static_cast<UiInputInt*>(element));
                break;
            case UiElementType::InputFloat:
                imgui_process_input_float(*this, *static_cast<UiInputFloat*>(element));
                break;
            case UiElementType::InputText:
                imgui_process_input_text(*this, *static_cast<UiInputText*>(element));
                break;
            case UiElementType::InputTextExt:
                imgui_process_input_text_ext(*this, *static_cast<UiInputTextExt*>(element));
                break;

            default:
                WG_LOG_ERROR("unknown type of ui element with tag " << element->tag);
                break;
        }
    }

    void ImguiProcessor::process(UiSlots<UiSlot<UiSubElement>>& elements) {
        auto& children = elements.get();
        for (const auto& child : children) {
            process(child.get());
        }
    }

    void ImguiProcessor::draw_icon(const Icon& icon, const Vec2f& icon_size) {
        const Ref<IconAtlas>& atlas = icon.get_atlas();
        const IconInfo&       info  = atlas->get_icon_info(icon.get_id());
        const IconAtlasPage&  page  = atlas->get_page(info.page_id);

        const ImTextureID texture_id   = m_manager->get_texture_id(page.texture);
        const ImVec2      texture_size = imgui_vec2(icon_size * info.pixels);
        const ImVec2      uv0          = imgui_vec2(info.uv_pos);
        const ImVec2      uv1          = imgui_vec2(info.uv_pos + info.uv_size);
        const ImVec4      tint         = imgui_color4(info.tint);
        const ImVec4      border_color;

        ImGui::Image(texture_id, texture_size, uv0, uv1, tint, border_color);
    }

    void ImguiProcessor::add_action_event(UiEvent<std::function<void()>>& event) {
        if (event.has_callback()) {
            add_action([e = &event]() {
                e->get()();
            });
        }
    }

    void ImguiProcessor::add_action(std::function<void()> action) {
        m_actions.push_back(std::move(action));
    }

    void ImguiProcessor::dispatch_actions() {
        for (const auto& action : m_actions) {
            action();
        }
    }

    void ImguiProcessor::clear_actions() {
        m_actions.clear();
    }

    array_view<char> ImguiProcessor::put_str_to_buffer(const std::string& s) {
        while (s.length() >= m_input_buffer.size()) {
            m_input_buffer.resize(m_input_buffer.size() * 2, '\0');
        }

        std::memcpy(m_input_buffer.data(), s.c_str(), s.length() + 1);

        return array_view<char>(m_input_buffer.data(), m_input_buffer.size());
    }

    std::string ImguiProcessor::pop_str_from_buffer() {
        std::string r(m_input_buffer.data());

        if (r.length() * 2 >= m_input_buffer.size()) {
            m_input_buffer.resize(m_input_buffer.size() * 2, '\0');
        }

        return std::move(r);
    }

}// namespace wmoge