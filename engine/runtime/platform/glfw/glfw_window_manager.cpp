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

#include "glfw_window_manager.hpp"

#include "core/log.hpp"
#include "profiler/profiler_cpu.hpp"

#include <cassert>
#include <cstdlib>

namespace wmoge {

    static GlfwWindowManager* g_glwl_manager = nullptr;

    GlfwWindowManager::GlfwWindowManager(bool vsync, bool client_api) {
        WG_PROFILE_CPU_GLFW("GlfwWindowManager::GlfwWindowManager");

        g_glwl_manager = this;
        m_vsync        = vsync;
        m_client_api   = client_api;

        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) {
            WG_LOG_ERROR("failed initialize glfw");
            std::abort();
        }

        m_input = std::make_shared<GlfwInput>(*this);

#if defined(TARGET_MACOS)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);          // Required on Mac
#elif defined(TARGET_LINUX)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#elif defined(TARGET_WINDOWS)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        if (!m_client_api) {
            // For Vulkan based renderer
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }

        WG_LOG_INFO("init glfw window and input manager");
    }

    GlfwWindowManager::~GlfwWindowManager() {
        WG_PROFILE_CPU_GLFW("GlfwWindowManager::~GlfwWindowManager");

        m_primary.reset();
        m_windows_by_hnd.clear();
        m_windows.clear();

        glfwTerminate();
        WG_LOG_INFO("terminate glfw manager");
    }

    void GlfwWindowManager::poll_events() {
        WG_PROFILE_CPU_GLFW("GlfwWindowManager::poll_events");

        clear_events();
        m_input->clear_events();

        glfwPollEvents();

        m_input->update();
    }

    buffered_vector<Ref<Window>> GlfwWindowManager::get_windows() {
        buffered_vector<Ref<Window>> windows_list;
        for (auto& window : m_windows) {
            windows_list.push_back(window.second.as<Window>());
        }

        return std::move(windows_list);
    }

    Ref<Window> GlfwWindowManager::get_primary_window() {
        return m_primary;
    }

    Ref<Window> GlfwWindowManager::create_window(const WindowInfo& window_info) {
        WG_PROFILE_CPU_GLFW("GlfwWindowManager::create");

        if (get_window(window_info.id)) {
            WG_LOG_ERROR("an attempt to recreate window with the same id=" << window_info.id);
            return nullptr;
        }

        // Create and register
        auto window = make_ref<GlfwWindow>(window_info, *this);

        // Check that is created
        if (!window->m_hnd) {
            return nullptr;
        }

        m_windows.emplace(window->id(), window);
        m_windows_by_hnd.emplace(window->m_hnd, window);

        // Subscribe notification callbacks
        glfwSetWindowCloseCallback(window->m_hnd, window_close_callback);
        glfwSetWindowSizeCallback(window->m_hnd, window_resized_callback);
        glfwSetWindowContentScaleCallback(window->m_hnd, window_content_scale_callback);
        glfwSetFramebufferSizeCallback(window->m_hnd, framebuffer_size_callback);
        glfwSetWindowIconifyCallback(window->m_hnd, iconify_callback);
        glfwSetWindowMaximizeCallback(window->m_hnd, maximize_callback);
        glfwSetWindowFocusCallback(window->m_hnd, focus_callback);

        // If first, make primary
        if (!m_primary) {
            m_primary = window;
        }

        if (m_client_api) {
            // First of all, make context current to enable following GL code in renderer
            glfwMakeContextCurrent(window->m_hnd);

            // Set interval for swap in glfw
            // https://www.glfw.org/docs/3.3/group__context.html#ga6d4e0cdf151b5e579bd67f13202994ed
            int vsync = m_vsync ? 1 : 0;
            glfwSwapInterval(vsync);
        }

        // Subscribe window to the input
        m_input->subscribe_window(window->m_hnd);

        return window;
    }

    Ref<Window> GlfwWindowManager::get_window(const Strid& window_id) {
        auto query = m_windows.find(window_id);
        return query != m_windows.end() ? query->second : nullptr;
    }

    const std::vector<WindowEvent>& GlfwWindowManager::get_window_events() {
        return m_events;
    }

    WindowManagerType GlfwWindowManager::get_type() {
        return WindowManagerType::Glfw;
    }

    void GlfwWindowManager::clear_events() {
        m_events.clear();
    }

    std::shared_ptr<GlfwInput> GlfwWindowManager::input() {
        return m_input;
    }

    std::vector<std::string> GlfwWindowManager::extensions() {
        WG_PROFILE_CPU_GLFW("GlfwWindowManager::extensions");

        const char** glfw_ext;
        uint32_t     glfw_ext_count;
        glfw_ext = glfwGetRequiredInstanceExtensions(&glfw_ext_count);

        std::vector<std::string> ext;
        for (uint32_t i = 0; i < glfw_ext_count; i++) {
            ext.push_back(glfw_ext[i]);
        }

        return ext;
    }

    std::function<VkResult(VkInstance, Ref<Window>, VkSurfaceKHR&)> GlfwWindowManager::factory() {
        auto func = [this](VkInstance instance, Ref<Window> window, VkSurfaceKHR& surface_khr) {
            WG_PROFILE_CPU_GLFW("GlfwWindowManager::glfwCreateWindowSurface");
            assert(instance);
            assert(window);
            auto glfw_window = dynamic_cast<GlfwWindow*>(window.get());
            return glfwCreateWindowSurface(instance, glfw_window->m_hnd, nullptr, &surface_khr);
        };
        return func;
    }

    Ref<GlfwWindow> GlfwWindowManager::get(GLFWwindow* hnd) {
        auto query = m_windows_by_hnd.find(hnd);
        return query != m_windows_by_hnd.end() ? query->second : nullptr;
    }

    void GlfwWindowManager::window_close_callback(GLFWwindow* hnd) {
        dispatch(hnd, WindowNotification::CloseRequested);
    }

    void GlfwWindowManager::window_resized_callback(GLFWwindow* hnd, int, int) {
        dispatch(hnd, WindowNotification::Resized);
    }

    void GlfwWindowManager::window_content_scale_callback(GLFWwindow* hnd, float, float) {
        dispatch(hnd, WindowNotification::ContentScale);
    }

    void GlfwWindowManager::framebuffer_size_callback(GLFWwindow* hnd, int, int) {
        dispatch(hnd, WindowNotification::FramebufferResized);
    }

    void GlfwWindowManager::iconify_callback(GLFWwindow* hnd, int iconify) {
        dispatch(hnd, iconify ? WindowNotification::Minimized : WindowNotification::Restored);
    }

    void GlfwWindowManager::maximize_callback(GLFWwindow* hnd, int maximize) {
        dispatch(hnd, maximize ? WindowNotification::Maximized : WindowNotification::Restored);
    }

    void GlfwWindowManager::focus_callback(GLFWwindow* hnd, int focus) {
        dispatch(hnd, focus ? WindowNotification::FocusReceived : WindowNotification::FocusLost);
    }

    void GlfwWindowManager::error_callback(int error_code, const char* description) {
        WG_LOG_ERROR("error code=" << error_code << " what=" << description);
    }

    void GlfwWindowManager::dispatch(GLFWwindow* hnd, WindowNotification notification) {
        auto window = g_glwl_manager->get(hnd);
        assert(window);

        WindowEvent& event = g_glwl_manager->m_events.emplace_back();
        event.window       = window;
        event.notification = notification;
    }

}// namespace wmoge