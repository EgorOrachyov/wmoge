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

#include "core/buffered_vector.hpp"
#include "core/flat_map.hpp"
#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "event/event_listener.hpp"
#include "io/yaml.hpp"
#include "platform/input_defs.hpp"
#include "platform/input_devices.hpp"

#include <vector>

namespace wmoge {

    /**
     * @class ActionActivation
     * @brief Single activation info which can be used to trigger an action
     */
    struct ActionActivation {
        Strid              device_name     = SID("unknown");
        InputDeviceType    device_type     = InputDeviceType::Any;
        InputAction        action          = InputAction::Unknown;
        InputKeyboardKey   key             = InputKeyboardKey::Unknown;
        InputMouseButton   mouse_button    = InputMouseButton::Unknown;
        int                joystick        = -1;
        int                joystick_button = -1;
        int                axis            = -1;
        InputGamepadButton gamepad_button  = InputGamepadButton::Unknown;
        InputGamepadAxis   gamepad_axis    = InputGamepadAxis::Unknown;
        float              threshold       = 0.1f;
        float              direction       = 0.0f;

        friend Status yaml_read(YamlConstNodeRef node, ActionActivation& activation);
        friend Status yaml_write(YamlNodeRef node, const ActionActivation& activation);
    };

    /**
     * @class ActionMapAction
     * @brief Single action which can be stored inside an action map and triggered by an activation info
     */
    struct ActionMapAction {
        Strid                             name         = SID("<unknown>");
        Strid                             display_name = SID("<empty action>");
        buffered_vector<ActionActivation> activations;

        friend Status yaml_read(YamlConstNodeRef node, ActionMapAction& action);
        friend Status yaml_write(YamlNodeRef node, const ActionMapAction& action);
    };

    /**
     * @class ActionMap
     * @brief Abstracts user input by actions table with input remapping
     */
    class ActionMap final : public RefCnt {
    public:
        ~ActionMap() override = default;

        void rename(Strid new_name);
        void add_action(const Strid& action_name, const Strid& display_name);
        void add_action_activation(const Strid& action_name, const ActionActivation& activation);
        void remove_action(const Strid& action_name);

        [[nodiscard]] const Strid&                 get_name();
        [[nodiscard]] int                          get_priority();
        [[nodiscard]] bool                         has_action(const Strid& action_name);
        [[nodiscard]] const ActionMapAction*       get_action(const Strid& action_name);
        [[nodiscard]] std::vector<ActionMapAction> get_actions() const;

        friend Status yaml_read(YamlConstNodeRef node, ActionMap& action_map);
        friend Status yaml_write(YamlNodeRef node, const ActionMap& action_map);

    private:
        friend class ActionManager;

        flat_map<Strid, ActionMapAction> m_actions;
        Strid                            m_name;
        int                              m_priority = 0;
    };

}// namespace wmoge

#endif//WMOGE_ACTION_MAP_HPP
