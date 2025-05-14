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
#include <utility>

namespace wmoge {

    static void imgui_push_var(UiParam param, float v) {
        switch (param) {
            case UiParam::Alpha:
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, v);
                break;
            case UiParam::DisabledAlpha:
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, v);
                break;
            case UiParam::WindowPaddingX:
                ImGui::PushStyleVarX(ImGuiStyleVar_WindowPadding, v);
                break;
            case UiParam::WindowPaddingY:
                ImGui::PushStyleVarY(ImGuiStyleVar_WindowPadding, v);
                break;
            case UiParam::WindowRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, v);
                break;
            case UiParam::WindowBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, v);
                break;
            case UiParam::WindowMinSizeX:
                ImGui::PushStyleVarX(ImGuiStyleVar_WindowMinSize, v);
                break;
            case UiParam::WindowMinSizeY:
                ImGui::PushStyleVarY(ImGuiStyleVar_WindowMinSize, v);
                break;
            case UiParam::WindowTitleAlignX:
                ImGui::PushStyleVarX(ImGuiStyleVar_WindowTitleAlign, v);
                break;
            case UiParam::WindowTitleAlignY:
                ImGui::PushStyleVarY(ImGuiStyleVar_WindowTitleAlign, v);
                break;
            case UiParam::ChildRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, v);
                break;
            case UiParam::ChildBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, v);
                break;
            case UiParam::PopupRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, v);
                break;
            case UiParam::PopupBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, v);
                break;
            case UiParam::FramePaddingX:
                ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, v);
                break;
            case UiParam::FramePaddingY:
                ImGui::PushStyleVarY(ImGuiStyleVar_FramePadding, v);
                break;
            case UiParam::FrameRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, v);
                break;
            case UiParam::FrameBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, v);
                break;
            case UiParam::ItemSpacingX:
                ImGui::PushStyleVarX(ImGuiStyleVar_ItemSpacing, v);
                break;
            case UiParam::ItemSpacingY:
                ImGui::PushStyleVarY(ImGuiStyleVar_ItemSpacing, v);
                break;
            case UiParam::ItemInnerSpacingX:
                ImGui::PushStyleVarX(ImGuiStyleVar_ItemInnerSpacing, v);
                break;
            case UiParam::ItemInnerSpacingY:
                ImGui::PushStyleVarY(ImGuiStyleVar_ItemInnerSpacing, v);
                break;
            case UiParam::IndentSpacing:
                ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, v);
                break;
            case UiParam::CellPaddingX:
                ImGui::PushStyleVarX(ImGuiStyleVar_CellPadding, v);
                break;
            case UiParam::CellPaddingY:
                ImGui::PushStyleVarY(ImGuiStyleVar_CellPadding, v);
                break;
            case UiParam::ScrollbarSize:
                ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, v);
                break;
            case UiParam::ScrollbarRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, v);
                break;
            case UiParam::GrabMinSize:
                ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, v);
                break;
            case UiParam::GrabRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, v);
                break;
            case UiParam::TabRounding:
                ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, v);
                break;
            case UiParam::TabBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, v);
                break;
            case UiParam::TabBarBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, v);
                break;
            case UiParam::TabBarOverlineSize:
                ImGui::PushStyleVar(ImGuiStyleVar_TabBarOverlineSize, v);
                break;
            case UiParam::TableAngledHeadersAngle:
                ImGui::PushStyleVar(ImGuiStyleVar_TableAngledHeadersAngle, v);
                break;
            case UiParam::TableAngledHeadersTextAlignX:
                ImGui::PushStyleVarX(ImGuiStyleVar_TableAngledHeadersTextAlign, v);
                break;
            case UiParam::TableAngledHeadersTextAlignY:
                ImGui::PushStyleVarY(ImGuiStyleVar_TableAngledHeadersTextAlign, v);
                break;
            case UiParam::ButtonTextAlignX:
                ImGui::PushStyleVarX(ImGuiStyleVar_ButtonTextAlign, v);
                break;
            case UiParam::ButtonTextAlignY:
                ImGui::PushStyleVarY(ImGuiStyleVar_ButtonTextAlign, v);
                break;
            case UiParam::SelectableTextAlignX:
                ImGui::PushStyleVarX(ImGuiStyleVar_SelectableTextAlign, v);
                break;
            case UiParam::SelectableTextAlignY:
                ImGui::PushStyleVarY(ImGuiStyleVar_SelectableTextAlign, v);
                break;
            case UiParam::SeparatorTextBorderSize:
                ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, v);
                break;
            case UiParam::SeparatorTextAlignX:
                ImGui::PushStyleVarX(ImGuiStyleVar_SeparatorTextAlign, v);
                break;
            case UiParam::SeparatorTextAlignY:
                ImGui::PushStyleVarY(ImGuiStyleVar_SeparatorTextAlign, v);
                break;
            case UiParam::SeparatorTextPaddingX:
                ImGui::PushStyleVarX(ImGuiStyleVar_SeparatorTextPadding, v);
                break;
            case UiParam::SeparatorTextPaddingY:
                ImGui::PushStyleVarY(ImGuiStyleVar_SeparatorTextPadding, v);
                break;
            case UiParam::DockingSeparatorSize:
                ImGui::PushStyleVar(ImGuiStyleVar_DockingSeparatorSize, v);
                break;

            default:
                break;
        }
    }

    static bool imgui_need_id(const UiElementType type) {
        switch (type) {
            case UiElementType::MainWindow:
            case UiElementType::DockWindow:
            case UiElementType::DockSpace:
                return false;

            case UiElementType::ContextMenu:
            case UiElementType::Menu:
            case UiElementType::Popup:
            case UiElementType::CompletionPopup:
            case UiElementType::Modal:
            case UiElementType::StackPanel:
            case UiElementType::ScrollPanel:
            case UiElementType::CollapsingPanel:
                return false;

            case UiElementType::MenuBar:
            case UiElementType::ToolBar:
            case UiElementType::StatusBar:
                return false;

            case UiElementType::Separator:
            case UiElementType::SeparatorText:
            case UiElementType::Text:
            case UiElementType::TextWrapped:
            case UiElementType::TextLink:
            case UiElementType::ProgressBar:
                return false;

            case UiElementType::MenuItem:
            case UiElementType::Selectable:
            case UiElementType::Button:
            case UiElementType::CheckBoxButton:
            case UiElementType::RadioButton:
            case UiElementType::ComboBox:
            case UiElementType::ListBox:
                return true;

            case UiElementType::DragInt:
            case UiElementType::DragFloat:
            case UiElementType::SliderInt:
            case UiElementType::SliderFloat:
            case UiElementType::InputInt:
            case UiElementType::InputFloat:
            case UiElementType::InputText:
            case UiElementType::InputTextExt:
                return false;

            default:
                return false;
        }
    }

    ImguiProcessor::ImguiProcessor(ImguiManager* manager)
        : m_manager(manager),
          m_input_buffer(256, '\0'),
          m_param_stack(static_cast<int>(UiParam::Count)),
          m_color_stack(static_cast<int>(UiColor::Count)) {
    }

    void ImguiProcessor::process_tree(UiElement* element) {
        push_style();
        process(element);
        pop_style();
    }

    void ImguiProcessor::process(UiElement* element) {
        if (!element) {
            return;
        }

        const UiElementType type    = element->type;
        const bool          need_id = imgui_need_id(type);

        push_sub_style(element->sub_style);

        if (need_id) {
            ImGui::PushID(element);
        }

        switch (type) {
            case UiElementType::MainWindow:
                imgui_process_main_window(*this, *static_cast<UiMainWindow*>(element));
                break;
            case UiElementType::DockWindow:
                imgui_process_dock_window(*this, *static_cast<UiDockWindow*>(element));
                break;
            case UiElementType::DockSpace:
                imgui_process_dock_space(*this, *static_cast<UiDockSpace*>(element));
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
            case UiElementType::CollapsingPanel:
                imgui_process_collapsing_panel(*this, *static_cast<UiCollapsingPanel*>(element));
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

        if (need_id) {
            ImGui::PopID();
        }

        for (const UiBindingUpdater& updater : element->bindings_updater) {
            updater();
        }

        pop_sub_style();
    }

    void ImguiProcessor::process(std::vector<Ref<UiElement>>& elements) {
        for (const auto& child : elements) {
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

    void ImguiProcessor::add_action_event(std::function<void()>& event) {
        if (event) {
            add_action([e = &event]() {
                (*e)();
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

    Color4f ImguiProcessor::get_color(UiColor color) const {
        auto& stack = m_color_stack[static_cast<int>(color)];
        return stack.back();
    }

    void ImguiProcessor::push_style() {
        const UiStyleDesc&    style_desc = m_manager->get_style_desc();
        const UiColorPalette& palette    = style_desc.palette;

        if (!style_desc.fonts.empty()) {
            if (ImFont* font = m_manager->find_font(style_desc.fonts.front().tag)) {
                ImGui::PushFont(font);
            }
        }
        for (const auto& slot : style_desc.colors) {
            auto          query = palette.colors.find(slot.value);
            const Color4f value = query != palette.colors.end() ? query->second : Color::RED4f;
            push_color(slot.type, value);
        }
    }

    void ImguiProcessor::pop_style() {
        const UiStyleDesc& style_desc = m_manager->get_style_desc();

        if (!style_desc.fonts.empty()) {
            if (m_manager->find_font(style_desc.fonts.front().tag)) {
                ImGui::PopFont();
            }
        }
        for (const auto& slot : style_desc.colors) {
            pop_color(slot.type);
        }
    }

    void ImguiProcessor::push_sub_style(Strid sub_style) {
        const UiStyleDesc& style_desc = m_manager->get_style_desc();

        auto query = style_desc.sub_styles.find(sub_style);
        if (query == style_desc.sub_styles.end()) {
            m_style_stack.push_back(nullptr);
            return;
        }

        const UiSubStyle*     s       = &query->second;
        const UiColorPalette& palette = style_desc.palette;

        if (!s->font.empty()) {
            if (ImFont* font = m_manager->find_font(s->font)) {
                ImGui::PushFont(font);
            }
        }
        for (const auto& slot : s->params) {
            push_param(slot.type, slot.value);
        }
        for (const auto& slot : s->colors) {
            auto          query = palette.colors.find(slot.value);
            const Color4f value = query != palette.colors.end() ? query->second : Color4f();
            push_color(slot.type, value);
        }

        m_style_stack.push_back(s);
    }

    void ImguiProcessor::pop_sub_style() {
        const UiSubStyle* s = m_style_stack.back();
        if (s == nullptr) {
            m_style_stack.pop_back();
            return;
        }

        const UiStyleDesc& style_desc = m_manager->get_style_desc();

        if (!s->font.empty()) {
            if (m_manager->find_font(s->font)) {
                ImGui::PopFont();
            }
        }
        for (const auto& slot : s->params) {
            pop_param(slot.type);
        }
        for (const auto& slot : s->colors) {
            pop_color(slot.type);
        }

        m_style_stack.pop_back();
    }

    void ImguiProcessor::push_param(UiParam param, float value) {
        auto& stack = m_param_stack[static_cast<int>(param)];
        stack.push_back(value);
        imgui_push_var(param, value);
    }

    void ImguiProcessor::pop_param(UiParam param) {
        auto& stack = m_param_stack[static_cast<int>(param)];
        stack.pop_back();
        ImGui::PopStyleVar();
    }

    void ImguiProcessor::push_color(UiColor color, Color4f value) {
        auto& stack = m_color_stack[static_cast<int>(color)];
        stack.push_back(value);
        ImGui::PushStyleColor(static_cast<ImGuiCol>(color), imgui_color4(value));
    }

    void ImguiProcessor::pop_color(UiColor color) {
        auto& stack = m_color_stack[static_cast<int>(color)];
        stack.pop_back();
        ImGui::PopStyleColor();
    }

}// namespace wmoge