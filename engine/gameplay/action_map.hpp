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

#ifndef WMOGE_ACTION_MAP_HPP
#define WMOGE_ACTION_MAP_HPP

#include "core/fast_map.hpp"
#include "core/fast_vector.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "event/event_listener.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class ActionActivation
     * @brief Single activation info which can be used to trigger an action
     */
    struct ActionActivation {
        StringId         device_name     = SID("unknown");
        InputDeviceType  device_type     = InputDeviceType::Any;
        InputAction      action          = InputAction::Unknown;
        InputKeyboardKey key             = InputKeyboardKey::Unknown;
        InputMouseButton mouse_button    = InputMouseButton::Unknown;
        int              joystick        = -1;
        int              joystick_button = -1;
        int              axis            = -1;
        int              gamepad_button  = -1;
        int              gamepad_axis    = -1;
        float            threshold       = 0.1f;
        float            direction       = 0.0f;
    };

    /**
     * @class ActionMapAction
     * @brief Single action which can be stored inside an action map and triggered by an activation info
     */
    class ActionMapAction {
    public:
        [[nodiscard]] const StringId&                      get_name() const;
        [[nodiscard]] const StringId&                      get_display_name() const;
        [[nodiscard]] const fast_vector<ActionActivation>& get_activations() const;

    private:
        friend class ActionMap;

        StringId                      m_name         = SID("<unknown>");
        StringId                      m_display_name = SID("<empty action>");
        fast_vector<ActionActivation> m_activations;
    };

    /**
     * @class ActionMap
     * @brief Abstracts user input by actions table with input remapping
     */
    class ActionMap final : public RefCnt {
    public:
        ~ActionMap() override = default;

        /**
         * @brief Loads action map from a disk
         *
         * @param filepath Path to action to load
         *
         * @return True if loaded
         */
        bool load(const std::string& filepath);
        void rename(StringId new_name);
        void enable();
        void disable();
        void add_action(const StringId& action_name, const StringId& display_name);
        void add_action_activation(const StringId& action_name, const ActionActivation& activation);
        void remove_action(const StringId& action_name);

        [[nodiscard]] const StringId&        get_name();
        [[nodiscard]] const std::string&     get_path();
        [[nodiscard]] bool                   is_active() const;
        [[nodiscard]] bool                   has_action(const StringId& action_name);
        [[nodiscard]] const ActionMapAction* get_action(const StringId& action_name);

    private:
        friend class ActionManager;

        fast_map<StringId, ActionMapAction> m_actions;
        StringId                            m_name;
        std::string                         m_path;
        bool                                m_is_active = false;
    };

}// namespace wmoge

#endif//WMOGE_ACTION_MAP_HPP
