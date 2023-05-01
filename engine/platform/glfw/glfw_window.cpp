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

#include "glfw_window.hpp"

#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "platform/glfw/glfw_window_manager.hpp"

namespace wmoge {

    GlfwWindow::GlfwWindow(const WindowInfo& window_info, class GlfwWindowManager& manager) : m_manager(manager) {
        WG_AUTO_PROFILE_GLFW();

        m_id  = window_info.id;
        m_hnd = glfwCreateWindow(window_info.width, window_info.height, window_info.title.c_str(), nullptr, nullptr);

        if (!m_hnd) {
            WG_LOG_ERROR("failed create window id=" << id());
            return;
        }

        if (window_info.icons[0] && window_info.icons[0]->is_not_empty() &&
            window_info.icons[1] && window_info.icons[1]->is_not_empty()) {
            GLFWimage glfw_images[2];

            for (int i = 0; i < 2; i++) {
                glfw_images[i].width  = window_info.icons[i]->get_width();
                glfw_images[i].height = window_info.icons[i]->get_height();
                glfw_images[i].pixels = window_info.icons[i]->get_pixel_data()->buffer();
            }

            glfwSetWindowIcon(m_hnd, 2, glfw_images);
        }
    }

    GlfwWindow::~GlfwWindow() {
        WG_AUTO_PROFILE_GLFW();

        std::lock_guard guard(m_manager.mutex());

        if (m_hnd) {
            close();
        }
    }

    void GlfwWindow::close() {
        WG_AUTO_PROFILE_GLFW();

        std::lock_guard guard(m_manager.mutex());

        if (!m_hnd) {
            WG_LOG_WARNING("window id=" << id() << " closed");
            return;
        }
        glfwDestroyWindow(m_hnd);
        m_hnd = nullptr;
    }

    int GlfwWindow::width() const {
        std::lock_guard guard(m_manager.mutex());

        int w;
        glfwGetWindowSize(m_hnd, &w, nullptr);
        return w;
    }

    int GlfwWindow::height() const {
        std::lock_guard guard(m_manager.mutex());

        int h;
        glfwGetWindowSize(m_hnd, nullptr, &h);
        return h;
    }

    int GlfwWindow::fbo_width() const {
        std::lock_guard guard(m_manager.mutex());

        int w;
        glfwGetFramebufferSize(m_hnd, &w, nullptr);
        return w;
    }

    int GlfwWindow::fbo_height() const {
        std::lock_guard guard(m_manager.mutex());

        int h;
        glfwGetFramebufferSize(m_hnd, nullptr, &h);
        return h;
    }

    float GlfwWindow::scale_x() const {
        std::lock_guard guard(m_manager.mutex());

        float scale;
        glfwGetWindowContentScale(m_hnd, &scale, nullptr);
        return scale;
    }

    float GlfwWindow::scale_y() const {
        std::lock_guard guard(m_manager.mutex());

        float scale;
        glfwGetWindowContentScale(m_hnd, nullptr, &scale);
        return scale;
    }

    bool GlfwWindow::in_focus() const {
        std::lock_guard guard(m_manager.mutex());

        return glfwGetWindowAttrib(m_hnd, GLFW_FOCUSED);
    }

    const StringId& GlfwWindow::id() const {
        return m_id;
    }

    const std::string& GlfwWindow::title() const {
        return m_title;
    }

}// namespace wmoge
