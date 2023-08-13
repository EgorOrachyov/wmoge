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
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "io/archive.hpp"
#include "io/yaml.hpp"

#include <list>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace wmoge {

    /**
     * @class Object
     * @brief Base class for any scene tree object or resource with reflection support
     *
     * Object is a thin as possible class which comes with only thread-safe reference
     * counting and single virtual table pointer overhead. The object provides and interface
     * for common methods, which may be required to manipulate objects and write generalized
     * runtime code.
     *
     * The main feature of the object is the runtime type information support (or reflection).
     * It is possible to get object class, query its inheritance tree, get all methods,
     * properties and fields. Object can be used for more simple automated serialization
     * and deserialization, viewing in structure in the inspector and so on.
     */
    class Object : public RefCnt {
    public:
        ~Object() override = default;

        virtual std::string to_string() { return ""; }
        virtual std::size_t hash() { return 0; }

        virtual Status set(const StringId& property, const Var& value);
        virtual Status get(const StringId& property, Var& value);
        virtual Status call(const StringId& method, int argc, const Var* argv, Var& ret);
        virtual Status signal(const StringId& signal) { return StatusCode::Ok; }
        virtual Status copy_to(Object& other) const { return StatusCode::Ok; }
        virtual Status read_from_yaml(const YamlConstNodeRef& node) { return StatusCode::NotImplemented; }
        virtual Status write_to_yaml(YamlNodeRef node) const { return StatusCode::NotImplemented; }
        virtual Status clone(Ref<Object>& object) const;

        virtual Ref<Object> duplicate() const;

        virtual const class Class* class_ptr() const;
        virtual const StringId&    class_name() const;
        virtual const StringId&    super_class_name() const;

        static const class Class* class_ptr_static();
        static const StringId&    class_name_static();
        static const StringId&    super_class_name_static();

        friend Status yaml_read_object(const YamlConstNodeRef& node, Ref<Object>& object);
        friend Status yaml_write_object(YamlNodeRef node, const Ref<Object>& object);
    };

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, Object*>>* = 0) {
        Ref<Object> object;
        auto        status = yaml_read_object(node, object);
        if (!status) return status;
        ref = object.template cast<T>();
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_read(const YamlConstNodeRef& node, Ref<T>& ref, typename std::enable_if_t<!std::is_convertible_v<T*, Object*>>* = 0) {
        ref = make_ref<T>();
        WG_YAML_READ(node, *ref);
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, Object*>>* = 0) {
        Ref<Object> object = ref.template as<Object>();
        auto        status = yaml_write_object(node, object);
        if (!status) return status;
        return StatusCode::Ok;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const Ref<T>& ref, typename std::enable_if_t<!std::is_convertible_v<T*, Object*>>* = 0) {
        assert(ref);
        WG_YAML_WRITE(node, *ref);
        return StatusCode::Ok;
    }

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
