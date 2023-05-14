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

#ifndef WMOGE_LUA_BRIDGE_HPP
#define WMOGE_LUA_BRIDGE_HPP

#include "scripting/lua/lua_defs.hpp"
#include "scripting/lua/lua_type_traits.hpp"

#include <magic_enum.hpp>

namespace wmoge {

    /**
     * @class LuaBindings
     * @brief Binds internal various engine modules API to lua
     */
    class LuaBindings {
    public:
        static void bind_math(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_core(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_platform(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_event(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_resource(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_gameplay(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_scene(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);
        static void bind_components(luabridge::Namespace& ns, fast_map<const Class*, LuaConvCppToLua>& mapping);

        template<typename Enum>
        static void bind_enum(luabridge::Namespace& ns, const char* name) {
            constexpr auto entries = magic_enum::enum_entries<Enum>();
            ns                     = ns.beginNamespace(name);

            for (const auto& entry : entries) {
                auto entry_name = std::string(entry.second);
                ns              = ns.addConstant(entry_name.c_str(), static_cast<int>(entry.first));
            }

            ns = ns.endNamespace();
        }
    };

}// namespace wmoge

#define WG_LUA_ARG(state, idx) luabridge::LuaRef::fromStack(state, idx + 1)

#define WG_LUA_ADD_ENUM(class, name) addConstant(#name, static_cast<int>(class ::name))

#define WG_LUA_MAP_CLASS_CUSTOM(mapping, EngineClass, rule) \
    mapping[EngineClass::class_ptr_static()] = rule;

#define WG_LUA_MAP_CLASS(mapping, EngineClass)                                        \
    mapping[EngineClass::class_ptr_static()] = [](lua_State* state, Object* object) { \
        return luabridge::LuaRef(state, Lua##EngineClass{Ref<Object>(object)});       \
    }

#define WG_LUA_CHECK_MSG(condition, message) \
    do {                                     \
        if (!(condition)) {                  \
            WG_LOG_ERROR(message);           \
            return 0;                        \
        }                                    \
    } while (false);

#endif//WMOGE_LUA_BRIDGE_HPP
