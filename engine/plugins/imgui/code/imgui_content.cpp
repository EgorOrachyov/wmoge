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

#include "imgui_content.hpp"

namespace wmoge {

    void imgui_process_separator(ImguiProcessor& processor, UiSeparator& element) {
        ImGui::Separator();
    }

    void imgui_process_separator_text(ImguiProcessor& processor, UiSeparatorText& element) {
        ImGui::SeparatorText(imgui_str(element.label));
    }

    void imgui_process_text(ImguiProcessor& processor, UiText& element) {
        ImGui::TextUnformatted(element.text.get().c_str());
    }

    void imgui_process_text_wrapped(ImguiProcessor& processor, UiTextWrapped& element) {
        ImGui::PushTextWrapPos();
        ImGui::TextUnformatted(element.text.get().c_str());
        ImGui::PopTextWrapPos();
    }

    void imgui_process_text_link(ImguiProcessor& processor, UiTextLink& element) {
        if (element.url.has_value()) {
            ImGui::TextLinkOpenURL(imgui_str(element.text), element.url.value().c_str());
        } else {
            if (ImGui::TextLink(imgui_str(element.text))) {
                processor.add_action_event(element.on_click);
            }
        }
    }

    void imgui_process_progress_bar(ImguiProcessor& processor, UiProgressBar& element) {
        float fraction = -0.0f;

        if (element.progress.has_value()) {
            fraction = element.progress.value();
        } else {
            fraction = -1.0f * static_cast<float>(ImGui::GetTime());
        }

        ImGui::ProgressBar(fraction, ImVec2(0.0f, 0.0f), imgui_str(element.label));
    }

}// namespace wmoge