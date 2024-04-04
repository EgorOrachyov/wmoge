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

#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"

namespace wmoge {

    class GlfwMouse final : public Mouse {
    public:
        GlfwMouse();
        ~GlfwMouse() override = default;

        void update_position(const Point2f& pos);
        void update_button(InputMouseButton button, InputAction action);
    };

    class GlfwKeyboard final : public Keyboard {
    public:
        GlfwKeyboard();
        ~GlfwKeyboard() override = default;

        void update_key(InputKeyboardKey key, InputAction action);
    };

    class GlfwJoystick final : public Joystick {
    public:
        explicit GlfwJoystick(int HND);
        ~GlfwJoystick() override = default;

        void update();
        void update_state(InputDeviceState state);
        int  hnd() const { return m_hnd; }

    private:
        int m_hnd = -1;
    };

}// namespace wmoge