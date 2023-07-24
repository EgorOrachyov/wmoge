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

#include "core/engine.hpp"
#include "core/object.hpp"
#include "debug/profiler.hpp"
#include "scripting/lua/lua_script_instance.hpp"
#include "scripting/lua/lua_script_system.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"

namespace wmoge {

    bool LuaScript::load_from_yaml(const YamlConstNodeRef& node) {
        WG_AUTO_PROFILE_LUA("LuaScript::load_from_import_options");

        if (!Script::load_from_yaml(node)) {
            return false;
        }

        auto* engine        = Engine::instance();
        auto* script_system = engine->script_system();

        m_system = dynamic_cast<LuaScriptSystem*>(script_system);

        if (!m_system) {
            WG_LOG_ERROR("engine script system not lua");
            return false;
        }

        if (m_system->get_language() != get_language()) {
            WG_LOG_ERROR("cannot load script of language " << get_language() << " for system " << m_system->get_name());
            return false;
        }

        std::lock_guard guard(m_system->get_mutex());
        m_state = m_system->get_global_state();

        luaL_dostring(m_state, get_code().c_str());
        auto user_object = luabridge::LuaRef::fromStack(m_state, -1);

        if (!user_object.isTable()) {
            std::stringstream out_error;
            user_object.print(out_error);
            WG_LOG_ERROR("expecting created instance to be an object (lua table)");
            WG_LOG_ERROR("luabridge log: " << out_error.str());
            return false;
        }

        if (!user_object[m_system->get_object_class()].isTable()) {
            WG_LOG_ERROR("expecting user class to be a table");
            return false;
        }

        auto user_class = user_object[m_system->get_object_class()];
        auto user_mask  = ScriptFunctionsMask();

        for (int i = 0; i < static_cast<int>(ScriptFunction::Total); i++) {
            if (user_class[m_system->get_object_callbacks()[i]].isFunction()) {
                user_mask.set(static_cast<ScriptFunction>(i));
            }
        }

        m_mask = user_mask;

        return true;
    }
    void LuaScript::copy_to(Resource& copy) {
        Script::copy_to(copy);
        auto* script             = dynamic_cast<LuaScript*>(&copy);
        script->m_state          = m_state;
        script->m_system         = m_system;
        script->m_lua_class      = m_lua_class;
        script->m_lua_methods    = m_lua_methods;
        script->m_lua_properties = m_lua_properties;
    }

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
    bool LuaScript::has_property(const StringId& property) {
        return m_lua_properties.find(property) != m_lua_properties.end();
    }
    bool LuaScript::has_method(const StringId& method) {
        return m_lua_methods.find(method) != m_lua_methods.end();
    }

    const fast_map<StringId, luabridge::LuaRef>& LuaScript::get_lua_properties() {
        return m_lua_properties;
    }
    const fast_map<StringId, luabridge::LuaRef>& LuaScript::get_lua_methods() {
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

    void LuaScript::register_class() {
        auto* cls = Class::register_class<LuaScript>();
    }

}// namespace wmoge