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

#ifndef WMOGE_INPUT_DEVICES_HPP
#define WMOGE_INPUT_DEVICES_HPP

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "math/vec.hpp"
#include "platform/input_defs.hpp"

#include <vector>

namespace wmoge {

    class InputDevice : public RefCnt {
    public:
        ~InputDevice() override = default;
        virtual const StringId&  name() const { return m_mame; }
        virtual InputDeviceState state() const { return m_state; }
        virtual InputDeviceType  device_type() const { return InputDeviceType::Any; }

    protected:
        StringId         m_mame;
        InputDeviceState m_state;
    };

    class Mouse : public InputDevice {
    public:
        static const int MAX_BUTTONS = 2;

        ~Mouse() override = default;
        const Point2f&                  position() const { return m_position; }
        const Size2f&                   delta() const { return m_delta; }
        const std::vector<InputAction>& button_states() const { return m_buttons; }
        InputDeviceType                 device_type() const override { return InputDeviceType::Mouse; }

    protected:
        std::vector<InputAction> m_buttons;
        Point2f                  m_position{};
        Size2f                   m_delta{};
    };

    class Keyboard : public InputDevice {
    public:
        static const int MAX_KEYS = 100;

        ~Keyboard() override = default;
        const std::vector<InputAction>& keys_states() const { return m_keys; }
        InputDeviceType                 device_type() const override { return InputDeviceType::Keyboard; }

    protected:
        std::vector<InputAction> m_keys;
    };

    class Joystick : public InputDevice {
    public:
        ~Joystick() override = default;
        const std::vector<float>&       axes_states() const { return m_axes; }
        const std::vector<InputAction>& buttons_states() const { return m_buttons; }
        const std::vector<float>&       gamepad_axes_states() const { return m_gamepad_axes; }
        const std::vector<InputAction>& gamepad_buttons_states() const { return m_gamepad_buttons; }
        const StringId&                 gamepad_name() const { return m_gamepad_name; }
        const StringId&                 guid() const { return m_guid; }
        int                             id() const { return m_id; }
        bool                            is_gamepad() const { return m_is_gamepad; }
        InputDeviceType                 device_type() const override { return InputDeviceType::Joystick; }

    protected:
        std::vector<InputAction> m_buttons;
        std::vector<float>       m_axes;
        std::vector<InputAction> m_gamepad_buttons;
        std::vector<float>       m_gamepad_axes;
        StringId                 m_gamepad_name;
        StringId                 m_guid;
        int                      m_id         = -1;
        bool                     m_is_gamepad = false;
    };

}// namespace wmoge

#endif//WMOGE_INPUT_DEVICES_HPP
