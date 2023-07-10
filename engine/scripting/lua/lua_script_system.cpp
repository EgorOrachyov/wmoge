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

#include "lua_script_system.hpp"

#include "core/log.hpp"
#include "debug/profiler.hpp"
#include "resource/script.hpp"
#include "scripting/lua/lua_event_script.hpp"
#include "scripting/lua/lua_script.hpp"
#include "scripting/lua_bindings/lua_bindings.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"

#include <sstream>

namespace wmoge {

    LuaScriptSystem::LuaScriptSystem() {
        WG_AUTO_PROFILE_LUA("LuaScriptSystem::LuaScriptSystem");

        LuaScript::register_class();
        LuaEventScript::register_class();

        m_global_state = luaL_newstate();
        m_language     = SID("lua");
        m_name         = SID("lua_script_system");

        if (!m_global_state) {
            WG_LOG_ERROR("failed to create lua state");
            return;
        }

        luaL_openlibs(m_global_state);

        auto ns = luabridge::getGlobalNamespace(m_global_state).beginNamespace("wmoge");
        LuaBindings::bind_math(ns, m_object_to_lua);
        LuaBindings::bind_core(ns, m_object_to_lua);
        LuaBindings::bind_platform(ns, m_object_to_lua);
        LuaBindings::bind_event(ns, m_object_to_lua);
        LuaBindings::bind_resource(ns, m_object_to_lua);
        LuaBindings::bind_gameplay(ns, m_object_to_lua);
        LuaBindings::bind_scene(ns, m_object_to_lua);
        ns = ns.endNamespace();

        luaL_dostring(m_global_state, "function new(target, base)\n"
                                      "    base_instance = base()\n"
                                      "    target.__index = target\n"
                                      "    return setmetatable({ parent = base_instance, class = target }, target)\n"
                                      "end\n"
                                      "return new");

        m_new_inst_func_lua = luabridge::LuaRef::fromStack(m_global_state, -1);
        assert(m_new_inst_func_lua.value().isFunction());

        m_object_callbacks = {"on_create",
                              "on_scene_enter",
                              "on_scene_exit",
                              "on_transform_updated",
                              "on_update",
                              "on_signal",
                              "on_input_mouse",
                              "on_input_keyboard",
                              "on_input_joystick",
                              "on_input_drop",
                              "on_action",
                              "on_token"};

        WG_LOG_INFO("init lua script engine v5.4.4");
    }

    LuaScriptSystem::~LuaScriptSystem() {
        shutdown();
    }

    void LuaScriptSystem::gc() {
        WG_AUTO_PROFILE_LUA("LuaScriptSystem::gc");

        std::lock_guard guard(m_mutex);

        lua_gc(m_global_state, LUA_GCCOLLECT);

        m_gc_cycles += 1;
        m_gc_frames_from_last = 0;
    }
    void LuaScriptSystem::update() {
        WG_AUTO_PROFILE_LUA("LuaScriptSystem::update");

        std::lock_guard guard(m_mutex);

        m_gc_frames_from_last += 1;
        m_queue.flush();

        if (m_gc_frames_from_last >= m_gc_interval) {
            gc();
        }
    }
    void LuaScriptSystem::shutdown() {
        WG_AUTO_PROFILE_LUA("LuaScriptSystem::shutdown");

        if (m_global_state) {
            m_new_inst_func_lua.reset();
            lua_close(m_global_state);
            m_global_state = nullptr;
        }

        WG_LOG_INFO("shutdown lua engine");
    }

    const std::vector<std::string>& LuaScriptSystem::get_object_callbacks() {
        return m_object_callbacks;
    }
    const std::string& LuaScriptSystem::get_object_parent() {
        return m_object_parent;
    }
    const std::string& LuaScriptSystem::get_object_class() {
        return m_object_class;
    }
    const fast_map<const Class*, LuaConvCppToLua>& LuaScriptSystem::get_object_to_lua() {
        return m_object_to_lua;
    }
    lua_State* LuaScriptSystem::get_global_state() {
        return m_global_state;
    }
    std::recursive_mutex& LuaScriptSystem::get_mutex() {
        return m_mutex;
    }

}// namespace wmoge