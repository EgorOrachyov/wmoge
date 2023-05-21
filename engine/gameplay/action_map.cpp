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
#include "core/engine.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "io/enum.hpp"
#include "io/yaml.hpp"
#include "platform/file_system.hpp"
#include "platform/input.hpp"

namespace wmoge {

    const StringId& ActionMapAction::get_name() const {
        return m_name;
    }
    const StringId& ActionMapAction::get_display_name() const {
        return m_display_name;
    }
    const fast_vector<ActionActivation>& ActionMapAction::get_activations() const {
        return m_activations;
    }

    bool ActionMap::load(const std::string& filepath) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionMap::load");

        std::vector<std::uint8_t> file_content;
        FileSystem*               file_system = Engine::instance()->file_system();

        if (!file_system->read_file(filepath, file_content)) {
            WG_LOG_ERROR("failed to load action map file " << filepath);
            return false;
        }

        YamlTree tree = Yaml::parse(file_content);
        if (tree.empty()) {
            WG_LOG_ERROR("failed to parse action map file " << filepath);
            return false;
        }

        m_name = Yaml::read_sid(tree["name"]);

        auto actions_list = tree["actions"];
        for (auto action_iter = actions_list.first_child(); action_iter.valid(); action_iter = action_iter.next_sibling()) {
            auto action_name = Yaml::read_sid(action_iter["action"]);

            ActionMapAction& action = m_actions[action_name];
            action.m_name           = action_name;
            action.m_display_name   = Yaml::read_sid(action_iter["display_name"]);

            auto activations_list = action_iter["activations"];
            for (auto activation_iter = activations_list.first_child(); activation_iter.valid(); activation_iter = activation_iter.next_sibling()) {
                ActionActivation activation;

                if (activation_iter.has_child("device_name")) {
                    activation.device_name = Yaml::read_sid(activation_iter["device_name"]);
                }
                if (activation_iter.has_child("device_type")) {
                    activation.device_type = Enum::parse<InputDeviceType>(activation_iter["device_type"]);
                }
                if (activation_iter.has_child("action")) {
                    activation.action = Enum::parse<InputAction>(activation_iter["action"]);
                }
                if (activation_iter.has_child("key")) {
                    activation.key = Enum::parse<InputKeyboardKey>(activation_iter["key"]);
                }
                if (activation_iter.has_child("mouse_button")) {
                    activation.mouse_button = Enum::parse<InputMouseButton>(activation_iter["mouse_button"]);
                }
                if (activation_iter.has_child("joystick")) {
                    activation.joystick = Yaml::read_int(activation_iter["joystick"]);
                }
                if (activation_iter.has_child("joystick_button")) {
                    activation.joystick_button = Yaml::read_int(activation_iter["joystick_button"]);
                }
                if (activation_iter.has_child("axis")) {
                    activation.axis = Yaml::read_int(activation_iter["axis"]);
                }
                if (activation_iter.has_child("gamepad_button")) {
                    activation.gamepad_button = int(Enum::parse<InputGamepadButton>(activation_iter["gamepad_button"]));
                }
                if (activation_iter.has_child("gamepad_axis")) {
                    activation.gamepad_axis = int(Enum::parse<InputGamepadAxis>(activation_iter["gamepad_axis"]));
                }
                if (activation_iter.has_child("threshold")) {
                    activation.threshold = Yaml::read_float(activation_iter["threshold"]);
                }
                if (activation_iter.has_child("direction")) {
                    activation.direction = Yaml::read_float(activation_iter["direction"]);
                }

                action.m_activations.push_back(activation);
            }
        }

        WG_LOG_INFO("load action map " << m_name);

        return true;
    }

    void ActionMap::rename(StringId new_name) {
        m_name = new_name;
    }
    void ActionMap::enable() {
        if (!m_is_active) {
            m_is_active = true;
            WG_LOG_INFO("enable action map " << m_name);
        }
    }
    void ActionMap::disable() {
        if (m_is_active) {
            m_is_active = false;
            WG_LOG_INFO("disable action map " << m_name);
        }
    }
    void ActionMap::add_action_activation(const StringId& action_name, const ActionActivation& activation) {
        if (!has_action(action_name)) {
            WG_LOG_ERROR("no such action " << action_name);
            return;
        }
        ActionMapAction& action = m_actions[action_name];
        action.m_activations.push_back(activation);
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
    const std::string& ActionMap::get_path() {
        return m_path;
    }
    bool ActionMap::is_active() const {
        return m_is_active;
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
        action.m_name           = action_name;
        action.m_display_name   = display_name;
    }
    const ActionMapAction* ActionMap::get_action(const StringId& action_name) {
        auto query = m_actions.find(action_name);
        return query != m_actions.end() ? &query->second : nullptr;
    }

}// namespace wmoge