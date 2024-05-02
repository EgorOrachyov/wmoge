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

#include "action_manager.hpp"

#include "core/log.hpp"
#include "event/event_manager.hpp"
#include "profiler/profiler.hpp"
#include "system/engine.hpp"

namespace wmoge {

    ActionManager::ActionManager() {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::ActionManager");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        m_listener_mouse    = event_manager->subscribe<EventMouse>([this](const EventMouse& event) { return on_input_mouse(event); });
        m_listener_keyboard = event_manager->subscribe<EventKeyboard>([this](const EventKeyboard& event) { return on_input_keyboard(event); });
        m_listener_joystick = event_manager->subscribe<EventJoystick>([this](const EventJoystick& event) { return on_input_joystick(event); });
        m_listener_gamepad  = event_manager->subscribe<EventGamepad>([this](const EventGamepad& event) { return on_input_gamepad(event); });
    }
    ActionManager::~ActionManager() {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::~ActionManager");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        event_manager->unsubscribe(m_listener_mouse);
        event_manager->unsubscribe(m_listener_keyboard);
        event_manager->unsubscribe(m_listener_joystick);
        event_manager->unsubscribe(m_listener_gamepad);
    }

    void ActionManager::update() {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::update");

        auto*       engine        = Engine::instance();
        auto*       input         = engine->input();
        auto*       event_manager = engine->event_manager();
        const auto& keys          = input->keyboard()->keys_states();

        for (auto& event : m_events) {
            event_manager->dispatch(event);
        }

        m_events.clear();

        for (const auto& info : m_maps) {
            if (!info.active) continue;

            for (const auto& entry : info.action_map->m_actions) {
                const ActionMapAction&                   action_map_action  = entry.second;
                const Strid&                             action_name        = action_map_action.name;
                const buffered_vector<ActionActivation>& action_activations = action_map_action.activations;

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type != InputDeviceType::Joystick) {
                        continue;
                    }
                    if (activation.joystick < 0) {
                        continue;
                    }
                    if (activation.axis < 0 && activation.gamepad_axis == InputGamepadAxis::Unknown) {
                        continue;
                    }

                    const Ref<Joystick> joystick = input->joystick(activation.joystick);

                    if (joystick && joystick->state() == InputDeviceState::Connected) {
                        const bool  gamepad_axis = activation.gamepad_axis != InputGamepadAxis::Unknown;
                        const int   axis_id      = gamepad_axis ? int(activation.gamepad_axis) : activation.axis;
                        const auto& axis_states  = gamepad_axis ? joystick->gamepad_axes_states() : joystick->axes_states();

                        if (axis_id < axis_states.size() && (activation.direction * axis_states[axis_id]) >= activation.threshold) {
                            Ref<EventAction> event_action = make_event<EventAction>();
                            event_action->name            = action_name;
                            event_action->strength        = Math::clamp(Math::abs(axis_states[axis_id]), 0.0f, 1.0f);
                            event_manager->dispatch(event_action.as<Event>());
                            break;
                        }
                    }
                }
            }
        }
    }

    bool ActionManager::load(const std::string& filepath) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::load");

        Ref<ActionMap> action_map = make_ref<ActionMap>();

        if (!yaml_read_file(filepath, *action_map)) {
            WG_LOG_ERROR("failed to parse action map " << filepath);
            return false;
        }

        if (has(action_map->get_name())) {
            remove(action_map->get_name());
        }

        return add(action_map);
    }
    bool ActionManager::add(const Ref<ActionMap>& action_map) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::add");

        if (!action_map) {
            WG_LOG_ERROR("passed null action map");
            return false;
        }
        if (has(action_map->get_name())) {
            WG_LOG_ERROR("already have action map with the same name " << action_map->get_name());
            return false;
        }

        ActionMapInfo& info = m_maps.emplace_back();
        info.active         = false;
        info.priority       = action_map->get_priority();
        info.action_map     = action_map;

        WG_LOG_INFO("add action map " << info.action_map->get_name());

        return true;
    }

    bool ActionManager::remove(const Strid& action_map) {
        const auto query = std::find_if(m_maps.begin(), m_maps.end(), [&](auto& info) { return info.action_map->get_name() == action_map; });
        const bool found = query != m_maps.end();

        m_maps.erase(query);

        return found;
    }

    bool ActionManager::has(const Strid& action_map) {
        return get_action_map_info(action_map) != nullptr;
    }

    void ActionManager::activate(const Strid& action_map, bool active) {
        ActionMapInfo* info = get_action_map_info(action_map);

        if (!info) {
            WG_LOG_ERROR("no such action map loaded " << action_map);
            return;
        }

        info->active = active;

        WG_LOG_INFO("[one] action map " << info->action_map->get_name() << " active=" << active);
    }
    void ActionManager::activate_all(bool active) {
        for (auto& info : m_maps) {
            info.active = active;

            WG_LOG_INFO("[all] action map " << info.action_map->get_name() << " active=" << active);
        }
    }
    void ActionManager::activate_all_except(const Strid& action_map, bool active) {
        for (auto& info : m_maps) {
            if (info.action_map->get_name() != action_map) {
                info.active = active;

                WG_LOG_INFO("[exp] action map " << info.action_map->get_name() << " active=" << active);
            }
        }
    }

    bool ActionManager::on_input_mouse(const EventMouse& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_mouse");

        for (const auto& info : m_maps) {
            if (!info.active) continue;

            for (const auto& entry : info.action_map->m_actions) {
                const ActionMapAction&                   action_map_action  = entry.second;
                const Strid&                             action_name        = action_map_action.name;
                const buffered_vector<ActionActivation>& action_activations = action_map_action.activations;

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Mouse &&
                        activation.mouse_button == event.button &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        m_events.push_back(event_action);
                        break;
                    }
                }
            }
        }

        return false;
    }
    bool ActionManager::on_input_keyboard(const EventKeyboard& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_keyboard");

        for (const auto& info : m_maps) {
            if (!info.active) continue;

            for (const auto& entry : info.action_map->m_actions) {
                const ActionMapAction&                   action_map_action  = entry.second;
                const Strid&                             action_name        = action_map_action.name;
                const buffered_vector<ActionActivation>& action_activations = action_map_action.activations;

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Keyboard &&
                        activation.key == event.key &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        m_events.push_back(event_action);
                        break;
                    }
                }
            }
        }

        return false;
    }
    bool ActionManager::on_input_joystick(const EventJoystick& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_joystick");

        for (const auto& info : m_maps) {
            if (!info.active) continue;

            for (const auto& entry : info.action_map->m_actions) {
                const ActionMapAction&                   action_map_action  = entry.second;
                const Strid&                             action_name        = action_map_action.name;
                const buffered_vector<ActionActivation>& action_activations = action_map_action.activations;

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Joystick &&
                        activation.joystick == event.joystick->id() &&
                        activation.joystick_button == event.button &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        m_events.push_back(event_action);
                        break;
                    }
                }
            }
        }

        return false;
    }
    bool ActionManager::on_input_gamepad(const EventGamepad& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_gamepad");

        for (const auto& info : m_maps) {
            if (!info.active) continue;

            for (const auto& entry : info.action_map->m_actions) {
                const ActionMapAction&                   action_map_action  = entry.second;
                const Strid&                             action_name        = action_map_action.name;
                const buffered_vector<ActionActivation>& action_activations = action_map_action.activations;

                for (const ActionActivation& activation : action_activations) {
                    if (activation.gamepad_button != InputGamepadButton::Unknown &&
                        activation.device_type == InputDeviceType::Joystick &&
                        activation.joystick == event.joystick->id() &&
                        int(activation.gamepad_button) == event.button &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        m_events.push_back(event_action);
                        break;
                    }
                }
            }
        }

        return false;
    }

    ActionManager::ActionMapInfo* ActionManager::get_action_map_info(const Strid& name) {
        for (auto& info : m_maps) {
            if (info.action_map->get_name() == name) {
                return &info;
            }
        };
        return nullptr;
    }

}// namespace wmoge