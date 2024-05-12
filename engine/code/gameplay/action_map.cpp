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

#include "action_map.hpp"

#include "core/data.hpp"
#include "core/log.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "platform/file_system.hpp"
#include "platform/input.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    Status yaml_read(IoContext& context, YamlConstNodeRef node, ActionActivation& activation) {
        WG_YAML_READ_AS_OPT(context, node, "device_name", activation.device_name);
        WG_YAML_READ_AS_OPT(context, node, "device_type", activation.device_type);
        WG_YAML_READ_AS_OPT(context, node, "action", activation.action);
        WG_YAML_READ_AS_OPT(context, node, "key", activation.key);
        WG_YAML_READ_AS_OPT(context, node, "mouse_button", activation.mouse_button);
        WG_YAML_READ_AS_OPT(context, node, "joystick", activation.joystick);
        WG_YAML_READ_AS_OPT(context, node, "joystick_button", activation.joystick_button);
        WG_YAML_READ_AS_OPT(context, node, "axis", activation.axis);
        WG_YAML_READ_AS_OPT(context, node, "gamepad_button", activation.gamepad_button);
        WG_YAML_READ_AS_OPT(context, node, "gamepad_axis", activation.gamepad_axis);
        WG_YAML_READ_AS_OPT(context, node, "threshold", activation.threshold);
        WG_YAML_READ_AS_OPT(context, node, "direction", activation.direction);

        return WG_OK;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const ActionActivation& activation) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(context, node, "device_name", activation.device_name);
        WG_YAML_WRITE_AS(context, node, "device_type", activation.device_type);
        WG_YAML_WRITE_AS(context, node, "action", activation.action);
        WG_YAML_WRITE_AS(context, node, "key", activation.key);
        WG_YAML_WRITE_AS(context, node, "mouse_button", activation.mouse_button);
        WG_YAML_WRITE_AS(context, node, "joystick", activation.joystick);
        WG_YAML_WRITE_AS(context, node, "joystick_button", activation.joystick_button);
        WG_YAML_WRITE_AS(context, node, "axis", activation.axis);
        WG_YAML_WRITE_AS(context, node, "gamepad_button", activation.gamepad_button);
        WG_YAML_WRITE_AS(context, node, "gamepad_axis", activation.gamepad_axis);
        WG_YAML_WRITE_AS(context, node, "threshold", activation.threshold);
        WG_YAML_WRITE_AS(context, node, "direction", activation.direction);

        return WG_OK;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, ActionMapAction& action) {
        WG_YAML_READ_AS(context, node, "action", action.name);
        WG_YAML_READ_AS(context, node, "display_name", action.display_name);
        WG_YAML_READ_AS(context, node, "activations", action.activations);

        return WG_OK;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const ActionMapAction& action) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(context, node, "action", action.name);
        WG_YAML_WRITE_AS(context, node, "display_name", action.display_name);
        WG_YAML_WRITE_AS(context, node, "activations", action.activations);

        return WG_OK;
    }

    void ActionMap::rename(Strid new_name) {
        m_name = new_name;
    }

    void ActionMap::add_action_activation(const Strid& action_name, const ActionActivation& activation) {
        if (!has_action(action_name)) {
            WG_LOG_ERROR("no such action " << action_name);
            return;
        }
        ActionMapAction& action = m_actions[action_name];
        action.activations.push_back(activation);
    }

    void ActionMap::remove_action(const Strid& action_name) {
        if (!has_action(action_name)) {
            WG_LOG_ERROR("no such action " << action_name);
            return;
        }
        m_actions.erase(action_name);
    }

    const Strid& ActionMap::get_name() {
        return m_name;
    }
    int ActionMap::get_priority() {
        return m_priority;
    }
    bool ActionMap::has_action(const Strid& action_name) {
        return get_action(action_name) != nullptr;
    }
    void ActionMap::add_action(const Strid& action_name, const Strid& display_name) {
        if (has_action(action_name)) {
            WG_LOG_ERROR("an attempt to overwrite action " << action_name);
            return;
        }
        ActionMapAction& action = m_actions[action_name];
        action.name             = action_name;
        action.display_name     = display_name;
    }
    const ActionMapAction* ActionMap::get_action(const Strid& action_name) {
        auto query = m_actions.find(action_name);
        return query != m_actions.end() ? &query->second : nullptr;
    }
    std::vector<ActionMapAction> ActionMap::get_actions() const {
        std::vector<ActionMapAction> actions;
        actions.reserve(m_actions.size());

        for (const auto& action : m_actions) {
            actions.push_back(action.second);
        }

        return actions;
    }

    Status yaml_read(IoContext& context, YamlConstNodeRef node, ActionMap& action_map) {
        std::vector<ActionMapAction> actions;

        WG_YAML_READ_AS(context, node, "name", action_map.m_name);
        WG_YAML_READ_AS(context, node, "priority", action_map.m_priority);
        WG_YAML_READ_AS(context, node, "actions", actions);

        for (auto& action : actions) {
            action_map.m_actions[action.name] = std::move(action);
        }

        return WG_OK;
    }
    Status yaml_write(IoContext& context, YamlNodeRef node, const ActionMap& action_map) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(context, node, "name", action_map.m_name);
        WG_YAML_WRITE_AS(context, node, "priority", action_map.m_priority);
        WG_YAML_WRITE_AS(context, node, "actions", action_map.get_actions());

        return WG_OK;
    }

}// namespace wmoge