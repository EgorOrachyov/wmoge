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

    Status Object::set(const Strid& property, const Var& value) {
        const Class* cls = class_ptr();
        assert(cls);

        const ClassProperty* prop = cls->property(property);
        if (!prop || !prop->has_setter()) {
            WG_LOG_ERROR("no property with setter: " << property);
            return StatusCode::NoProperty;
        }

        const ClassMethod* setter = cls->method(prop->setter());
        if (!setter || setter->args_count() < 1) {
            WG_LOG_ERROR("no setter method to invoke: " << prop->setter());
            return StatusCode::NoMethod;
        }

        Var dummy;
        return setter->call(this, 1, &value, dummy);
    }
    Status Object::get(const Strid& property, Var& value) {
        const Class* cls = class_ptr();
        assert(cls);

        const ClassProperty* prop = cls->property(property);
        if (!prop || !prop->has_getter()) {
            WG_LOG_ERROR("no property with getter: " << property);
            return StatusCode::NoProperty;
        }

        const ClassMethod* getter = cls->method(prop->getter());
        if (!getter || !getter->has_ret()) {
            WG_LOG_ERROR("no getter method to invoke: " << prop->setter());
            return StatusCode::NoMethod;
        }

        return getter->call(this, 0, nullptr, value);
    }
    Status Object::call(const Strid& method, int argc, const Var* argv, Var& ret) {
        const Class* cls = class_ptr();
        assert(cls);

        const ClassMethod* callable = cls->method(method);
        if (!callable) {
            WG_LOG_ERROR("no method to call: " << method);
            return StatusCode::NoMethod;
        }

        return callable->call(this, argc, argv, ret);
    }
    Status Object::clone(Ref<Object>& object) const {
        const Class* cls = class_ptr();

        object = cls->instantiate();

        if (!object) {
            WG_LOG_ERROR("failed to instantiate class " << cls->name());
            return StatusCode::Error;
        }

        return copy_to(*object);
    }
    Ref<Object> Object::duplicate() const {
        Ref<Object> result;

        clone(result);

        return result;
    }

    const Class* Object::class_ptr() const {
        return class_ptr_static();
    }
    const Strid& Object::class_name() const {
        return class_name_static();
    }
    const Strid& Object::super_class_name() const {
        return super_class_name_static();
    }
    const Class* Object::class_ptr_static() {
        return Class::class_ptr(class_name_static());
    }
    const Strid& Object::class_name_static() {
        static Strid name = SID("Object");
        return name;
    }
    const Strid& Object::super_class_name_static() {
        static Strid name = SID("");
        return name;
    }

    Status yaml_read_object(const YamlConstNodeRef& node, Ref<Object>& object) {
        assert(!object);

        Strid class_name;
        WG_YAML_READ_AS(node, "rtti", class_name);

        auto* cls = Class::class_ptr(class_name);

        if (!cls) {
            WG_LOG_ERROR("no such class to read from yaml " << class_name);
            return StatusCode::NoClass;
        }

        object = cls->instantiate();

        if (!object) {
            WG_LOG_ERROR("failed to instantiate class " << class_name);
            return StatusCode::FailedInstantiate;
        }

        return object->read_from_yaml(node);
    }
    Status yaml_write_object(YamlNodeRef node, const Ref<Object>& object) {
        assert(object);
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(node, "rtti", object->class_name());
        return object->write_to_yaml(node);
    }

    Status archive_read_object(Archive& archive, Ref<Object>& object) {
        assert(!object);

        Strid class_name;
        WG_ARCHIVE_READ(archive, class_name);

        auto* cls = Class::class_ptr(class_name);

        if (!cls) {
            WG_LOG_ERROR("no such class to read from archive " << class_name);
            return StatusCode::NoClass;
        }

        object = cls->instantiate();

        if (!object) {
            WG_LOG_ERROR("failed to instantiate class " << class_name);
            return StatusCode::FailedInstantiate;
        }

        return object->read_from_archive(archive);
    }
    Status archive_write_object(Archive& archive, const Ref<Object>& object) {
        assert(object);
        WG_ARCHIVE_WRITE(archive, object->class_name());
        return object->write_to_archive(archive);
    }

}// namespace wmoge
