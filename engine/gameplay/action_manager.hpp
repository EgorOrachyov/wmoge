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

#ifndef WMOGE_ACTION_MANAGER_HPP
#define WMOGE_ACTION_MANAGER_HPP

#include "core/fast_vector.hpp"
#include "event/event.hpp"
#include "event/event_action.hpp"
#include "event/event_input.hpp"
#include "event/event_listener.hpp"
#include "gameplay/action_map.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class ActionManager
     * @brief Manager for translation raw input into game action events
     */
    class ActionManager final {
    public:
        ActionManager();
        ~ActionManager();

        void update();

        bool load_action_map(const std::string& filepath);
        bool add_action_map(const Ref<ActionMap>& action_map);
        bool remove_action_map(const StringId& name);
        bool has_action_map(const StringId& name);
        void activate_action_map(const StringId& name, bool active = true);

    private:
        struct ActionMapInfo {
            Ref<ActionMap> action_map;
            bool           active   = false;
            int            priority = 0;
        };

        bool           on_input_mouse(const EventMouse& event);
        bool           on_input_keyboard(const EventKeyboard& event);
        bool           on_input_joystick(const EventJoystick& event);
        bool           on_input_gamepad(const EventGamepad& event);
        ActionMapInfo* get_action_map_info(const StringId& name);

    private:
        fast_vector<ActionMapInfo> m_maps;
        Ref<EventListener>         m_listener_mouse;
        Ref<EventListener>         m_listener_keyboard;
        Ref<EventListener>         m_listener_joystick;
        Ref<EventListener>         m_listener_gamepad;
    };

}// namespace wmoge

#endif//WMOGE_ACTION_MANAGER_HPP
