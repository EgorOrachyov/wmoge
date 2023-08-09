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

#include "lua_bindings_gameplay.hpp"

namespace wmoge {

    static int lua_ActionManager_load_action_map(lua_State* state) {
        auto path = WG_LUA_ARG(state, 0);
        WG_LUA_CHECK_MSG(path.isString(), "path must be string");

        auto* action_manager = Engine::instance()->action_manager();
        luabridge::LuaRef(state, action_manager->load(path.cast<std::string>())).push();
        return 1;
    }

    static int lua_ActionManager_has_action_map(lua_State* state) {
        auto  name           = WG_LUA_ARG(state, 0);
        auto* action_manager = Engine::instance()->action_manager();
        luabridge::LuaRef(state, action_manager->has(LuaTypeTraits::to_sid(name))).push();
        return 1;
    }

    static int lua_ActionManager_enable_action_map(lua_State* state) {
        auto  name           = WG_LUA_ARG(state, 0);
        auto* action_manager = Engine::instance()->action_manager();
        action_manager->activate(LuaTypeTraits::to_sid(name), true);
        return 0;
    }

    static int lua_ActionManager_disable_action_map(lua_State* state) {
        auto  name           = WG_LUA_ARG(state, 0);
        auto* action_manager = Engine::instance()->action_manager();
        action_manager->activate(LuaTypeTraits::to_sid(name), false);
        return 0;
    }

    static int lua_GameTokenManager_set(lua_State* state) {
        auto  name               = WG_LUA_ARG(state, 0);
        auto  value              = WG_LUA_ARG(state, 1);
        auto* game_token_manager = Engine::instance()->game_token_manager();
        game_token_manager->set(LuaTypeTraits::to_sid(name), LuaTypeTraits::to_var(value));
        return 0;
    }

    static int lua_GameTokenManager_get(lua_State* state) {
        auto  name               = WG_LUA_ARG(state, 0);
        auto* game_token_manager = Engine::instance()->game_token_manager();

        Var value;
        if (game_token_manager->get(LuaTypeTraits::to_sid(name), value)) {
            LuaTypeTraits::from_var(state, value).push();
            return 1;
        }

        return 0;
    }

    void LuaBindings::bind_gameplay(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping) {
        ns = ns.beginNamespace("ActionManager")
                     .addFunction("load", lua_ActionManager_load_action_map)
                     .addFunction("has", lua_ActionManager_has_action_map)
                     .addFunction("enable_action_map", lua_ActionManager_enable_action_map)
                     .addFunction("disable_action_map", lua_ActionManager_disable_action_map)
                     .endNamespace();

        ns = ns.beginNamespace("GameTokenManager")
                     .addFunction("set", lua_GameTokenManager_set)
                     .addFunction("get", lua_GameTokenManager_get)
                     .endNamespace();
    }

}// namespace wmoge
