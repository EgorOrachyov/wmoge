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

#ifndef WMOGE_LUA_BINDINGS_PLATFORM_HPP
#define WMOGE_LUA_BINDINGS_PLATFORM_HPP

#include "scripting/lua_bindings/lua_bindings.hpp"
#include "scripting/lua_bindings/lua_bindings_core.hpp"

#include "core/engine.hpp"
#include "platform/file_system.hpp"
#include "platform/input.hpp"
#include "platform/input_devices.hpp"
#include "platform/window.hpp"
#include "platform/window_manager.hpp"

namespace wmoge {

    struct LuaWindow : public LuaRefCnt {
        void close() {
            cast_unsafe<Window>()->close();
        }
        int width() const {
            return cast_unsafe<Window>()->width();
        }
        int height() const {
            return cast_unsafe<Window>()->height();
        }
        int fbo_width() const {
            return cast_unsafe<Window>()->fbo_width();
        }
        int fbo_height() const {
            return cast_unsafe<Window>()->fbo_height();
        }
        float scale_x() const {
            return cast_unsafe<Window>()->scale_x();
        }
        float scale_y() const {
            return cast_unsafe<Window>()->scale_y();
        }
        bool in_focus() const {
            return cast_unsafe<Window>()->in_focus();
        }
        const StringId& id() const {
            return cast_unsafe<Window>()->id();
        }
        const std::string& title() const {
            return cast_unsafe<Window>()->title();
        }
    };

    struct LuaInputDevice : public LuaRefCnt {
    };

    struct LuaMouse : public LuaInputDevice {
    };

    struct LuaKeyboard : public LuaInputDevice {
    };

    struct LuaJoystick : public LuaInputDevice {
    };

}// namespace wmoge

#endif//WMOGE_LUA_BINDINGS_PLATFORM_HPP
