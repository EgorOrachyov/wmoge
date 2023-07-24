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

#ifndef WMOGE_LUA_SCRIPT_HPP
#define WMOGE_LUA_SCRIPT_HPP

#include "core/fast_map.hpp"
#include "resource/script.hpp"
#include "scripting/lua/lua_defs.hpp"

#include <optional>

namespace wmoge {

    /**
     * @class LuaScript
     * @brief Holds info about a lua declared class in a user script
     */
    class LuaScript final : public Script {
    public:
        WG_OBJECT(LuaScript, Script)

        bool load_from_yaml(const YamlConstNodeRef& node) override;
        void copy_to(Resource& copy) override;

        Ref<ScriptInstance> attach_to(Object* object) override;
        bool                has_property(const StringId& property) override;
        bool                has_method(const StringId& method) override;

        const fast_map<StringId, luabridge::LuaRef>& get_lua_properties();
        const fast_map<StringId, luabridge::LuaRef>& get_lua_methods();
        const std::optional<luabridge::LuaRef>&      get_lua_class();
        class LuaScriptSystem*                       get_system();
        lua_State*                                   get_state();

    private:
        fast_map<StringId, luabridge::LuaRef> m_lua_properties;
        fast_map<StringId, luabridge::LuaRef> m_lua_methods;
        std::optional<luabridge::LuaRef>      m_lua_class;
        class LuaScriptSystem*                m_system = nullptr;
        lua_State*                            m_state  = nullptr;
    };

}// namespace wmoge

#endif//WMOGE_LUA_SCRIPT_HPP
