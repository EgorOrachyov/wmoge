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

#include "lua_script.hpp"

#include "core/object.hpp"
#include "profiler/profiler.hpp"
#include "scripting/lua/lua_script_instance.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"
#include "system/engine.hpp"

namespace wmoge {

    Ref<ScriptInstance> LuaScript::attach_to(Object* object) {
        WG_AUTO_PROFILE_LUA("LuaScript::attach_to");

        if (!object) {
            WG_LOG_ERROR("passed null object to attach to");
            return {};
        }

        std::lock_guard guard(m_system->get_mutex());
        luaL_dostring(m_state, get_code().c_str());

        auto user_object = luabridge::LuaRef::fromStack(m_state, -1);
        if (!user_object.isTable()) {
            std::stringstream out_error;
            user_object.print(out_error);
            WG_LOG_ERROR("expecting created instance to be an object (lua table)");
            WG_LOG_ERROR("luabridge log: " << out_error.str());
            return {};
        }

        auto parent_object = user_object[m_system->get_object_parent()];
        if (!parent_object.isInstance<LuaObject>()) {
            WG_LOG_ERROR("expecting object parent to sub-class <wmoge.Object> class");
            return {};
        }

        parent_object.cast<LuaObject&>().ptr = std::move(Ref(object));
        assert(parent_object.cast<LuaObject&>().ptr.get() == object);

        return make_ref<LuaScriptInstance>(std::move(user_object), Ref<LuaScript>(this), object, m_state);
    }
    bool LuaScript::has_property(const Strid& property) {
        return m_lua_properties.find(property) != m_lua_properties.end();
    }
    bool LuaScript::has_method(const Strid& method) {
        return m_lua_methods.find(method) != m_lua_methods.end();
    }

    const flat_map<Strid, luabridge::LuaRef>& LuaScript::get_lua_properties() {
        return m_lua_properties;
    }
    const flat_map<Strid, luabridge::LuaRef>& LuaScript::get_lua_methods() {
        return m_lua_methods;
    }
    const std::optional<luabridge::LuaRef>& LuaScript::get_lua_class() {
        return m_lua_class;
    }
    class LuaScriptSystem* LuaScript::get_system() {
        return m_system;
    }
    lua_State* LuaScript::get_state() {
        return m_state;
    }

}// namespace wmoge