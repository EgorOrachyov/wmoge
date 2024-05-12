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

#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "platform/input.hpp"

#include "glfw_input_defs.hpp"
#include "glfw_input_devices.hpp"

#include <GLFW/glfw3.h>

#include <vector>

namespace wmoge {

    /**
     * @class GlfwInput
     * @brief Glfw-based implementation of hardware input
     */
    class GlfwInput final : public Input {
    public:
        explicit GlfwInput(class GlfwWindowManager& manager);
        ~GlfwInput() override = default;

        Ref<Mouse>    mouse() override;
        Ref<Keyboard> keyboard() override;
        Ref<Joystick> joystick(int id) override;

    private:
        friend class GlfwWindowManager;

        void              subscribe_window(GLFWwindow* window);
        void              update();
        void              check_connected_joysticks();
        Ref<GlfwJoystick> get_joystick(int jid);

    private:
        // Glfw specific
        static void drop_callback(GLFWwindow* window, int count, const char** paths);
        static void mouse_position_callback(GLFWwindow* window, double x, double y);
        static void mouse_buttons_callback(GLFWwindow* window, int button, int action, int mods);
        static void keyboard_keys_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void keyboard_text_callback(GLFWwindow* window, unsigned int code_point);
        static void joystick_callback(int jid, int state);

    private:
        buffered_vector<Ref<GlfwJoystick>> m_joysticks;
        Ref<GlfwMouse>                     m_mouse;
        Ref<GlfwKeyboard>                  m_keyboard;
        class GlfwWindowManager&           m_manager;
    };

}// namespace wmoge