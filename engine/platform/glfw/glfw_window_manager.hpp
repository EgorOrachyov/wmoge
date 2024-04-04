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

#pragma once

// include first
#include <volk.h>

// include after vk symbols are available
#include <GLFW/glfw3.h>

#include "core/fast_map.hpp"
#include "event/event_window.hpp"
#include "platform/glfw/glfw_input.hpp"
#include "platform/glfw/glfw_window.hpp"
#include "platform/window_manager.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace wmoge {

    /**
     * @class GlfwWindowManager
     * @brief Glfw window and input manager
     */
    class GlfwWindowManager final : public WindowManager {
    public:
        explicit GlfwWindowManager(bool vsync, bool client_api);
        ~GlfwWindowManager() override;

        void                     poll_events() override;
        fast_vector<Ref<Window>> windows() override;
        Ref<Window>              primary_window() override;
        Ref<Window>              create(const WindowInfo& window_info) override;
        Ref<Window>              get(const Strid& window_id) override;

        std::shared_ptr<GlfwInput> input();
        std::recursive_mutex&      mutex();

        Ref<GlfwWindow>                                                 get(GLFWwindow* hnd);
        std::vector<std::string>                                        extensions();
        std::function<VkResult(VkInstance, Ref<Window>, VkSurfaceKHR&)> factory();

    private:
        // Glfw Specifics
        static void window_close_callback(GLFWwindow* hnd);
        static void window_resized_callback(GLFWwindow* hnd, int width, int height);
        static void window_content_scale_callback(GLFWwindow* hnd, float x_scale, float y_scale);
        static void framebuffer_size_callback(GLFWwindow* hnd, int width, int height);
        static void iconify_callback(GLFWwindow* hnd, int iconify);
        static void maximize_callback(GLFWwindow* hnd, int maximize);
        static void focus_callback(GLFWwindow* hnd, int focus);
        static void error_callback(int error_code, const char* description);
        static void dispatch(GLFWwindow* hnd, WindowNotification notification);

    private:
        fast_map<Strid, Ref<GlfwWindow>>       m_windows;
        fast_map<GLFWwindow*, Ref<GlfwWindow>> m_windows_by_hnd;
        std::shared_ptr<GlfwInput>             m_input;
        Ref<GlfwWindow>                        m_primary;
        bool                                   m_vsync;
        bool                                   m_client_api;

        mutable std::recursive_mutex m_mutex;
    };

}// namespace wmoge