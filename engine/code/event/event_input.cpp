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

#include "event_input.hpp"

#include "core/class.hpp"

#include <sstream>

namespace wmoge {

    std::string EventInput::to_string() {
        return "input event 0x" + std::to_string(reinterpret_cast<std::uint64_t>(this));
    }
    void EventInput::register_class() {
        auto* cls = Class::register_class<EventInput>();
    }

    std::string EventMouse::to_string() {
        std::stringstream ss;
        ss << type_static()
           << ": mouse=" << mouse->name()
           << " pos=" << position
           << " delta=" << delta
           << " mods=" << modifiers
           << " action=" << static_cast<int>(action)
           << " button=" << static_cast<int>(button);
        return ss.str();
    }
    void EventMouse::register_class() {
        auto* cls = Class::register_class<EventMouse>();
    }

    std::string EventKeyboard::to_string() {
        std::stringstream ss;
        ss << type_static()
           << " keyboard=" << keyboard->name()
           << " mods=" << modifiers
           << " action=" << static_cast<int>(action)
           << " key=" << static_cast<int>(key)
           << " text=" << text;
        return ss.str();
    }
    void EventKeyboard::register_class() {
        auto* cls = Class::register_class<EventKeyboard>();
    }

    std::string EventJoystick::to_string() {
        std::stringstream ss;
        ss << type_static()
           << " : joystick=" << joystick->name()
           << " action=" << static_cast<int>(action)
           << " button=" << button;
        return ss.str();
    }
    void EventJoystick::register_class() {
        auto* cls = Class::register_class<EventJoystick>();
    }

    std::string EventGamepad::to_string() {
        std::stringstream ss;
        ss << type_static()
           << " : joystick=" << joystick->name()
           << " action=" << static_cast<int>(action)
           << " button=" << button;
        return ss.str();
    }
    void EventGamepad::register_class() {
        auto* cls = Class::register_class<EventGamepad>();
    }

    std::string EventDrop::to_string() {
        std::stringstream ss;
        ss << type_static()
           << " : paths_count=" << paths.size();
        for (const auto& path : paths)
            ss << " " << path << ";";
        return ss.str();
    }
    void EventDrop::register_class() {
        auto* cls = Class::register_class<EventDrop>();
    }

}// namespace wmoge
