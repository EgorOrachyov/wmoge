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
#include "platform/file_system.hpp"
#include "platform/input.hpp"

#include <magic_enum.hpp>
#include <tinyxml2.hpp>

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

        tinyxml2::XMLDocument document;
        if (document.Parse(reinterpret_cast<const char*>(file_content.data()), static_cast<int>(file_content.size()))) {
            WG_LOG_ERROR("failed to parse file " << filepath);
            return false;
        }

        auto* engine = Engine::instance();
        auto* input  = engine->input();

        auto* xml_action_map = document.FirstChildElement("action_map");
        auto* xml_actions    = xml_action_map->FirstChildElement("actions");
        m_name               = SID(xml_action_map->FindAttribute("name")->Value());

        for (auto* xml_action_info = xml_actions->FirstChildElement("action"); xml_action_info; xml_action_info = xml_action_info->NextSiblingElement("action")) {
            auto* xml_name         = xml_action_info->FindAttribute("name");
            auto* xml_display_name = xml_action_info->FindAttribute("display_name");
            auto* xml_activations  = xml_action_info->FirstChildElement("activations");

            StringId action_name = SID(xml_name->Value());
            add_action(action_name, SID(xml_display_name->Value()));

            for (auto* xml_activation = xml_activations->FirstChildElement("activation"); xml_activation; xml_activation = xml_activation->NextSiblingElement("activation")) {
                auto* xml_device_name     = xml_activation->FirstChildElement("device_name");
                auto* xml_device_type     = xml_activation->FirstChildElement("device_type");
                auto* xml_action          = xml_activation->FirstChildElement("action");
                auto* xml_key             = xml_activation->FirstChildElement("key");
                auto* xml_mouse_button    = xml_activation->FirstChildElement("mouse_button");
                auto* xml_joystick        = xml_activation->FirstChildElement("joystick");
                auto* xml_joystick_button = xml_activation->FirstChildElement("joystick_button");
                auto* xml_axis            = xml_activation->FirstChildElement("axis");
                auto* xml_threshold       = xml_activation->FirstChildElement("threshold");
                auto* xml_direction       = xml_activation->FirstChildElement("direction");

                ActionActivation activation;
                if (xml_device_name) { activation.device_name = SID(xml_device_name->GetText()); }
                if (xml_device_type) { activation.device_type = magic_enum::enum_cast<InputDeviceType>(xml_device_type->GetText()).value(); }
                if (xml_action) { activation.action = magic_enum::enum_cast<InputAction>(xml_action->GetText()).value(); }
                if (xml_key) { activation.key = magic_enum::enum_cast<InputKeyboardKey>(xml_key->GetText()).value(); }
                if (xml_mouse_button) { activation.mouse_button = magic_enum::enum_cast<InputMouseButton>(xml_mouse_button->GetText()).value(); }
                if (xml_joystick) { activation.joystick = xml_joystick->IntText(-1); }
                if (xml_joystick_button) { activation.joystick_button = input->joystick_mapping(SID(xml_joystick_button->GetText())); }
                if (xml_axis) { activation.axis = input->joystick_mapping(SID(xml_axis->GetText())); }
                if (xml_threshold) { activation.threshold = xml_threshold->FloatText(0.1f); }
                if (xml_direction) { activation.direction = xml_direction->FloatText(0.0); }

                add_action_activation(action_name, activation);
            }
        }

        return true;
    }

    void ActionMap::rename(StringId new_name) {
        m_name = std::move(new_name);
    }

    const StringId& ActionMap::get_name() {
        return m_name;
    }
    const std::string& ActionMap::get_path() {
        return m_path;
    }
    bool ActionMap::is_active() {
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
    const ActionMapAction* ActionMap::get_action(const StringId& action_name) {
        auto query = m_actions.find(action_name);
        return query != m_actions.end() ? &query->second : nullptr;
    }

}// namespace wmoge