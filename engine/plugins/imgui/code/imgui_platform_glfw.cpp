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

#include "imgui_platform_glfw.hpp"

#include "backends/imgui_impl_glfw.h"
#include "platform/glfw/glfw_window.hpp"

namespace wmoge {

    ImguiPlatformGlfw::ImguiPlatformGlfw(const Ref<Window>& window, GfxDriver* gfx_driver) {
        GlfwWindow* glfw_window = dynamic_cast<GlfwWindow*>(window.get());

        if (gfx_driver->get_gfx_type() == GfxType::Vulkan) {
            ImGui_ImplGlfw_InitForVulkan(glfw_window->handle(), true);
        }
    }

    ImguiPlatformGlfw::~ImguiPlatformGlfw() {
        ImGui_ImplGlfw_Shutdown();
    }

    void ImguiPlatformGlfw::new_frame() {
        ImGui_ImplGlfw_NewFrame();
    }

}// namespace wmoge