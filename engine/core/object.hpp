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

#ifndef WMOGE_OBJECT_HPP
#define WMOGE_OBJECT_HPP

#include "core/ref.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"

#include <list>
#include <string>
#include <unordered_map>

namespace wmoge {

    /**
     * @class Object
     * @brief Base class for any scene object or resource with reflection support
     */
    class Object : public RefCnt {
    public:
        ~Object() override = default;

        virtual std::string        to_string() { return ""; }
        virtual std::size_t        hash() { return 0; }
        virtual int                set(const StringId& property, const Var& value);
        virtual int                get(const StringId& property, Var& value);
        virtual int                call(const StringId& method, int argc, const Var* argv, Var& ret);
        virtual int                signal(const StringId& signal) { return 0; }
        virtual const class Class* class_ptr() const;
        virtual const StringId&    class_name() const;
        virtual const StringId&    super_class_name() const;

        static const class Class* class_ptr_static();
        static const StringId&    class_name_static();
        static const StringId&    super_class_name_static();
    };

}// namespace wmoge

#define WG_OBJECT(name, super)                                                                        \
public:                                                                                               \
    friend class Class;                                                                               \
    name()           = default;                                                                       \
    ~name() override = default;                                                                       \
    static void               register_class();                                                       \
    const class Class*        class_ptr() const override { return class_ptr_static(); }               \
    const StringId&           class_name() const override { return class_name_static(); }             \
    const StringId&           super_class_name() const override { return super_class_name_static(); } \
    static const class Class* class_ptr_static() {                                                    \
        return Class::class_ptr(class_name_static());                                                 \
    }                                                                                                 \
    static const StringId& class_name_static() {                                                      \
        static StringId sid = SID(#name);                                                             \
        return sid;                                                                                   \
    }                                                                                                 \
    static const StringId& super_class_name_static() {                                                \
        return super::class_name_static();                                                            \
    }
#endif//WMOGE_OBJECT_HPP
