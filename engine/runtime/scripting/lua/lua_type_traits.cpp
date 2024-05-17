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

#include "lua_type_traits.hpp"

#include "scripting/lua/lua_script_instance.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"

namespace wmoge {

    Strid LuaTypeTraits::to_sid(luabridge::LuaRef& ref) {
        assert(ref.isString() || ref.isInstance<Strid>());

        if (ref.isString()) {
            return SID(ref.cast<std::string>());
        }
        if (ref.isInstance<Strid>()) {
            return ref.cast<Strid>();
        }

        WG_LOG_ERROR("failed to convert lua object to string id");

        return Strid();
    }

    Var LuaTypeTraits::to_var(luabridge::LuaRef& ref) {
        if (ref.isNil()) {
            return Var();
        }
        if (ref.isBool()) {
            return Var(ref.cast<int>());
        }
        if (ref.isNumber()) {
            return Var(ref.cast<float>());
        }
        if (ref.isString()) {
            return Var(ref.cast<std::string>());
        }

        WG_LOG_ERROR("unsupported lua to var type");
        return Var();
    }

    luabridge::LuaRef LuaTypeTraits::from_object(lua_State* state, Object* object) {
        assert(state);

        if (!object) {
            // null object: ok, return nil
            return luabridge::LuaRef(state);
        }

        // Look up to convert native object
        auto*       script_system = reinterpret_cast<LuaScriptSystem*>(Engine::instance()->script_system());
        const auto& mappings      = script_system->get_object_to_lua();
        const auto* cls           = object->class_ptr();

        auto rule = mappings.find(cls);
        if (rule == mappings.end()) {
            WG_LOG_ERROR("no such rule to map object of type " << cls->name());
            return luabridge::LuaRef(state);
        }

        return rule->second(state, object);
    }

    luabridge::LuaRef LuaTypeTraits::from_var(lua_State* state, const Var& var) {
        if (var.type() == VarType::Nil) {
            return luabridge::LuaRef(state);
        }
        if (var.type() == VarType::Int) {
            return luabridge::LuaRef(state, var.operator int());
        }
        if (var.type() == VarType::Float) {
            return luabridge::LuaRef(state, var.operator float());
        }
        if (var.type() == VarType::String) {
            return luabridge::LuaRef(state, var.operator std::string());
        }
        if (var.type() == VarType::Strid) {
            return luabridge::LuaRef(state, var.operator Strid().str());
        }

        WG_LOG_ERROR("unsupported var to lua type");
        return luabridge::LuaRef(state);
    }

}// namespace wmoge