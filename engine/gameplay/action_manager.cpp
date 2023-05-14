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

#include "core/engine.hpp"
#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "event/event_manager.hpp"

namespace wmoge {

    ActionManager::ActionManager() {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::ActionManager");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        m_listener_mouse    = make_listener<EventMouse>([this](const EventMouse& event) { return on_input_mouse(event); });
        m_listener_keyboard = make_listener<EventKeyboard>([this](const EventKeyboard& event) { return on_input_keyboard(event); });
        m_listener_joystick = make_listener<EventJoystick>([this](const EventJoystick& event) { return on_input_joystick(event); });

        event_manager->subscribe(m_listener_mouse);
        event_manager->subscribe(m_listener_keyboard);
        event_manager->subscribe(m_listener_joystick);
    }
    ActionManager::~ActionManager() {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::~ActionManager");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        event_manager->unsubscribe(m_listener_mouse);
        event_manager->unsubscribe(m_listener_keyboard);
        event_manager->unsubscribe(m_listener_joystick);
    }

    void ActionManager::update() {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::update");

        auto*       engine        = Engine::instance();
        auto*       input         = engine->input();
        auto*       event_manager = engine->event_manager();
        const auto& keys          = input->keyboard()->keys_states();

        for (const auto& action_map : m_maps) {
            if (!action_map->is_active()) continue;

            for (const auto& entry : action_map->m_actions) {
                const ActionMapAction&               action_map_action  = entry.second;
                const StringId&                      action_name        = action_map_action.get_name();
                const fast_vector<ActionActivation>& action_activations = action_map_action.get_activations();

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Joystick &&
                        activation.axis >= 0 &&
                        activation.joystick >= 0) {

                        Ref<Joystick> joystick = input->joystick(activation.joystick);
                        if (joystick && joystick->state() == InputDeviceState::Connected) {
                            const auto& axes_states = joystick->axes_states();

                            if (activation.axis < axes_states.size() &&
                                (activation.direction * axes_states[activation.axis]) >= activation.threshold) {

                                Ref<EventAction> event_action = make_event<EventAction>();
                                event_action->name            = action_name;
                                event_action->strength        = Math::clamp(Math::abs(axes_states[activation.axis]), 0.0f, 1.0f);

                                event_manager->dispatch(event_action.as<Event>());
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    bool ActionManager::add_action_map(Ref<ActionMap> action_map) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::add_action_map");

        if (!action_map) {
            WG_LOG_ERROR("passed null action map");
            return false;
        }
        if (has_action_map(action_map->get_name())) {
            WG_LOG_ERROR("already have action map with the same name " << action_map->get_name());
            return false;
        }

        m_maps.push_back(std::move(action_map));
        return true;
    }
    bool ActionManager::load_action_map(const std::string& filepath) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::load_action_map");

        Ref<ActionMap> action_map = make_ref<ActionMap>();

        if (!action_map->load(filepath)) {
            WG_LOG_ERROR("failed to load action map " << filepath);
            return false;
        }

        return add_action_map(std::move(action_map));
    }
    bool ActionManager::has_action_map(const StringId& name) {
        return get_action_map(name) != nullptr;
    }
    void ActionManager::enable_action_map(const StringId& name) {
        ActionMap* map = get_action_map(name);

        if (!map) {
            WG_LOG_ERROR("no such action map loaded " << name);
            return;
        }

        map->m_is_active = true;
    }
    void ActionManager::disable_action_map(const StringId& name) {
        ActionMap* map = get_action_map(name);

        if (!map) {
            WG_LOG_ERROR("no such action map loaded " << name);
            return;
        }

        map->m_is_active = false;
    }

    bool ActionManager::on_input_mouse(const EventMouse& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_mouse");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        for (const auto& action_map : m_maps) {
            if (!action_map->is_active()) continue;

            for (const auto& entry : action_map->m_actions) {
                const ActionMapAction&               action_map_action  = entry.second;
                const StringId&                      action_name        = action_map_action.get_name();
                const fast_vector<ActionActivation>& action_activations = action_map_action.get_activations();

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Mouse &&
                        activation.mouse_button == event.button &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        event_manager->dispatch(event_action.as<Event>());
                        break;
                    }
                }
            }
        }

        return false;
    }
    bool ActionManager::on_input_keyboard(const EventKeyboard& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_keyboard");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        for (const auto& action_map : m_maps) {
            if (!action_map->is_active()) continue;

            for (const auto& entry : action_map->m_actions) {
                const ActionMapAction&               action_map_action  = entry.second;
                const StringId&                      action_name        = action_map_action.get_name();
                const fast_vector<ActionActivation>& action_activations = action_map_action.get_activations();

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Keyboard &&
                        activation.key == event.key &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        event_manager->dispatch(event_action.as<Event>());
                        break;
                    }
                }
            }
        }

        return false;
    }
    bool ActionManager::on_input_joystick(const EventJoystick& event) {
        WG_AUTO_PROFILE_GAMEPLAY("ActionManager::on_input_joystick");

        auto* engine        = Engine::instance();
        auto* event_manager = engine->event_manager();

        for (const auto& action_map : m_maps) {
            if (!action_map->is_active()) continue;

            for (const auto& entry : action_map->m_actions) {
                const ActionMapAction&               action_map_action  = entry.second;
                const StringId&                      action_name        = action_map_action.get_name();
                const fast_vector<ActionActivation>& action_activations = action_map_action.get_activations();

                for (const ActionActivation& activation : action_activations) {
                    if (activation.device_type == InputDeviceType::Joystick &&
                        activation.joystick == event.joystick->id() &&
                        activation.joystick_button == event.button &&
                        activation.action == event.action) {

                        Ref<EventAction> event_action = make_event<EventAction>();
                        event_action->name            = action_name;
                        event_action->strength        = 1.0f;

                        event_manager->dispatch(event_action.as<Event>());
                        break;
                    }
                }
            }
        }

        return false;
    }

    ActionMap* ActionManager::get_action_map(const StringId& name) {
        for (const auto& map : m_maps) {
            if (map->get_name() == name) {
                return map.get();
            }
        };
        return nullptr;
    }

}// namespace wmoge