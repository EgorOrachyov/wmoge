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

#include "glfw_input_devices.hpp"
#include "glfw_input_defs.hpp"

#include "core/engine.hpp"
#include "event/event_input.hpp"
#include "event/event_manager.hpp"

#include <algorithm>
#include <cassert>

namespace wmoge {

    GlfwMouse::GlfwMouse() {
        m_mame  = SID("GlfwMouse");
        m_state = InputDeviceState::Connected;
        m_buttons.resize(MAX_BUTTONS, InputAction::Unknown);
    }

    void GlfwMouse::update_position(const Point2f& pos) {
        m_delta    = pos - m_position;
        m_position = pos;
    }

    void GlfwMouse::update_button(InputMouseButton button, InputAction action) {
        m_buttons[static_cast<int>(button)] = action;
    }

    GlfwKeyboard::GlfwKeyboard() {
        m_mame  = SID("GlfwKeyboard");
        m_state = InputDeviceState::Connected;
        m_keys.resize(MAX_KEYS, InputAction::Unknown);
    }

    void GlfwKeyboard::update_key(InputKeyboardKey key, InputAction action) {
        m_keys[static_cast<int>(key)] = action;
    }

    GlfwJoystick::GlfwJoystick(int HND) {
        m_hnd   = HND;
        m_state = InputDeviceState::Connected;
        m_mame  = SID(glfwGetJoystickName(m_hnd));
        m_guid  = SID(glfwGetJoystickGUID(m_hnd));
        m_id    = HND;

        int axes_count;
        int buttons_count;

        glfwGetJoystickAxes(m_hnd, &axes_count);
        glfwGetJoystickButtons(m_hnd, &buttons_count);

        m_axes.resize(axes_count, 0.0f);
        m_buttons.resize(buttons_count, InputAction::Release);
    }

    void GlfwJoystick::update() {
        auto engine        = Engine::instance();
        auto event_manager = engine->event_manager();

        int          axes_count;
        const float* p_axes = glfwGetJoystickAxes(m_hnd, &axes_count);
        assert(axes_count <= m_axes.size());

        for (int i = 0; i < axes_count; i++) {
            m_axes[i] = p_axes[i];
        }

        int                  buttons_count;
        const unsigned char* pButtons = glfwGetJoystickButtons(m_hnd, &buttons_count);
        assert(buttons_count <= m_buttons.size());

        for (int i = 0; i < buttons_count; i++) {
            auto action = GlfwInputDefs::action(pButtons[i]);

            // Something changed, dispatch event
            if (m_buttons[i] != action) {
                auto button_update      = make_event<EventJoystick>();
                button_update->joystick = ref_ptr<Joystick>(this);
                button_update->action   = action;
                button_update->button   = i;
                event_manager->dispatch(button_update);
                m_buttons[i] = action;
            }

            // Handle press and held case
            if (action == InputAction::Press || action == InputAction::Repeat) {
                auto button_held      = make_event<EventJoystick>();
                button_held->joystick = ref_ptr<Joystick>(this);
                button_held->action   = InputAction::PressHeld;
                button_held->button   = i;
                event_manager->dispatch(button_held);
            }
        }
    }

    void GlfwJoystick::update_state(InputDeviceState state) {
        m_state = state;

        if (state == InputDeviceState::Connected) {
            std::fill(m_buttons.begin(), m_buttons.end(), InputAction::Release);
        }
    }

}// namespace wmoge