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
#include "io/serialization.hpp"
#include "rtti/class.hpp"

namespace wmoge {

    /**
     * @class RttiObject
     * @brief Base class for any engine object which has class information and full rtti support.
    */
    class RttiObject : public RefCnt {
    public:
        ~RttiObject() override = default;

        virtual Status          clone(Ref<RttiObject>& object) const;
        virtual Status          read_from_yaml(YamlConstNodeRef node);
        virtual Status          write_to_yaml(YamlNodeRef node) const;
        virtual Status          read_from_archive(Archive& archive);
        virtual Status          write_to_archive(Archive& archive) const;
        virtual Ref<RttiObject> duplicate() const;
        virtual Strid           get_class_name() const;
        virtual Strid           get_parent_class_name() const;
        virtual RttiClass*      get_class() const;
        virtual RttiClass*      get_parent_class() const;

        static Strid      get_class_name_static();
        static Strid      get_parent_class_name_static();
        static RttiClass* get_class_static();
        static RttiClass* get_parent_class_static();
        static Status     yaml_read_object(YamlConstNodeRef node, Ref<RttiObject>& object);
        static Status     yaml_write_object(YamlNodeRef node, const Ref<RttiObject>& object);
        static Status     archive_read_object(Archive& archive, Ref<RttiObject>& object);
        static Status     archive_write_object(Archive& archive, const Ref<RttiObject>& object);
    };

    template<typename T, typename std::enable_if<std::is_convertible_v<T*, RttiObject*>>::type>
    Status copy_objects(const std::vector<Ref<T>>& objects, std::vector<Ref<T>>& copied) {
        static_assert(std::is_base_of_v<RttiObject, T>, "T must be sub-class of object");

        copied.reserve(objects.size());

        for (auto& object : objects) {
            Ref<RttiObject> copy;

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

    template<typename T, typename std::enable_if<std::is_convertible_v<T*, RttiObject*>>::type>
    std::vector<Ref<T>> copy_objects(const std::vector<Ref<T>>& objects) {
        std::vector<Ref<T>> result;

        if (!copy_objects(objects, result)) {
            WG_LOG_ERROR("failed to copy objects vector type: " << T::get_class_name_static() << " count: " << objects.size());
            return {};
        }

        return std::move(result);
    }

    template<typename T>
    Status yaml_read(YamlConstNodeRef node, Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object;
        WG_CHECKED(RttiObject::yaml_read_object(node, object));
        ref = object.template cast<T>();
        return WG_OK;
    }

    template<typename T>
    Status yaml_write(YamlNodeRef node, const Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object = ref.template as<RttiObject>();
        WG_CHECKED(RttiObject::yaml_write_object(node, object));
        return WG_OK;
    }

    template<typename T>
    Status archive_read(Archive& archive, Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object;
        WG_CHECKED(RttiObject::archive_read_object(archive, object));
        ref = object.template cast<T>();
        return WG_OK;
    }

    template<typename T>
    Status archive_write(Archive& archive, const Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object = ref.template as<RttiObject>();
        WG_CHECKED(RttiObject::archive_write_object(archive, object));
        return WG_OK;
    }

}// namespace wmoge