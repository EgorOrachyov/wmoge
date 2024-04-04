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

#include "platform/window.hpp"

#include <GLFW/glfw3.h>

namespace wmoge {

    /**
     * @class GlfwWindow
     * @brief Glfw-based implementation of OS-window object
     */
    class GlfwWindow final : public Window {
    public:
        explicit GlfwWindow(const WindowInfo& window_info, class GlfwWindowManager& manager);
        ~GlfwWindow() override;

        void               close() override;
        int                width() const override;
        int                height() const override;
        Size2i             size() const override;
        int                fbo_width() const override;
        int                fbo_height() const override;
        Size2i             fbo_size() const override;
        float              scale_x() const override;
        float              scale_y() const override;
        bool               in_focus() const override;
        const Strid&       id() const override;
        const std::string& title() const override;

    private:
        friend class GlfwWindowManager;

        Strid                    m_id;
        std::string              m_title;
        GLFWwindow*              m_hnd = nullptr;
        class GlfwWindowManager& m_manager;
    };

}// namespace wmoge