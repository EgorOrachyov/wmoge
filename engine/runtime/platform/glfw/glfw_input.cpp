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

#include "glfw_input.hpp"

#include "core/string_utf.hpp"
#include "platform/glfw/glfw_window_manager.hpp"
#include "profiler/profiler.hpp"

#include <algorithm>

namespace wmoge {

    static GlfwInput* g_glfw_input = nullptr;

    GlfwInput::GlfwInput(class GlfwWindowManager& manager) : m_manager(manager) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::GlfwInput");

        g_glfw_input = this;
        m_mouse      = make_ref<GlfwMouse>();
        m_keyboard   = make_ref<GlfwKeyboard>();

        check_connected_joysticks();
    }

    Ref<Mouse> GlfwInput::get_mouse() {
        return m_mouse;
    }
    Ref<Keyboard> GlfwInput::get_keyboard() {
        return m_keyboard;
    }
    Ref<Joystick> GlfwInput::get_joystick(int id) {
        return id < m_joysticks.size() ? m_joysticks[id] : nullptr;
    }
    const std::vector<InputEventMouse>& GlfwInput::get_events_mouse() {
        return m_events_mouse;
    }
    const std::vector<InputEventKeyboard>& GlfwInput::get_events_keyboard() {
        return m_events_keyboard;
    }
    const std::vector<InputEventJoystick>& GlfwInput::get_events_joystick() {
        return m_events_joystick;
    }
    const std::vector<InputEventGamepad>& GlfwInput::get_events_gamepad() {
        return m_events_gamepad;
    }
    const std::vector<InputEventDrop>& GlfwInput::get_events_drop() {
        return m_events_drop;
    }

    void GlfwInput::subscribe_window(GLFWwindow* window) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::subscribe_window");

        glfwSetDropCallback(window, drop_callback);
        glfwSetCursorPosCallback(window, mouse_position_callback);
        glfwSetMouseButtonCallback(window, mouse_buttons_callback);
        glfwSetKeyCallback(window, keyboard_keys_callback);
        glfwSetCharCallback(window, keyboard_text_callback);
        glfwSetJoystickCallback(joystick_callback);
    }

    void GlfwInput::update() {
        WG_AUTO_PROFILE_GLFW("GlfwInput::update");

        for (auto& joystick : m_joysticks) {
            if (joystick->state() == InputDeviceState::Connected) {
                joystick->update();
            }
        }
    }

    void GlfwInput::check_connected_joysticks() {
        for (int jid = 0; jid < GLFW_JOYSTICK_LAST; jid++) {
            if (glfwJoystickPresent(jid)) {
                m_joysticks.push_back(make_ref<GlfwJoystick>(jid));
            }
        }
    }

    Ref<GlfwJoystick> GlfwInput::get_joystick_by_hnd(int jid) {
        auto query = std::find_if(m_joysticks.begin(), m_joysticks.end(), [=](Ref<GlfwJoystick>& joystick) {
            return joystick->hnd() == jid;
        });

        return query == m_joysticks.end() ? Ref<GlfwJoystick>() : *query;
    }

    void GlfwInput::clear_events() {
        m_events_mouse.clear();
        m_events_keyboard.clear();
        m_events_joystick.clear();
        m_events_gamepad.clear();
        m_events_drop.clear();
    }

    void GlfwInput::drop_callback(GLFWwindow*, int count, const char** paths) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::drop_callback");

        if (count > 0) {
            std::vector<std::string> paths_vector;
            paths_vector.reserve(count);

            for (int i = 0; i < count; i++) {
                paths_vector.emplace_back(paths[i]);
            }

            InputEventDrop& event = g_glfw_input->m_events_drop.emplace_back();
            event.paths           = std::move(paths_vector);
        }
    }

    void GlfwInput::mouse_position_callback(GLFWwindow*, double x, double y) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::mouse_position_callback");

        auto mouse = g_glfw_input->m_mouse;

        Point2f position(static_cast<float>(x), static_cast<float>(y));
        mouse->update_position(position);

        InputEventMouse& event = g_glfw_input->m_events_mouse.emplace_back();
        event.mouse            = mouse;
        event.action           = InputAction::Move;
        event.position         = mouse->position();
        event.delta            = mouse->delta();
    }

    void GlfwInput::mouse_buttons_callback(GLFWwindow*, int button, int action, int mods) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::mouse_buttons_callback");

        auto mouse = g_glfw_input->m_mouse;

        auto mask         = GlfwInputDefs::mods_mask(mods);
        auto mouse_button = GlfwInputDefs::mouse_button(button);
        auto mouse_action = GlfwInputDefs::action(action);

        if (mouse_button != InputMouseButton::Unknown && mouse_action != InputAction::Unknown) {
            mouse->update_button(mouse_button, mouse_action);

            InputEventMouse& event = g_glfw_input->m_events_mouse.emplace_back();
            event.mouse            = mouse;
            event.action           = mouse_action;
            event.button           = mouse_button;
            event.modifiers        = mask;
        }
    }

    void GlfwInput::keyboard_keys_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::keyboard_keys_callback");

        auto keyboard = g_glfw_input->m_keyboard;

        auto mask            = GlfwInputDefs::mods_mask(mods);
        auto keyboard_key    = GlfwInputDefs::keyboard_key(key);
        auto keyboard_action = GlfwInputDefs::action(action);

        if (keyboard_key != InputKeyboardKey::Unknown && keyboard_action != InputAction::Unknown) {
            keyboard->update_key(keyboard_key, keyboard_action);

            InputEventKeyboard& event = g_glfw_input->m_events_keyboard.emplace_back();
            event.keyboard            = keyboard;
            event.action              = keyboard_action;
            event.key                 = keyboard_key;
            event.modifiers           = mask;
        }
    }

    void GlfwInput::keyboard_text_callback(GLFWwindow*, unsigned int code_point) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::keyboard_text_callback");

        auto keyboard = g_glfw_input->m_keyboard;

        char text[8];
        int  length = 0;
        StringUtf::utf32_to_utf8_chr(code_point, text, length);

        InputEventKeyboard& event = g_glfw_input->m_events_keyboard.emplace_back();
        event.keyboard            = keyboard;
        event.action              = InputAction::Text;
        event.text                = std::string(text, length);
    }

    void GlfwInput::joystick_callback(int jid, int state) {
        WG_AUTO_PROFILE_GLFW("GlfwInput::joystick_callback");

        auto joystick    = g_glfw_input->get_joystick_by_hnd(jid);
        auto deviceState = state == GLFW_CONNECTED ? InputDeviceState::Connected : InputDeviceState::Disconnected;

        // If found (previously was connected, update state)
        if (joystick) {
            joystick->update_state(deviceState);
        }

        // If connected and not found, add new one
        if (!joystick && deviceState == InputDeviceState::Connected) {
            joystick = make_ref<GlfwJoystick>(jid);
            g_glfw_input->m_joysticks.push_back(joystick);
        }

        if (joystick) {
            InputEventJoystick& event = g_glfw_input->m_events_joystick.emplace_back();
            event.joystick            = joystick;
            event.action              = InputAction::State;
        }
    }

}// namespace wmoge