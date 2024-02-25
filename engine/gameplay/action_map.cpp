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
#include "debug/profiler.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "platform/file_system.hpp"
#include "platform/input.hpp"
#include "system/engine.hpp"

namespace wmoge {

    Status yaml_read(const YamlConstNodeRef& node, ActionActivation& activation) {
        WG_YAML_READ_AS_OPT(node, "device_name", activation.device_name);
        WG_YAML_READ_AS_OPT(node, "device_type", activation.device_type);
        WG_YAML_READ_AS_OPT(node, "action", activation.action);
        WG_YAML_READ_AS_OPT(node, "key", activation.key);
        WG_YAML_READ_AS_OPT(node, "mouse_button", activation.mouse_button);
        WG_YAML_READ_AS_OPT(node, "joystick", activation.joystick);
        WG_YAML_READ_AS_OPT(node, "joystick_button", activation.joystick_button);
        WG_YAML_READ_AS_OPT(node, "axis", activation.axis);
        WG_YAML_READ_AS_OPT(node, "gamepad_button", activation.gamepad_button);
        WG_YAML_READ_AS_OPT(node, "gamepad_axis", activation.gamepad_axis);
        WG_YAML_READ_AS_OPT(node, "threshold", activation.threshold);
        WG_YAML_READ_AS_OPT(node, "direction", activation.direction);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ActionActivation& activation) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "device_name", activation.device_name);
        WG_YAML_WRITE_AS(node, "device_type", activation.device_type);
        WG_YAML_WRITE_AS(node, "action", activation.action);
        WG_YAML_WRITE_AS(node, "key", activation.key);
        WG_YAML_WRITE_AS(node, "mouse_button", activation.mouse_button);
        WG_YAML_WRITE_AS(node, "joystick", activation.joystick);
        WG_YAML_WRITE_AS(node, "joystick_button", activation.joystick_button);
        WG_YAML_WRITE_AS(node, "axis", activation.axis);
        WG_YAML_WRITE_AS(node, "gamepad_button", activation.gamepad_button);
        WG_YAML_WRITE_AS(node, "gamepad_axis", activation.gamepad_axis);
        WG_YAML_WRITE_AS(node, "threshold", activation.threshold);
        WG_YAML_WRITE_AS(node, "direction", activation.direction);

        return StatusCode::Ok;
    }

    Status yaml_read(const YamlConstNodeRef& node, ActionMapAction& action) {
        WG_YAML_READ_AS(node, "action", action.name);
        WG_YAML_READ_AS(node, "display_name", action.display_name);
        WG_YAML_READ_AS(node, "activations", action.activations);

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ActionMapAction& action) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "action", action.name);
        WG_YAML_WRITE_AS(node, "display_name", action.display_name);
        WG_YAML_WRITE_AS(node, "activations", action.activations);

        return StatusCode::Ok;
    }

    void ActionMap::rename(StringId new_name) {
        m_name = new_name;
    }

    void ActionMap::add_action_activation(const StringId& action_name, const ActionActivation& activation) {
        if (!has_action(action_name)) {
            WG_LOG_ERROR("no such action " << action_name);
            return;
        }
        ActionMapAction& action = m_actions[action_name];
        action.activations.push_back(activation);
    }

    void ActionMap::remove_action(const StringId& action_name) {
        if (!has_action(action_name)) {
            WG_LOG_ERROR("no such action " << action_name);
            return;
        }
        m_actions.erase(action_name);
    }

    const StringId& ActionMap::get_name() {
        return m_name;
    }
    int ActionMap::get_priority() {
        return m_priority;
    }
    bool ActionMap::has_action(const StringId& action_name) {
        return get_action(action_name) != nullptr;
    }
    void ActionMap::add_action(const StringId& action_name, const StringId& display_name) {
        if (has_action(action_name)) {
            WG_LOG_ERROR("an attempt to overwrite action " << action_name);
            return;
        }
        ActionMapAction& action = m_actions[action_name];
        action.name             = action_name;
        action.display_name     = display_name;
    }
    const ActionMapAction* ActionMap::get_action(const StringId& action_name) {
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

    Status yaml_read(const YamlConstNodeRef& node, ActionMap& action_map) {
        std::vector<ActionMapAction> actions;

        WG_YAML_READ_AS(node, "name", action_map.m_name);
        WG_YAML_READ_AS(node, "priority", action_map.m_priority);
        WG_YAML_READ_AS(node, "actions", actions);

        for (auto& action : actions) {
            action_map.m_actions[action.name] = std::move(action);
        }

        return StatusCode::Ok;
    }
    Status yaml_write(YamlNodeRef node, const ActionMap& action_map) {
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "name", action_map.m_name);
        WG_YAML_WRITE_AS(node, "priority", action_map.m_priority);
        WG_YAML_WRITE_AS(node, "actions", action_map.get_actions());

        return StatusCode::Ok;
    }

}// namespace wmoge