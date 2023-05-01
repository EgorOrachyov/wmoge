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

#ifndef WMOGE_GLFW_INPUT_HPP
#define WMOGE_GLFW_INPUT_HPP

#include <vector>

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"
#include "platform/input.hpp"

#include "glfw_input_defs.hpp"
#include "glfw_input_devices.hpp"

#include <GLFW/glfw3.h>

namespace wmoge {

    /**
     * @class GlfwInput
     * @brief Glfw-based implementation of hardware input
     */
    class GlfwInput final : public Input {
    public:
        GlfwInput(class GlfwWindowManager& manager);
        ~GlfwInput() override = default;

        ref_ptr<Mouse>    mouse() override;
        ref_ptr<Keyboard> keyboard() override;
        ref_ptr<Joystick> joystick(int id) override;
        int               joystick_mapping(const StringId& mapping) override;

    private:
        friend class GlfwWindowManager;

        void                  init_mappings();
        void                  subscribe_window(GLFWwindow* window);
        void                  update();
        void                  check_connected_joysticks();
        ref_ptr<GlfwJoystick> get_joystick(int jid);

    private:
        // Glfw specific
        static void drop_callback(GLFWwindow* window, int count, const char** paths);
        static void mouse_position_callback(GLFWwindow* window, double x, double y);
        static void mouse_buttons_callback(GLFWwindow* window, int button, int action, int mods);
        static void keyboard_keys_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void keyboard_text_callback(GLFWwindow* window, unsigned int code_point);
        static void joystick_callback(int jid, int state);

    private:
        fast_map<StringId, int>            m_joystick_mappings;
        fast_vector<ref_ptr<GlfwJoystick>> m_joysticks;
        ref_ptr<GlfwMouse>                 m_mouse;
        ref_ptr<GlfwKeyboard>              m_keyboard;
        class GlfwWindowManager&           m_manager;
    };

}// namespace wmoge

#endif//WMOGE_GLFW_INPUT_HPP
