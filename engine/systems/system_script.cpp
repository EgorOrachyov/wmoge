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

#include "system_script.hpp"

#include "components/script_component.hpp"
#include "core/engine.hpp"
#include "debug/profiler.hpp"
#include "event/event_action.hpp"
#include "event/event_input.hpp"
#include "event/event_manager.hpp"
#include "event/event_resource.hpp"
#include "event/event_window.hpp"
#include "scene/scene.hpp"
#include "scripting/script_instance.hpp"

namespace wmoge {

    SystemScript::SystemScript(class Scene* scene) : SceneSystem(scene) {
        EventCallback sniffer = [this](const Ref<Event>& event) {
            m_events.push_back(event);
            return false;
        };

        m_on_action         = make_ref<EventListener>(EventAction::type_static(), sniffer);
        m_on_input_mouse    = make_ref<EventListener>(EventMouse::type_static(), sniffer);
        m_on_input_keyboard = make_ref<EventListener>(EventKeyboard::type_static(), sniffer);
        m_on_input_joystick = make_ref<EventListener>(EventJoystick::type_static(), sniffer);
        m_on_input_drop     = make_ref<EventListener>(EventDrop::type_static(), sniffer);
        m_on_token          = make_ref<EventListener>(EventToken::type_static(), sniffer);

        auto event_manager = Engine::instance()->event_manager();
        event_manager->subscribe(m_on_action);
        event_manager->subscribe(m_on_input_mouse);
        event_manager->subscribe(m_on_input_keyboard);
        event_manager->subscribe(m_on_input_joystick);
        event_manager->subscribe(m_on_input_drop);
        event_manager->subscribe(m_on_token);
    }
    SystemScript::~SystemScript() {
        auto event_manager = Engine::instance()->event_manager();
        event_manager->unsubscribe(m_on_action);
        event_manager->unsubscribe(m_on_input_mouse);
        event_manager->unsubscribe(m_on_input_keyboard);
        event_manager->unsubscribe(m_on_input_joystick);
        event_manager->unsubscribe(m_on_input_drop);
        event_manager->unsubscribe(m_on_token);
    }

    void SystemScript::process() {
        WG_AUTO_PROFILE_SCRIPTING("SystemScript::process");

        auto* registry = m_scene->get_registry();
        auto* scripts  = registry->get_container<ScriptComponent>();

        // regular engine update with delta time
        {
            auto dt = float(Engine::instance()->get_delta_time_game());
            scripts->for_each([dt](ScriptComponent& c) { c.get_script()->on_update(dt); });
        }

        // process incoming events
        for (const auto& event_ref : m_events) {
            if (event_ref->type() == EventAction::type_static()) {
                const auto event = event_ref.cast<EventAction>();
                scripts->for_each([&](ScriptComponent& c) {
                    c.get_script()->on_action(event);
                });
            } else if (event_ref->type() == EventMouse::type_static()) {
                const auto event = event_ref.cast<EventMouse>();
                scripts->for_each([&](ScriptComponent& c) {
                    c.get_script()->on_input_mouse(event);
                });
            } else if (event_ref->type() == EventKeyboard::type_static()) {
                const auto event = event_ref.cast<EventKeyboard>();
                scripts->for_each([&](ScriptComponent& c) {
                    c.get_script()->on_input_keyboard(event);
                });
            } else if (event_ref->type() == EventJoystick::type_static()) {
                const auto event = event_ref.cast<EventJoystick>();
                scripts->for_each([&](ScriptComponent& c) {
                    c.get_script()->on_input_joystick(event);
                });
            } else if (event_ref->type() == EventDrop::type_static()) {
                const auto event = event_ref.cast<EventDrop>();
                scripts->for_each([&](ScriptComponent& c) {
                    c.get_script()->on_input_drop(event);
                });
            } else if (event_ref->type() == EventToken::type_static()) {
                const auto event = event_ref.cast<EventToken>();
                scripts->for_each([&](ScriptComponent& c) {
                    c.get_script()->on_token(event);
                });
            }
        }

        m_events.clear();
    }

}// namespace wmoge
