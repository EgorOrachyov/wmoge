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

#ifndef WMOGE_LUA_SCRIPT_SYSTEM_HPP
#define WMOGE_LUA_SCRIPT_SYSTEM_HPP

#include "core/callback_queue.hpp"
#include "core/class.hpp"
#include "core/string_id.hpp"
#include "scripting/lua/lua_defs.hpp"
#include "scripting/script_system.hpp"
#include <core/buffered_vector.hpp>
#include <core/flat_map.hpp>

#include <mutex>
#include <optional>
#include <vector>

namespace wmoge {

    /**
     * @class LuaScriptSystem
     * @brief Lua language based script system of the engine
     */
    class LuaScriptSystem final : public ScriptSystem {
    public:
        LuaScriptSystem();
        ~LuaScriptSystem() override;

        void gc() override;
        void update() override;
        void shutdown() override;

        const std::vector<std::string>&                get_object_callbacks();
        const std::string&                             get_object_parent();
        const std::string&                             get_object_class();
        const flat_map<const Class*, LuaConvCppToLua>& get_object_to_lua();
        lua_State*                                     get_global_state();
        std::recursive_mutex&                          get_mutex();

    private:
        flat_map<const Class*, LuaConvCppToLua> m_object_to_lua;
        std::vector<std::string>                m_object_callbacks;
        std::string                             m_object_parent = "parent";
        std::string                             m_object_class  = "class";
        lua_State*                              m_global_state  = nullptr;
        std::optional<luabridge::LuaRef>        m_new_inst_func_lua;
        CallbackQueue                           m_queue;

        mutable std::recursive_mutex m_mutex;
    };

}// namespace wmoge

#endif//WMOGE_LUA_SCRIPT_SYSTEM_HPP
