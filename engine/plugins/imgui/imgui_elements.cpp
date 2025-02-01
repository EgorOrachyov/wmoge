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

    ImguiMenuAction::ImguiMenuAction(ImguiManager* manager, std::string name, UiOnClick callback)
        : ImguiElementBase<UiMenuAction>(manager) {
        m_name     = std::move(name);
        m_callback = std::move(callback);
    }

    void ImguiMenuAction::process(ImguiProcessContext& context) {
        if (ImGui::MenuItem(m_name.c_str(), "", m_selected, m_enabled)) {
            context.add_action([this]() {
                m_callback();
            });
        }
    }

    ImguiMenuGroup::ImguiMenuGroup(ImguiManager* manager)
        : ImguiElementBase<UiMenuGroup>(manager) {
    }

    void ImguiMenuGroup::process(ImguiProcessContext& context) {
        for (const auto& item : m_items) {
            ImguiElement* imgui_item = dynamic_cast<ImguiElement*>(item.get());
            imgui_item->process(context);
        }
    }

    ImguiMenu::ImguiMenu(ImguiManager* manager, std::string name)
        : ImguiElementBase<UiMenu>(manager) {
        m_name = std::move(name);
    }

    void ImguiMenu::process(ImguiProcessContext& context) {
        if (ImGui::BeginMenu(m_name.c_str(), m_enabled)) {
            const std::size_t num_groups    = m_groups.size();
            const std::size_t current_group = 0;
            for (const auto& group : m_groups) {
                ImguiElement* imgui_group = dynamic_cast<ImguiElement*>(group.get());
                imgui_group->process(context);
                if (current_group + 1 < num_groups) {
                    ImGui::Separator();
                }
            }
            ImGui::EndMenu();
        }
    }

    ImguiMenuBar::ImguiMenuBar(ImguiManager* manager)
        : ImguiElementBase<UiMenuBar>(manager) {
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

    ImguiMainWindow::ImguiMainWindow(ImguiManager* manager, std::string name, Ref<UiMenuBar> menu_bar)
        : ImguiElementBase<UiMainWindow>(manager) {
        m_name     = std::move(name);
        m_menu_bar = std::move(menu_bar);
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
        if (m_no_background) {
            window_flags |= ImGuiWindowFlags_NoBackground;
            dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
        }
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        if (!m_padding) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        }

        ImGui::Begin(m_name.c_str(), &m_open, window_flags);

        if (!m_padding) {
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

        ImGui::End();
    }

    ImguiDockWindow::ImguiDockWindow(ImguiManager* manager, std::string name)
        : ImguiElementBase<UiDockWindow>(manager) {
        m_name = std::move(name);
    }

    void ImguiDockWindow::process(ImguiProcessContext& context) {
    }

}// namespace wmoge