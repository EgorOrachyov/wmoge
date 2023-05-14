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

#include "class.hpp"

#include "core/object.hpp"

#include "components/register_classes_components.hpp"
#include "event/register_classes_event.hpp"
#include "pfx/register_classes_pfx.hpp"
#include "resource/register_classes_resource.hpp"
#include "scene/register_classes_scene.hpp"

namespace wmoge {

    Property::Property(VarType type, StringId name, StringId getter, StringId setter)
        : m_name(name), m_getter(getter), m_setter(setter), m_type(type) {
    }

    Method::Method(VarType ret, StringId name, std::vector<StringId> args)
        : m_args_names(std::move(args)), m_name(name), m_ret(ret) {
    }
    int Method::call(Object* object, int argc, const Var* argv, Var& ret) const {
        return m_callable(*this, object, argc, argv, ret);
    }

    const Class* Class::super() const {
        return class_ptr(super_name());
    }
    const Property* Class::property(const StringId& name) const {
        auto query = m_properties.find(name);
        return query != m_properties.end() ? &query->second : nullptr;
    }
    const Method* Class::method(const StringId& name) const {
        auto query = m_methods.find(name);
        return query != m_methods.end() ? &query->second : nullptr;
    }

    Ref<Object> Class::instantiate() const {
        if (!m_instantiate) {
            WG_LOG_ERROR("no function to instantiate " << name());
            return Ref<Object>();
        }
        return Ref<Object>(m_instantiate());
    }

    bool Class::is_inherited_from(const StringId& name) const {
        return m_supers.find(name) != m_supers.end();
    }

    Class* Class::class_ptr(StringId name) {
        ClassDB* db    = class_db();
        auto     query = db->find(name);
        return query != db->end() ? query->second.get() : nullptr;
    }
    Class* Class::register_class(const StringId& name, const StringId& super, std::size_t size, std::function<Object*()> instantiate) {
        if (!class_ptr(super)) {
            WG_LOG_ERROR("no such supper class: " << super << " registered");
            return nullptr;
        }
        if (auto cls = class_ptr(name)) {
            WG_LOG_ERROR("class: " << name << " already registered");
            return cls;
        }
        ClassDB& db        = *class_db();
        db[name]           = std::make_unique<Class>();
        Class* cls         = db[name].get();
        Class* super_cls   = db[super].get();
        cls->m_name        = name;
        cls->m_super_name  = super;
        cls->m_size        = size;
        cls->m_instantiate = std::move(instantiate);
        cls->m_properties  = super_cls->m_properties;
        cls->m_methods     = super_cls->m_methods;
        cls->m_supers      = super_cls->m_supers;
        cls->m_supers.emplace(name);
        return cls;
    }
    ClassDB* Class::class_db() {
        static ClassDB classDb;
        return &classDb;
    }

    Class* Class::add_property(Property property) {
        m_properties.emplace(property.name(), std::move(property));
        return this;
    }

    void Class::register_types() {
        ClassDB& db                     = *class_db();
        db[Object::class_name_static()] = std::make_unique<Class>();
        Class* cls                      = db[Object::class_name_static()].get();

        cls->m_name        = SID("Object");
        cls->m_size        = sizeof(Object);
        cls->m_instantiate = []() { return new Object(); };
        cls->m_supers.emplace(Object::class_name_static());
        cls->add_method(Method(VarType::Int, SID("hash"), {}), &Object::hash, {});
        cls->add_method(Method(VarType::String, SID("to_string"), {}), &Object::to_string, {});

        register_classes_event();
        register_classes_resource();
        register_classes_pfx();
        register_classes_scene();
        register_classes_components();
    }

}// namespace wmoge
