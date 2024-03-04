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

#pragma once

#include "core/ref.hpp"
#include "core/status.hpp"
#include "core/string_id.hpp"
#include "core/var.hpp"
#include "io/archive.hpp"
#include "io/yaml.hpp"

#include <cassert>
#include <list>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace wmoge {

    /**
     * @class Object
     * @brief Base class for any scene tree object or resource with reflection support
     *
     * Object is a thin as possible class which comes with only thread-safe reference
     * counting and single virtual table pointer overhead. The object provides an interface
     * for common methods, which may be required to manipulate objects and write generalized
     * runtime code.
     *
     * The main feature of the object is the runtime type information support (or reflection).
     * It is possible to get object class, query its inheritance tree, get all methods,
     * properties and fields. Object can be used for more simple automated serialization
     * and deserialization, viewing its structure in the inspector and so on.
     */
    class Object : public RefCnt {
    public:
        ~Object() override = default;

        virtual std::string to_string() { return ""; }
        virtual std::size_t hash() { return 0; }

        virtual Status set(const Strid& property, const Var& value);
        virtual Status get(const Strid& property, Var& value);
        virtual Status call(const Strid& method, int argc, const Var* argv, Var& ret);
        virtual Status signal(const Strid& signal) { return StatusCode::Ok; }
        virtual Status copy_to(Object& other) const { return StatusCode::Ok; }
        virtual Status read_from_yaml(const YamlConstNodeRef& node) { return StatusCode::NotImplemented; }
        virtual Status write_to_yaml(YamlNodeRef node) const { return StatusCode::NotImplemented; }
        virtual Status read_from_archive(Archive& archive) { return StatusCode::NotImplemented; }
        virtual Status write_to_archive(Archive& archive) const { return StatusCode::NotImplemented; }
        virtual Status clone(Ref<Object>& object) const;

        virtual Ref<Object> duplicate() const;

        virtual const class Class* class_ptr() const;
        virtual const Strid&       class_name() const;
        virtual const Strid&       super_class_name() const;

        static const class Class* class_ptr_static();
        static const Strid&       class_name_static();
        static const Strid&       super_class_name_static();

        friend Status yaml_read_object(const YamlConstNodeRef& node, Ref<Object>& object);
        friend Status yaml_write_object(YamlNodeRef node, const Ref<Object>& object);

        friend Status archive_read_object(Archive& archive, Ref<Object>& object);
        friend Status archive_write_object(Archive& archive, const Ref<Object>& object);
    };

    template<typename T>
    Status copy_objects(const std::vector<Ref<T>>& objects, std::vector<Ref<T>>& copied) {
        static_assert(std::is_base_of_v<Object, T>, "T must be sub-class of object");

        copied.reserve(objects.size());

        for (auto& object : objects) {
            Ref<Object> copy;

            if (!object->clone(copy)) {
                WG_LOG_ERROR("failed to clone object " << object->class_name());
                return StatusCode::Error;
            }

            auto as_t = copy.cast<T>();
            assert(as_t);

            copied.push_back(as_t);
        }

        return StatusCode::Ok;
    }

    template<typename T>
    std::vector<Ref<T>> copy_objects(const std::vector<Ref<T>>& objects) {
        std::vector<Ref<T>> result;

        if (!copy_objects(objects, result)) {
            WG_LOG_ERROR("failed to copy objects vector type: " << T::class_name_static() << " count: " << objects.size());
            return {};
        }

        return std::move(result);
    }

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

    template<typename T>
    Status archive_read(Archive& archive, Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, Object*>>* = 0) {
        Ref<Object> object;
        auto        status = archive_read_object(archive, object);
        if (!status) return status;
        ref = object.template cast<T>();
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_read(Archive& archive, Ref<T>& ref, typename std::enable_if_t<!std::is_convertible_v<T*, Object*>>* = 0) {
        ref = make_ref<T>();
        WG_ARCHIVE_READ(archive, *ref);
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_write(Archive& archive, const Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, Object*>>* = 0) {
        Ref<Object> object = ref.template as<Object>();
        auto        status = archive_write_object(archive, object);
        if (!status) return status;
        return StatusCode::Ok;
    }

    template<typename T>
    Status archive_write(Archive& archive, const Ref<T>& ref, typename std::enable_if_t<!std::is_convertible_v<T*, Object*>>* = 0) {
        assert(ref);
        WG_ARCHIVE_WRITE(archive, *ref);
        return StatusCode::Ok;
    }

#define WG_OBJECT(name, super)                                                                        \
public:                                                                                               \
    friend class Class;                                                                               \
    name()           = default;                                                                       \
    ~name() override = default;                                                                       \
    static void               register_class();                                                       \
    const class Class*        class_ptr() const override { return class_ptr_static(); }               \
    const Strid&              class_name() const override { return class_name_static(); }             \
    const Strid&              super_class_name() const override { return super_class_name_static(); } \
    static const class Class* class_ptr_static() {                                                    \
        thread_local Class* cls = Class::class_ptr(class_name_static());                              \
        return cls;                                                                                   \
    }                                                                                                 \
    static const Strid& class_name_static() {                                                         \
        thread_local Strid sid = SID(#name);                                                          \
        return sid;                                                                                   \
    }                                                                                                 \
    static const Strid& super_class_name_static() {                                                   \
        return super::class_name_static();                                                            \
    }

}// namespace wmoge