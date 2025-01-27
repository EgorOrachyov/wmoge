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

#include "imgui_manager.hpp"

#include "imgui.h"
#include "imgui_driver_vulkan.hpp"
#include "imgui_platform_glfw.hpp"

namespace wmoge {

    ImguiManager::ImguiManager(WindowManager* window_manager, GfxDriver* driver) {
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;    // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
        ImGui::StyleColorsDark();

        if (window_manager->get_type() == WindowManagerType::Glfw) {
            m_platform = std::make_unique<ImguiPlatformGlfw>(window_manager->get_primary_window(), driver);
        }
        if (driver->get_gfx_type() == GfxType::Vulkan) {
            m_driver = std::make_unique<ImguiDriverVulkan>(window_manager->get_primary_window(), driver);
        }

        assert(m_platform);
        assert(m_driver);
    }

    ImguiManager::~ImguiManager() {
        m_driver.reset();
        m_platform.reset();
        ImGui::DestroyContext();
    }

    void ImguiManager::update() {
        m_driver->new_frame();
        m_platform->new_frame();
        ImGui::NewFrame();

        // tmp
        if (m_show_demo_window)
            ImGui::ShowDemoWindow(&m_show_demo_window);
    }

    void ImguiManager::render(const GfxCmdListRef& cmd_list) {
        ImGui::Render();
        m_driver->render(cmd_list);
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

}// namespace wmoge