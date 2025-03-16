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

#include "imgui_inputs.hpp"

#include <cstring>

namespace wmoge {

    void imgui_process_drag_int(ImguiProcessor& processor, UiDragInt& element) {
        float       v_speed = element.speed.has_value() ? element.speed.value() : 1.0f;
        int         v_min   = element.value_min.has_value() ? element.value_min.value() : 0;
        int         v_max   = element.value_max.has_value() ? element.value_max.value() : 0;
        const char* format  = element.format.has_value() ? element.format.value().c_str() : "%d";

        if (ImGui::DragInt(imgui_str(element.label), element.value.get_ptr(), v_speed, v_min, v_max, format)) {
            processor.add_action_event(element.on_input);
        }
    }

    void imgui_process_drag_float(ImguiProcessor& processor, UiDragFloat& element) {
        float       v_speed = element.speed.has_value() ? element.speed.value() : 1.0f;
        float       v_min   = element.value_min.has_value() ? element.value_min.value() : 0.0f;
        float       v_max   = element.value_max.has_value() ? element.value_max.value() : 0.0f;
        const char* format  = element.format.has_value() ? element.format.value().c_str() : "%.3f";

        if (ImGui::DragFloat(imgui_str(element.label), element.value.get_ptr(), v_speed, v_min, v_max, format)) {
            processor.add_action_event(element.on_input);
        }
    }

    void imgui_process_slider_int(ImguiProcessor& processor, UiSliderInt& element) {
        int         v_min  = element.value_min;
        int         v_max  = element.value_max;
        const char* format = element.format.has_value() ? element.format.value().c_str() : "%d";

        if (ImGui::SliderInt(imgui_str(element.label), element.value.get_ptr(), v_min, v_max, format)) {
            processor.add_action_event(element.on_input);
        }
    }

    void imgui_process_slider_float(ImguiProcessor& processor, UiSliderFloat& element) {
        float       v_min  = element.value_min;
        float       v_max  = element.value_max;
        const char* format = element.format.has_value() ? element.format.value().c_str() : "%.3f";

        if (ImGui::SliderFloat(imgui_str(element.label), element.value.get_ptr(), v_min, v_max, format)) {
            processor.add_action_event(element.on_input);
        }
    }

    void imgui_process_input_int(ImguiProcessor& processor, UiInputInt& element) {
        int step      = element.step.has_value() ? element.step.value() : 1;
        int step_fast = element.step_fast.has_value() ? element.step_fast.value() : 100;

        if (ImGui::InputInt(imgui_str(element.label), element.value.get_ptr()), step, step_fast) {
            processor.add_action_event(element.on_input);
        }
    }

    void imgui_process_input_float(ImguiProcessor& processor, UiInputFloat& element) {
        float step      = element.step.has_value() ? element.step.value() : 0.0f;
        float step_fast = element.step_fast.has_value() ? element.step_fast.value() : 0.0f;

        if (ImGui::InputFloat(imgui_str(element.label), element.value.get_ptr()), step, step_fast) {
            processor.add_action_event(element.on_input);
        }
    }

    void imgui_process_input_text(ImguiProcessor& processor, UiInputText& element) {
        ImGuiInputTextFlags flags  = ImGuiInputTextFlags_None;
        auto                buffer = processor.put_str_to_buffer(element.text);

        ImGui::PushID(&element);

        if (element.hint.has_value()) {
            if (ImGui::InputTextWithHint("##input", element.hint.value().c_str(), buffer.data(), static_cast<int>(buffer.size()), flags)) {
                processor.add_action_event(element.on_input);
            }
        } else {
            if (ImGui::InputText("##input", buffer.data(), static_cast<int>(buffer.size()), flags)) {
                processor.add_action_event(element.on_input);
            }
        }

        element.text = std::move(processor.pop_str_from_buffer());

        ImGui::PopID();
    }

    struct ImguiInputTextUserData {
        ImguiProcessor& processor;
        UiInputTextExt& element;
    };

    int imgui_input_text_callback(ImGuiInputTextCallbackData* data) {
        ImguiInputTextUserData* user_data = reinterpret_cast<ImguiInputTextUserData*>(data->UserData);

        switch (data->EventFlag) {
            case ImGuiInputTextFlags_CallbackCompletion:
                user_data->processor.add_action_event(user_data->element.on_completion);
                break;
            case ImGuiInputTextFlags_CallbackHistory:
                if (data->EventKey == ImGuiKey_UpArrow) {
                    user_data->processor.add_action_event(user_data->element.on_history_prev);
                }
                if (data->EventKey == ImGuiKey_DownArrow) {
                    user_data->processor.add_action_event(user_data->element.on_history_next);
                }
                break;
            case ImGuiInputTextFlags_CallbackAlways:
                user_data->processor.add_action_event(user_data->element.on_input);
                break;

            default:
                break;
        }

        return 0;
    }

    void imgui_process_input_text_ext(ImguiProcessor& processor, UiInputTextExt& element) {
        ImGuiInputTextFlags flags  = ImGuiInputTextFlags_None;
        auto                buffer = processor.put_str_to_buffer(element.text);

        ImguiInputTextUserData user_data{processor, element};

        flags |= ImGuiInputTextFlags_CallbackAlways;

        if (element.on_enter.has_callback()) {
            flags |= ImGuiInputTextFlags_EnterReturnsTrue;
        }
        if (element.on_completion.has_callback()) {
            flags |= ImGuiInputTextFlags_CallbackCompletion;
        }
        if (element.on_history_prev.has_callback() || element.on_history_next.has_callback()) {
            flags |= ImGuiInputTextFlags_CallbackHistory;
        }
        if (element.esc_to_clear) {
            flags |= ImGuiInputTextFlags_EscapeClearsAll;
        }

        ImGui::PushID(&element);

        if (element.hint.has_value()) {
            if (ImGui::InputTextWithHint("##input", element.hint.value().c_str(), buffer.data(), static_cast<int>(buffer.size()), flags, imgui_input_text_callback, &user_data)) {
                processor.add_action_event(element.on_enter);
            }
        } else {
            if (ImGui::InputText("##input", buffer.data(), static_cast<int>(buffer.size()), flags, imgui_input_text_callback, &user_data)) {
                processor.add_action_event(element.on_enter);
            }
        }

        element.text = std::move(processor.pop_str_from_buffer());

        if (element.completion_popup.has_value()) {
            if (element.completion_popup.get()->should_show) {
                ImGui::OpenPopup(imgui_str(element.completion_popup.get()->name));
            }

            const ImVec2 popup_size = ImVec2(
                    ImGui::GetItemRectSize().x,
                    ImGui::GetItemRectSize().y * float(element.completion_popup_lines.get()));

            const ImVec2 popup_pos = ImVec2(
                    ImGui::GetItemRectMin().x,
                    element.completion_popup_bottom ? ImGui::GetItemRectMax().y : ImGui::GetItemRectMin().y - popup_size.y);

            ImGui::SetNextWindowSize(popup_size);
            ImGui::SetNextWindowPos(popup_pos);

            processor.process(element.completion_popup.get());
        }

        ImGui::PopID();
    }

}// namespace wmoge