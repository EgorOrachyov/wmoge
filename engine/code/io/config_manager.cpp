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

#include "config_manager.hpp"

namespace wmoge {

    CfgManager::CfgManager(InitValResolver resolver)
        : m_init_val_resolver(std::move(resolver)) {
    }

    void CfgManager::add_object(Ref<CfgValState> object) {
        if (has_object(object->name)) {
            WG_LOG_ERROR("duplicated object registration, skip " << object->name);
            return;
        }
        m_objects[object->name] = std::move(object);
    }

    Ref<CfgValState> CfgManager::add_val(Strid name, std::string help, Var value) {
        if (m_init_val_resolver) {
            Var new_value;
            if (!m_init_val_resolver(name, value.type(), new_value)) {
                WG_LOG_ERROR("failed to resolve init value for " << name);
            } else {
                if (new_value != value) {
                    WG_LOG_INFO("override " << name << " to " << new_value << " (default is " << value << ")");
                }
                std::swap(new_value, value);
            }
        }
        auto state          = make_ref<CfgValState>();
        state->name         = name;
        state->help         = std::move(help);
        state->value        = value;
        state->defaul_value = value;
        add_object(state);
        return state;
    }

    Ref<CfgValState> CfgManager::add_trigger(Strid name, std::string help) {
        auto state  = make_ref<CfgValState>();
        state->name = name;
        state->help = std::move(help);
        add_object(state);
        return state;
    }

    Ref<CfgValState> CfgManager::add_cmd(Strid name, std::string help, CfgOnCmdExecute on_execute) {
        auto state        = make_ref<CfgValState>();
        state->name       = name;
        state->help       = std::move(help);
        state->on_execute = std::move(on_execute);
        add_object(state);
        return state;
    }

    Ref<CfgValState> CfgManager::add_list(Strid name, std::string help, int selected, std::vector<std::string> options) {
        auto state     = make_ref<CfgValState>();
        state->name    = name;
        state->help    = std::move(help);
        state->value   = selected;
        state->options = std::move(options);
        add_object(state);
        return state;
    }

    Status CfgManager::set_val(Strid name, Var value) {
        auto obj = try_find_object(name);

        if (!obj) {
            WG_LOG_ERROR("faield to find and cast val " << name);
            return StatusCode::InvalidParameter;
        }

        if (obj->defaul_value.type() != value.type()) {
            WG_LOG_ERROR("mismatched types of val to set " << name);
            return StatusCode::InvalidParameter;
        }

        obj->value = std::move(value);

        return WG_OK;
    }

    Status CfgManager::set_trigger(Strid name, bool value) {
        auto obj = try_find_object(name);

        if (!obj || obj->type != CfgValType::Trigger) {
            WG_LOG_ERROR("faield to find and cast trigger " << name);
            return StatusCode::InvalidParameter;
        }

        if ((bool) obj->value && value) {
            m_triggered.push_back(obj);
        }

        obj->value = value;

        return WG_OK;
    }

    Status CfgManager::set_list(Strid name, int value) {
        auto obj = try_find_object(name);

        if (!obj || obj->type != CfgValType::List) {
            WG_LOG_ERROR("faield to find and cast list " << name);
            return StatusCode::InvalidParameter;
        }
        if (obj->options.size() >= value) {
            WG_LOG_ERROR("no such option to select " << name << " " << value);
            return StatusCode::InvalidParameter;
        }

        obj->value = value;

        return WG_OK;
    }

    Status CfgManager::exec_command(Strid name, array_view<std::string> args) {
        auto obj = try_find_object(name);

        if (!obj || obj->type != CfgValType::Cmd) {
            WG_LOG_ERROR("faield to find and cast cmd " << name);
            return StatusCode::InvalidParameter;
        }

        return obj->on_execute(args);
    }

    Ref<CfgValState> CfgManager::try_find_object(Strid name) {
        auto query = m_objects.find(name);
        if (query == m_objects.end()) {
            return {};
        }
        return query->second;
    }

    bool CfgManager::has_object(Strid name) {
        auto query = m_objects.find(name);
        return query != m_objects.end();
    }

    void CfgManager::update() {
        for (const auto& trigger : m_triggered) {
            trigger->value = false;
        }
        m_triggered.clear();
    }

    void CfgManager::dump_objects(std::vector<Ref<CfgValState>>& out_vals) {
        out_vals.clear();
        out_vals.reserve(m_objects.size());

        for (const auto& entry : m_objects) {
            out_vals.push_back(entry.second);
        }
    }

}// namespace wmoge