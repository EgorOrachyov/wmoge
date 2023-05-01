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

#include "core/engine.hpp"
#include "core/string_utf.hpp"
#include "debug/profiler.hpp"
#include "event/event_input.hpp"
#include "event/event_manager.hpp"
#include "platform/glfw/glfw_window_manager.hpp"

#include <algorithm>

namespace wmoge {

    GlfwInput::GlfwInput(class GlfwWindowManager& manager) : m_manager(manager) {
        WG_AUTO_PROFILE_GLFW();

        m_mouse    = make_ref<GlfwMouse>();
        m_keyboard = make_ref<GlfwKeyboard>();
        init_mappings();
        check_connected_joysticks();
    }

    ref_ptr<Mouse> GlfwInput::mouse() {
        std::lock_guard guard(m_manager.mutex());

        return m_mouse;
    }
    ref_ptr<Keyboard> GlfwInput::keyboard() {
        std::lock_guard guard(m_manager.mutex());

        return m_keyboard;
    }
    ref_ptr<Joystick> GlfwInput::joystick(int id) {
        std::lock_guard guard(m_manager.mutex());

        return id < m_joysticks.size() ? m_joysticks[id] : nullptr;
    }
    int GlfwInput::joystick_mapping(const StringId& mapping) {
        std::lock_guard guard(m_manager.mutex());

        auto query = m_joystick_mappings.find(mapping);
        return query != m_joystick_mappings.end() ? query->second : -1;
    }

    void GlfwInput::init_mappings() {
        m_joystick_mappings[SID("ps4:square")]    = 0;
        m_joystick_mappings[SID("ps4:cross")]     = 1;
        m_joystick_mappings[SID("ps4:circle")]    = 2;
        m_joystick_mappings[SID("ps4:triangle")]  = 3;
        m_joystick_mappings[SID("ps4:l1")]        = 4;
        m_joystick_mappings[SID("ps4:r1")]        = 5;
        m_joystick_mappings[SID("ps4:l2")]        = 6;
        m_joystick_mappings[SID("ps4:r2")]        = 7;
        m_joystick_mappings[SID("ps4:share")]     = 8;
        m_joystick_mappings[SID("ps4:options")]   = 9;
        m_joystick_mappings[SID("ps4:l3")]        = 10;
        m_joystick_mappings[SID("ps4:r3")]        = 11;
        m_joystick_mappings[SID("ps4:ps")]        = 12;
        m_joystick_mappings[SID("ps4:touchpad")]  = 13;
        m_joystick_mappings[SID("ps4:up")]        = 14;
        m_joystick_mappings[SID("ps4:right")]     = 15;
        m_joystick_mappings[SID("ps4:down")]      = 16;
        m_joystick_mappings[SID("ps4:left")]      = 17;
        m_joystick_mappings[SID("ps4:axis:l3:h")] = 0;
        m_joystick_mappings[SID("ps4:axis:l3:v")] = 1;
        m_joystick_mappings[SID("ps4:axis:r3:h")] = 2;
        m_joystick_mappings[SID("ps4:axis:r3:v")] = 3;
        m_joystick_mappings[SID("ps4:axis:l2")]   = 4;
        m_joystick_mappings[SID("ps4:axis:r2")]   = 5;
    }

    void GlfwInput::subscribe_window(GLFWwindow* window) {
        WG_AUTO_PROFILE_GLFW();

        std::lock_guard guard(m_manager.mutex());

        glfwSetDropCallback(window, drop_callback);
        glfwSetCursorPosCallback(window, mouse_position_callback);
        glfwSetMouseButtonCallback(window, mouse_buttons_callback);
        glfwSetKeyCallback(window, keyboard_keys_callback);
        glfwSetCharCallback(window, keyboard_text_callback);
        glfwSetJoystickCallback(joystick_callback);
    }

    void GlfwInput::update() {
        WG_AUTO_PROFILE_GLFW();

        std::lock_guard guard(m_manager.mutex());

        for (auto& joystick : m_joysticks) {
            if (joystick->state() == InputDeviceState::Connected) {
                joystick->update();
            }
        }

        // Handle press and held case
        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();

        const auto& keys = m_keyboard->keys_states();
        for (int i = 0; i < static_cast<int>(keys.size()); ++i) {
            if (keys[i] == InputAction::Press || keys[i] == InputAction::Repeat) {
                auto event      = make_ref<EventKeyboard>();
                event->keyboard = m_keyboard;
                event->key      = static_cast<InputKeyboardKey>(i);
                event->action   = InputAction::PressHeld;
                event_manager->dispatch(event);
            }
        }

        const auto& buttons = m_mouse->button_states();
        for (int i = 0; i < static_cast<int>(buttons.size()); ++i) {
            if (buttons[i] == InputAction::Press || buttons[i] == InputAction::Repeat) {
                auto event      = make_ref<EventMouse>();
                event->mouse    = m_mouse;
                event->button   = static_cast<InputMouseButton>(i);
                event->position = m_mouse->position();
                event->delta    = m_mouse->delta();
                event->action   = InputAction::PressHeld;
                event_manager->dispatch(event);
            }
        }
    }

