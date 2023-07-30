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

#include "object.hpp"
#include "class.hpp"
#include "engine.hpp"

#include <cassert>

namespace wmoge {

    int Object::set(const StringId& property, const Var& value) {
        const Class* cls = class_ptr();
        assert(cls);

        const ClassProperty* prop = cls->property(property);
        if (!prop || !prop->has_setter()) {
            WG_LOG_ERROR("no property with setter: " << property);
            return 1;
        }

        const ClassMethod* setter = cls->method(prop->setter());
        if (!setter || setter->args_count() < 1) {
            WG_LOG_ERROR("no setter method to invoke: " << prop->setter());
            return 1;
        }

        Var dummy;
        return setter->call(this, 1, &value, dummy);
    }
    int Object::get(const StringId& property, Var& value) {
        const Class* cls = class_ptr();
        assert(cls);

        const ClassProperty* prop = cls->property(property);
        if (!prop || !prop->has_getter()) {
            WG_LOG_ERROR("no property with getter: " << property);
            return 1;
        }

        const ClassMethod* getter = cls->method(prop->getter());
        if (!getter || !getter->has_ret()) {
            WG_LOG_ERROR("no getter method to invoke: " << prop->setter());
            return 1;
        }

        return getter->call(this, 0, nullptr, value);
    }
    int Object::call(const StringId& method, int argc, const Var* argv, Var& ret) {
        const Class* cls = class_ptr();
        assert(cls);

        const ClassMethod* callable = cls->method(method);
        if (!callable) {
            WG_LOG_ERROR("no method to call: " << method);
            return 1;
        }

        return callable->call(this, argc, argv, ret);
    }

    const Class* Object::class_ptr() const {
        return class_ptr_static();
    }
    const StringId& Object::class_name() const {
        return class_name_static();
    }
    const StringId& Object::super_class_name() const {
        return super_class_name_static();
    }
    const Class* Object::class_ptr_static() {
        return Class::class_ptr(class_name_static());
    }
    const StringId& Object::class_name_static() {
        static StringId name = SID("Object");
        return name;
    }
    const StringId& Object::super_class_name_static() {
        static StringId name = SID("");
        return name;
    }

}// namespace wmoge
