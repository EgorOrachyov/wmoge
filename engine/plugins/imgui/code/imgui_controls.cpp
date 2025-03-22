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

#include "imgui_controls.hpp"

#include "imgui_manager.hpp"

namespace wmoge {

    void imgui_process_menu_item(ImguiProcessor& processor, UiMenuItem& element) {
        if (ImGui::MenuItem(imgui_str(element.label), imgui_str(element.shortcut), element.selected, !element.disabled)) {
            processor.add_action_event(element.on_click);
        }
    }

    void imgui_process_selectable(ImguiProcessor& processor, UiSelectable& element) {
        ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;

        if (element.disabled) {
            flags |= ImGuiSelectableFlags_Disabled;
        }

        if (ImGui::Selectable("", element.selected, flags)) {
            processor.add_action_event(element.on_click);
        }

        if (!element.icon.is_empty()) {
            ImGui::SameLine();
            const float pos_y = ImGui::GetCursorPosY();
            ImGui::SetCursorPosY(pos_y - ImGui::GetStyle().FramePadding.y);
            processor.draw_icon(element.icon, element.icon_size);
            ImGui::SetCursorPosY(pos_y);
        }
        if (!element.label.empty()) {
            ImGui::SameLine();
            ImGui::Text(imgui_str(element.label));
        }
    }

    void imgui_process_button(ImguiProcessor& processor, UiButton& element) {
        if (element.icon.is_empty()) {
            if (ImGui::Button(imgui_str(element.label))) {
                processor.add_action_event(element.on_click);
            }
        } else {
            const Icon&           icon  = element.icon;
            const Ref<IconAtlas>& atlas = icon.get_atlas();
            const IconInfo&       info  = atlas->get_icon_info(icon.get_id());
            const IconAtlasPage&  page  = atlas->get_page(info.page_id);

            const ImTextureID texture_id   = processor.get_manager()->get_texture_id(page.texture);
            const ImVec2      texture_size = imgui_vec2(element.icon_size * info.pixels);
            const ImVec2      uv0          = imgui_vec2(info.uv_pos);
            const ImVec2      uv1          = imgui_vec2(info.uv_pos + info.uv_size);
            const ImVec4      tint         = imgui_color4(info.tint);
            const ImVec4      bg_color;

            if (element.label.empty()) {
                if (ImGui::ImageButton(imgui_str(element.label), texture_id, texture_size, uv0, uv1, bg_color, tint)) {
                    processor.add_action_event(element.on_click);
                }
            } else {
                if (ImGui::ImageButtonWithLabel(imgui_str(element.label), imgui_str(element.label), texture_id, texture_size, uv0, uv1, bg_color, tint)) {
                    processor.add_action_event(element.on_click);
                }
            }
        }
    }

    void imgui_process_check_box_button(ImguiProcessor& processor, UiCheckBoxButton& element) {
        if (ImGui::Checkbox(imgui_str(element.label), &element.checked)) {
            processor.add_action_event(element.on_click);
        }
    }

    void imgui_process_radio_button(ImguiProcessor& processor, UiRadioButton& element) {
        if (ImGui::RadioButton(imgui_str(element.label), element.checked)) {
            processor.add_action_event(element.on_click);
        }
    }

    static const char* imgui_combo_box_items_getter(void* user_data, int idx) {
        UiComboBox* element = reinterpret_cast<UiComboBox*>(user_data);
        return element->items[idx].c_str();
    }

    void imgui_process_combo_box(ImguiProcessor& processor, UiComboBox& element) {
        int current_item = -1;
        int max_height   = -1;
        int items_count  = static_cast<int>(element.items.size());

        if (element.current_item.has_value()) {
            current_item = element.current_item.value();
        }
        if (element.max_popup_items.has_value()) {
            max_height = element.max_popup_items.value();
        }

        if (ImGui::Combo(imgui_str(element.label), &current_item, imgui_combo_box_items_getter, &element, items_count, max_height)) {
            processor.add_action_event(element.on_click);
        }

        if (current_item != -1) {
            element.current_item = current_item;
        }
    }

    static const char* imgui_list_box_items_getter(void* user_data, int idx) {
        UiListBox* element = reinterpret_cast<UiListBox*>(user_data);
        return element->items[idx].c_str();
    }

    void imgui_process_list_box(ImguiProcessor& processor, UiListBox& element) {
        int current_item = -1;
        int height       = -1;
        int items_count  = static_cast<int>(element.items.size());

        if (element.current_item.has_value()) {
            current_item = element.current_item.value();
        }
        if (element.height_in_items.has_value()) {
            height = element.height_in_items.value();
        }

        if (ImGui::ListBox(imgui_str(element.label), &current_item, imgui_list_box_items_getter, &element, items_count, height)) {
            processor.add_action_event(element.on_click);
        }

        if (current_item != -1) {
            element.current_item = current_item;
        }
    }

}// namespace wmoge