    void GlfwInput::check_connected_joysticks() {
        std::lock_guard guard(m_manager.mutex());

        for (int jid = 0; jid < GLFW_JOYSTICK_LAST; jid++) {
            if (glfwJoystickPresent(jid)) {
                m_joysticks.push_back(make_ref<GlfwJoystick>(jid));
            }
        }
    }

    ref_ptr<GlfwJoystick> GlfwInput::get_joystick(int jid) {
        std::lock_guard guard(m_manager.mutex());

        auto query = std::find_if(m_joysticks.begin(), m_joysticks.end(), [=](ref_ptr<GlfwJoystick>& joystick) {
            return joystick->hnd() == jid;
        });

        return query == m_joysticks.end() ? ref_ptr<GlfwJoystick>() : *query;
    }

    void GlfwInput::drop_callback(GLFWwindow*, int count, const char** paths) {
        WG_AUTO_PROFILE_GLFW();

        if (count > 0) {
            std::vector<std::string> paths_vector;
            paths_vector.reserve(count);

            for (int i = 0; i < count; i++) {
                paths_vector.emplace_back(paths[i]);
            }

            auto engine        = Engine::instance();
            auto event_manager = engine->event_manager();
            auto event         = make_ref<EventDrop>();
            event->paths       = std::move(paths_vector);
            event_manager->dispatch(event);
        }
    }

    void GlfwInput::mouse_position_callback(GLFWwindow*, double x, double y) {
        WG_AUTO_PROFILE_GLFW();

        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();
        auto input         = dynamic_cast<GlfwInput*>(engine->input());
        auto mouse         = input->m_mouse;

        Point2f position(static_cast<float>(x), static_cast<float>(y));
        mouse->update_position(position);
        auto event      = make_ref<EventMouse>();
        event->mouse    = mouse;
        event->action   = InputAction::Move;
        event->position = mouse->position();
        event->delta    = mouse->delta();
        event_manager->dispatch(event);
    }

    void GlfwInput::mouse_buttons_callback(GLFWwindow*, int button, int action, int mods) {
        WG_AUTO_PROFILE_GLFW();

        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();
        auto input         = dynamic_cast<GlfwInput*>(engine->input());
        auto mouse         = input->m_mouse;

        auto mask         = GlfwInputDefs::mods_mask(mods);
        auto mouse_button = GlfwInputDefs::mouse_button(button);
        auto mouse_action = GlfwInputDefs::action(action);

        if (mouse_button != InputMouseButton::Unknown && mouse_action != InputAction::Unknown) {
            mouse->update_button(mouse_button, mouse_action);
            auto event       = make_ref<EventMouse>();
            event->mouse     = mouse;
            event->action    = mouse_action;
            event->button    = mouse_button;
            event->modifiers = mask;
            event_manager->dispatch(event);
        }
    }

    void GlfwInput::keyboard_keys_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
        WG_AUTO_PROFILE_GLFW();

        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();
        auto input         = dynamic_cast<GlfwInput*>(engine->input());
        auto keyboard      = input->m_keyboard;

        auto mask            = GlfwInputDefs::mods_mask(mods);
        auto keyboard_key    = GlfwInputDefs::keyboard_key(key);
        auto keyboard_action = GlfwInputDefs::action(action);

        if (keyboard_key != InputKeyboardKey::Unknown && keyboard_action != InputAction::Unknown) {
            keyboard->update_key(keyboard_key, keyboard_action);
            auto event       = make_ref<EventKeyboard>();
            event->keyboard  = keyboard;
            event->action    = keyboard_action;
            event->key       = keyboard_key;
            event->modifiers = mask;
            event_manager->dispatch(event);
        }
    }

    void GlfwInput::keyboard_text_callback(GLFWwindow*, unsigned int code_point) {
        WG_AUTO_PROFILE_GLFW();

        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();
        auto input         = dynamic_cast<GlfwInput*>(engine->input());
        auto keyboard      = input->m_keyboard;

        char text[8];
        int  length = 0;
        StringUtf::utf32_to_utf8_chr(code_point, text, length);

        auto event      = make_ref<EventKeyboard>();
        event->keyboard = keyboard;
        event->action   = InputAction::Text;
        event->text     = std::string(text, length);
        event_manager->dispatch(event);
    }

    void GlfwInput::joystick_callback(int jid, int state) {
        WG_AUTO_PROFILE_GLFW();

        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();
        auto input         = dynamic_cast<GlfwInput*>(engine->input());

        auto joystick    = input->get_joystick(jid);
        auto deviceState = state == GLFW_CONNECTED ? InputDeviceState::Connected : InputDeviceState::Disconnected;

        // If found (previously was connected, update state)
        if (joystick) {
            joystick->update_state(deviceState);
        }

        // If connected and not found, add new one
        if (!joystick && deviceState == InputDeviceState::Connected) {
            joystick = make_ref<GlfwJoystick>(jid);
            input->m_joysticks.push_back(joystick);
        }

        if (joystick) {
            auto event      = make_ref<EventJoystick>();
            event->joystick = joystick;
            event->action   = InputAction::State;
            event_manager->dispatch(event);
        }
    }

}// namespace wmoge