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

#include "math/vec.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class InputEventMouse
     * @brief Mouse input
     */
    struct InputEventMouse {
        Ref<Mouse>       mouse;
        Point2f          position;
        Size2f           delta;
        InputModifiers   modifiers;
        InputAction      action;
        InputMouseButton button;
    };

    /**
     * @class InputEventKeyboard
     * @brief Keyboard input
     */
    struct InputEventKeyboard {
        Ref<Keyboard>    keyboard;
        InputModifiers   modifiers;
        InputAction      action;
        InputKeyboardKey key;
        std::string      text;
    };

    /**
     * @class InputEventJoystick
     * @brief Joystick input
     */
    struct InputEventJoystick {
        Ref<Joystick> joystick;
        InputAction   action;
        int           button;
    };

    /**
     * @class InputEventGamepad
     * @brief Gamepad mapped input
     */
    struct InputEventGamepad {
        Ref<Joystick> joystick;
        InputAction   action;
        int           button;
    };

    /**
     * @class InputEventDrop
     * @brief Drop input for desktop
     */
    struct InputEventDrop {
        std::vector<std::string> paths;
    };

    /**
     * @class Input
     * @brief Interface for OS-specific input layer to get user input
     */
    class Input {
    public:
        virtual ~Input()                                                     = default;
        virtual Ref<Mouse>                             get_mouse()           = 0;
        virtual Ref<Keyboard>                          get_keyboard()        = 0;
        virtual Ref<Joystick>                          get_joystick(int id)  = 0;
        virtual const std::vector<InputEventMouse>&    get_events_mouse()    = 0;
        virtual const std::vector<InputEventKeyboard>& get_events_keyboard() = 0;
        virtual const std::vector<InputEventJoystick>& get_events_joystick() = 0;
        virtual const std::vector<InputEventGamepad>&  get_events_gamepad()  = 0;
        virtual const std::vector<InputEventDrop>&     get_events_drop()     = 0;
    };

}// namespace wmoge