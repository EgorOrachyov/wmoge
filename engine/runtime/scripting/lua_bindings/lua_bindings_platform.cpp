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

#include "lua_bindings_platform.hpp"

namespace wmoge {

    static int lua_FileSystem_resolve(lua_State* state) {
        auto path = WG_LUA_ARG(state, 0);
        WG_LUA_CHECK_MSG(path.isString(), "expecting path to be a string");

        auto* file_system = Engine::instance()->file_system();

        // luabridge::LuaRef(state, file_system->resolve(path.cast<std::string>())).push();
        return 1;
    }

    static int lua_FileSystem_exists(lua_State* state) {
        auto path = WG_LUA_ARG(state, 0);
        WG_LUA_CHECK_MSG(path.isString(), "expecting path to be a string");

        auto* file_system = Engine::instance()->file_system();

        luabridge::LuaRef(state, file_system->exists(path.cast<std::string>())).push();
        return 1;
    }

    static int lua_FileSystem_read_file(lua_State* state) {
        auto path = WG_LUA_ARG(state, 0);
        auto data = WG_LUA_ARG(state, 1);
        WG_LUA_CHECK_MSG(path.isString(), "expecting path to be a string");

        auto* file_system = Engine::instance()->file_system();

        if (data.isInstance<LuaData>()) {
            Ref<Data> ret;
            bool      status            = file_system->read_file(path.cast<std::string>(), ret);
            data.cast<LuaObject&>().ptr = ret;
            luabridge::LuaRef(state, file_system->exists(path.cast<std::string>())).push();
            return 1;
        }

        WG_LOG_ERROR("invalid return argument where to store read result");

        return 0;
    }

    static int lua_WindowManager_primary_window(lua_State* state) {
        auto* window_manager = Engine::instance()->window_manager();

        luabridge::LuaRef(state, LuaWindow{window_manager->get_primary_window()}).push();
        return 1;
    }

    void LuaBindings::bind_platform(luabridge::Namespace& ns, flat_map<const Class*, LuaConvCppToLua>& mapping) {
        LuaBindings::bind_enum<InputDeviceType>(ns, "InputDeviceType");
        LuaBindings::bind_enum<InputDeviceState>(ns, "InputDeviceState");
        LuaBindings::bind_enum<InputAction>(ns, "InputAction");
        LuaBindings::bind_enum<InputModifier>(ns, "InputModifier");
        LuaBindings::bind_enum<InputMouseButton>(ns, "InputMouseButton");
        LuaBindings::bind_enum<InputKeyboardKey>(ns, "InputKeyboardKey");

        ns = ns.beginNamespace("FileSystem")
                     .addFunction("resolve", lua_FileSystem_resolve)
                     .addFunction("exists", lua_FileSystem_exists)
                     .addFunction("read_file", lua_FileSystem_read_file)
                     .endNamespace();

        ns = ns.beginNamespace("WindowManager")
                     .addFunction("primary_window", lua_WindowManager_primary_window)
                     .endNamespace();

        ns = ns.deriveClass<LuaWindow, LuaRefCnt>("Window")
                     .addFunction("close", &LuaWindow::close)
                     .addProperty("width", &LuaWindow::width)
                     .addProperty("height", &LuaWindow::height)
                     .addProperty("fbo_width", &LuaWindow::fbo_width)
                     .addProperty("fbo_height", &LuaWindow::fbo_height)
                     .addProperty("scale_x", &LuaWindow::scale_x)
                     .addProperty("scale_y", &LuaWindow::scale_y)
                     .addProperty("in_focus", &LuaWindow::in_focus)
                     .addProperty("id", &LuaWindow::id)
                     .addProperty("title", &LuaWindow::title)
                     .endClass();
    }

}// namespace wmoge