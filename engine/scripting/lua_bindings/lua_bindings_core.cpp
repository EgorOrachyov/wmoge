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

#include "lua_bindings_core.hpp"

namespace wmoge {

    static int lua_Log_log(lua_State* state) {
        auto arg0 = WG_LUA_ARG(state, 0);
        auto arg1 = WG_LUA_ARG(state, 1);
        auto arg2 = WG_LUA_ARG(state, 2);
        auto arg3 = WG_LUA_ARG(state, 3);
        auto arg4 = WG_LUA_ARG(state, 4);

        WG_LUA_CHECK_MSG(arg0.isNumber(), "expected number as log level");
        const LogLevel level = static_cast<LogLevel>(arg0.cast<int>());

        WG_LUA_CHECK_MSG(arg1.isString(), "expected string message");
        const std::string message = arg1.cast<std::string>();

        const std::string file     = arg2.isString() ? arg2.cast<std::string>() : __FILE__;
        const std::string function = arg3.isString() ? arg3.cast<std::string>() : __FUNCTION__;
        const std::size_t line     = arg4.isNumber() ? arg4.cast<int>() : __LINE__ + 2;

        Log::instance()->log(level, std::move(message), std::move(file), std::move(function), line);

        return 0;
    }

    static int lua_CmdLine_get_int(lua_State* state) {
        auto  arg0     = WG_LUA_ARG(state, 0);
        auto* cmd_line = Engine::instance()->cmd_line();

        auto value = luabridge::LuaRef(state, cmd_line->get_int(arg0.cast<std::string>()));
        value.push();

        return 1;
    }

    static int lua_CmdLine_get_bool(lua_State* state) {
        auto  arg0     = WG_LUA_ARG(state, 0);
        auto* cmd_line = Engine::instance()->cmd_line();

        auto value = luabridge::LuaRef(state, cmd_line->get_bool(arg0.cast<std::string>()));
        value.push();

        return 1;
    }

    static int lua_CmdLine_get_string(lua_State* state) {
        auto  arg0     = WG_LUA_ARG(state, 0);
        auto* cmd_line = Engine::instance()->cmd_line();

        auto value = luabridge::LuaRef(state, cmd_line->get_string(arg0.cast<std::string>()));
        value.push();

        return 1;
    }

    static int lua_CmdLine_get_help(lua_State* state) {
        auto* cmd_line = Engine::instance()->cmd_line();

        auto help = luabridge::LuaRef(state, cmd_line->get_help());
        help.push();

        return 1;
    }

    static int lua_Random_next_float(lua_State* state) {
        luabridge::LuaRef(state, Random::next_float()).push();
        return 1;
    }

    static int lua_Random_next_float_in_range(lua_State* state) {
        auto left  = WG_LUA_ARG(state, 0);
        auto right = WG_LUA_ARG(state, 1);

        WG_LUA_CHECK_MSG(left.isNumber(), "left must be number");
        WG_LUA_CHECK_MSG(right.isNumber(), "right must be number");
        luabridge::LuaRef(state, Random::next_float(left.cast<float>(), right.cast<float>())).push();
        return 1;
    }

    void LuaBindings::bind_core(luabridge::Namespace& ns, flat_map<const Class*, LuaConvCppToLua>& mapping) {
        ns = ns.beginClass<LuaRefCnt>("RefCnt")
                     .addFunction("is_null", &LuaRefCnt::is_null)
                     .addFunction("ref", &LuaRefCnt::ref)
                     .addFunction("unref", &LuaRefCnt::unref)
                     .addFunction("refs_count", &LuaRefCnt::refs_count)
                     .endClass();

        ns = ns.beginClass<Strid>("Strid")
                     .addStaticFunction("new", std::function([](const std::string& id) { return SID(id); }))
                     .addConstructor<void (*)(void)>()
                     .addProperty("empty", &Strid::empty)
                     .addProperty("id", &Strid::id)
                     .addProperty("hash", &Strid::hash)
                     .addProperty("str", &Strid::str)
                     .addProperty("__tostring", &Strid::str)
                     .endClass();

        ns = ns.deriveClass<LuaData, LuaRefCnt>("Data")
                     .addConstructor<void (*)(void)>()
                     .addProperty("size", &LuaData::size)
                     .addProperty("size_as_kib", &LuaData::size_as_kib)
                     .addFunction("to_string", &LuaData::to_string)
                     .addFunction("__tostring", &LuaData::to_string)
                     .endClass();

        WG_LUA_MAP_CLASS(mapping, Object);
        ns = ns.deriveClass<LuaObject, LuaRefCnt>("Object")
                     .addFunction("class_name", &LuaObject::class_name)
                     .addFunction("signal", &LuaObject::signal)
                     .addFunction("to_string", &LuaObject::to_string)
                     .addFunction("__tostring", &LuaObject::to_string)
                     .endClass();

        ns = ns.beginNamespace("Log")
                     .addCFunction("log", lua_Log_log)
                     .WG_LUA_ADD_ENUM(LogLevel, Never)
                     .WG_LUA_ADD_ENUM(LogLevel, Info)
                     .WG_LUA_ADD_ENUM(LogLevel, Warning)
                     .WG_LUA_ADD_ENUM(LogLevel, Error)
                     .endNamespace();

        ns = ns.beginNamespace("CmdLine")
                     .addCFunction("get_bool", lua_CmdLine_get_bool)
                     .addCFunction("get_int", lua_CmdLine_get_int)
                     .addCFunction("get_string", lua_CmdLine_get_string)
                     .addCFunction("get_help", lua_CmdLine_get_help)
                     .endNamespace();

        ns = ns.beginNamespace("Random")
                     .addCFunction("next_float", lua_Random_next_float)
                     .addCFunction("next_float", lua_Random_next_float_in_range)
                     .endNamespace();
    }

}// namespace wmoge