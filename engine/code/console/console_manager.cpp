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

#include "console_manager.hpp"

namespace wmoge {

    void ConsoleManager::add_object(Ref<ConsoleObject> object) {
        if (has_object(object->get_name())) {
            WG_LOG_ERROR("duplicated object registration " << object->get_name());
            return;
        }

        m_objects[object->get_name()] = std::move(object);
    }

    Status ConsoleManager::set_var(Strid name, Var value) {
        auto        obj = try_find_object(name);
        ConsoleVar* var = dynamic_cast<ConsoleVar*>(obj.get());

        if (!var) {
            WG_LOG_ERROR("faield to find and cast var " << name);
            return StatusCode::InvalidParameter;
        }

        if (var->get_value_type() != value.type()) {
            WG_LOG_ERROR("mismatched types of var to set " << name);
            return StatusCode::InvalidParameter;
        }

        var->m_value = std::move(value);

        return WG_OK;
    }

    Status ConsoleManager::set_trigger(Strid name, bool value) {
        auto            obj     = try_find_object(name);
        ConsoleTrigger* trigger = dynamic_cast<ConsoleTrigger*>(obj.get());

        if (!trigger) {
            WG_LOG_ERROR("faield to find and cast trigger " << name);
            return StatusCode::InvalidParameter;
        }

        if (!trigger->m_triggered && value) {
            m_triggered.push_back(std::move(obj.cast<ConsoleTrigger>()));
        }

        trigger->m_triggered = value;

        return WG_OK;
    }

    Status ConsoleManager::exec_command(Strid name, array_view<std::string> args) {
        auto        obj = try_find_object(name);
        ConsoleCmd* cmd = dynamic_cast<ConsoleCmd*>(obj.get());

        if (!cmd) {
            WG_LOG_ERROR("faield to find and cast cmd " << name);
            return StatusCode::InvalidParameter;
        }

        return cmd->m_on_execute(args);
    }

    Ref<ConsoleObject> ConsoleManager::try_find_object(Strid name) {
        auto query = m_objects.find(name);
        if (query == m_objects.end()) {
            return {};
        }
        return query->second;
    }

    bool ConsoleManager::has_object(Strid name) {
        auto query = m_objects.find(name);
        return query != m_objects.end();
    }

    void ConsoleManager::update() {
        for (const auto& trigger : m_triggered) {
            trigger->m_triggered = false;
        }
        m_triggered.clear();
    }

}// namespace wmoge