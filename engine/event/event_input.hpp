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

#ifndef WMOGE_EVENT_INPUT_HPP
#define WMOGE_EVENT_INPUT_HPP

#include "event/event.hpp"
#include "platform/input.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"

#include <string>
#include <vector>

namespace wmoge {

    class EventInput : public Event {
    public:
        WG_EVENT(EventInput, Event)
    };

    class EventMouse final : public EventInput {
    public:
        WG_EVENT(EventMouse, EventInput)

        Ref<Mouse>       mouse;
        Point2f          position;
        Size2f           delta;
        InputModifiers   modifiers;
        InputAction      action;
        InputMouseButton button;
    };

    class EventKeyboard final : public EventInput {
    public:
        WG_EVENT(EventKeyboard, EventInput)

        Ref<Keyboard>    keyboard;
        InputModifiers   modifiers;
        InputAction      action;
        InputKeyboardKey key;
        std::string      text;
    };

    class EventJoystick final : public EventInput {
    public:
        WG_EVENT(EventJoystick, EventInput)

        Ref<Joystick> joystick;
        InputAction   action;
        int           button;
    };

    class EventGamepad final : public EventInput {
    public:
        WG_EVENT(EventGamepad, EventInput)

        Ref<Joystick> joystick;
        InputAction   action;
        int           button;
    };

    class EventDrop final : public EventInput {
    public:
        WG_EVENT(EventDrop, EventInput);

        std::vector<std::string> paths;
    };

}// namespace wmoge

#endif//WMOGE_EVENT_INPUT_HPP
