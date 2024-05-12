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

#ifndef WMOGE_LUA_TYPE_TRAITS_HPP
#define WMOGE_LUA_TYPE_TRAITS_HPP

#include "scripting/lua/lua_defs.hpp"

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "math/mat.hpp"
#include "math/quat.hpp"
#include "math/vec.hpp"

namespace wmoge {

    /**
     * @class LuaTypeTraits
     * @brief Utility class to simplify common engine primitive types marshaling
     */
    class LuaTypeTraits {
    public:
        static Strid             to_sid(luabridge::LuaRef& ref);
        static Var               to_var(luabridge::LuaRef& ref);
        static luabridge::LuaRef from_object(lua_State* state, Object* object);
        static luabridge::LuaRef from_var(lua_State* state, const Var& var);
    };

}// namespace wmoge

#endif//WMOGE_LUA_TYPE_TRAITS_HPP
