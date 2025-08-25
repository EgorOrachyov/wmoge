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

#pragma once

#include "core/flat_map.hpp"
#include "io/config_val.hpp"

#include <functional>
#include <vector>

namespace wmoge {

    /**
     * @class CfgManager
     * @brief Stores and processes all config objects
     */
    class CfgManager {
    public:
        using InitValResolver = std::function<Status(Strid name, VarType type, Var& value)>;

        CfgManager(InitValResolver resolver = InitValResolver());

        void             add_object(Ref<CfgValState> object);
        Ref<CfgValState> add_val(Strid name, std::string help, Var value);
        Ref<CfgValState> add_trigger(Strid name, std::string help);
        Ref<CfgValState> add_cmd(Strid name, std::string help, CfgOnCmdExecute on_execute);
        Ref<CfgValState> add_list(Strid name, std::string help, int selected, std::vector<std::string> options);
        Status           set_val(Strid name, Var value);
        Status           set_trigger(Strid name, bool value);
        Status           set_list(Strid name, int value);
        Status           exec_command(Strid name, array_view<std::string> args);
        Ref<CfgValState> try_find_object(Strid name);
        bool             has_object(Strid name);
        void             update();
        void             dump_objects(std::vector<Ref<CfgValState>>& out_vals);

    private:
        flat_map<Strid, Ref<CfgValState>> m_objects;
        std::vector<Ref<CfgValState>>     m_triggered;
        InitValResolver                   m_init_val_resolver;
    };

}// namespace wmoge

#define WG_CFG_BIND_VAL(cfg, val, help) \
    val.bind(cfg->add_val(val.get_name(), std::string(help), val.get_value()))
#define WG_CFG_BIND_TRIGGER(cfg, val, help) \
    val.bind(cfg->add_trigger(val.get_name(), std::string(help)))
#define WG_CFG_BIND_CMD(cfg, val, function, help) \
    val.bind(cfg->add_cmd(val.get_name(), std::string(help), function))
#define WG_CFG_BIND_LIST(cfg, val, selected, options, help) \
    val.bind(cfg->add_list(val.get_name(), std::string(help), int(selected), options))