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

#ifndef WMOGE_LUA_BINDINGS_CORE_HPP
#define WMOGE_LUA_BINDINGS_CORE_HPP

#include "scripting/lua_bindings/lua_bindings.hpp"

#include "core/cmd_line.hpp"
#include "core/data.hpp"
#include "core/engine.hpp"
#include "core/log.hpp"
#include "core/object.hpp"
#include "core/random.hpp"
#include "core/ref.hpp"
#include "event/event.hpp"

#include <magic_enum.hpp>

namespace wmoge {

    struct LuaRefCnt {
        bool is_null() const {
            return !ptr.operator bool();
        }
        unsigned int ref() {
            return ptr->ref();
        }
        unsigned int unref() {
            return ptr->unref();
        }
        unsigned int refs_count() const {
            return ptr->refs_count();
        }

        void set(Ref<RefCnt> new_ptr) {
            ptr = std::move(new_ptr);
        }
        Ref<RefCnt> get() const {
            return ptr;
        };

        template<typename T>
        T* cast() const {
            return dynamic_cast<T*>(ptr.get());
        };

        template<typename T>
        T* cast_unsafe() const {
            return reinterpret_cast<T*>(ptr.get());
        };

        Ref<RefCnt> ptr;
    };

    struct LuaData : public LuaRefCnt {
        std::size_t size() const {
            return cast_unsafe<Data>()->size();
        }
        std::size_t size_as_kib() const {
            return cast_unsafe<Data>()->size_as_kib();
        }
        std::string to_string() const {
            return cast_unsafe<Data>()->to_string();
        }
    };

    struct LuaObject : public LuaRefCnt {
        std::string to_string() const {
            return cast_unsafe<Object>()->to_string();
        }
        const StringId& class_name() const {
            return cast_unsafe<Object>()->class_name();
        }
        void signal(const StringId& signal) {
            cast_unsafe<Object>()->signal(signal);
        }
    };

}// namespace wmoge

#endif//WMOGE_LUA_BINDINGS_CORE_HPP
