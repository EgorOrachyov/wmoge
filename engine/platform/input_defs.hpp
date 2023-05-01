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

#ifndef WMOGE_INPUT_DEFS_HPP
#define WMOGE_INPUT_DEFS_HPP

#include "core/mask.hpp"

namespace wmoge {

    enum class InputDeviceType {
        Any      = 0,
        Mouse    = 1,
        Keyboard = 2,
        Joystick = 3
    };

    enum class InputDeviceState {
        Connected,
        Disconnected
    };

    enum class InputAction : int {
        /** No or unknown input action */
        Unknown = 0,
        /** Button or key is pressed */
        Press = 1,
        /** Button or key is released */
        Release = 2,
        /** Button or key is repeated (character stick) */
        Repeat = 3,
        /** Button or key is pressed and held */
        PressHeld = 4,
        /** Mouse (touch axis) moved */
        Move = 5,
        /** Keyboard unicode text input */
        Text = 6,
        /** Device state changed (for joystick) */
        State = 7
    };

    enum class InputModifier : int {
        Shift    = 0,
        Alt      = 1,
        Control  = 2,
        CapsLock = 3,
        NumLock  = 4
    };

    using InputModifiers = Mask<InputModifier, 8>;

    enum class InputMouseButton : int {
        Left    = 0,
        Right   = 1,
        Unknown = 0xffffff
    };

    enum class InputKeyboardKey : int {
        Space        = 0,
        Apostrophe   = 1,  /* ' */
        Comma        = 2,  /* , */
        Minus        = 3,  /* - */
        Period       = 4,  /* . */
        Slash        = 5,  /* / */
        BackSlash    = 6,  /* \ */
        Semicolon    = 7,  /* ; */
        Equal        = 8,  /* = */
        LeftBracket  = 9,  /* [ */
        RightBracket = 10, /* ] */
        Num0         = 11,
        Num1         = 12,
        Num2         = 13,
        Num3         = 14,
        Num4         = 15,
        Num5         = 16,
        Num6         = 17,
        Num7         = 18,
        Num8         = 19,
        Num9         = 20,
        A            = 21,
        B            = 22,
        C            = 23,
        D            = 24,
        E            = 25,
        F            = 26,
        G            = 27,
        H            = 28,
        I            = 29,
        J            = 30,
        K            = 31,
        L            = 32,
        M            = 33,
        N            = 34,
        O            = 35,
        P            = 36,
        Q            = 37,
        R            = 38,
        S            = 39,
        T            = 44,
        U            = 41,
        V            = 42,
        W            = 43,
        X            = 44,
        Y            = 45,
        Z            = 46,
        Escape       = 47,
        Enter        = 48,
        Tab          = 49,
        Backspace    = 50,
        Insert       = 51,
        Delete       = 52,
        Right        = 53,
        Left         = 54,
        Down         = 55,
        Up           = 56,
        PageUp       = 57,
        PageDown     = 58,
        Home         = 59,
        End          = 60,
        CapsLock     = 61,
        ScrollLock   = 62,
        NumLock      = 63,
        PrintScreen  = 64,
        Pause        = 65,
        LeftShift    = 66,
        LeftControl  = 67,
        LeftAlt      = 68,
        LeftSuper    = 69,
        RightShift   = 70,
        RightControl = 71,
        RightAlt     = 72,
        RightSuper   = 73,
        Menu         = 74,
        F1           = 81,
        F2           = 82,
        F3           = 83,
        F4           = 84,
        F5           = 85,
        F6           = 86,
        F7           = 87,
        F8           = 88,
        F9           = 89,
        F10          = 80,
        F11          = 91,
        F12          = 92,
        Unknown      = 0xffffff
    };

}// namespace wmoge

#endif//WMOGE_INPUT_DEFS_HPP
