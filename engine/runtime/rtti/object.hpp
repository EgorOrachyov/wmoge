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

        virtual std::string     to_string() const;
        virtual Status          clone(Ref<RttiObject>& object) const;
        virtual Status          read_from_tree(IoContext& context, IoPropertyTree& tree);
        virtual Status          write_to_tree(IoContext& context, IoPropertyTree& tree) const;
        virtual Status          read_from_stream(IoContext& context, IoStream& stream);
        virtual Status          write_to_stream(IoContext& context, IoStream& stream) const;
        virtual Ref<RttiObject> duplicate() const;
        virtual Strid           get_class_name() const;
        virtual Strid           get_parent_class_name() const;
        virtual RttiClass*      get_class() const;
        virtual RttiClass*      get_parent_class() const;

        static Strid      get_class_name_static();
        static Strid      get_parent_class_name_static();
        static RttiClass* get_class_static();
        static RttiClass* get_parent_class_static();
        static Status     tree_read_object(IoContext& context, IoPropertyTree& tree, Ref<RttiObject>& object);
        static Status     tree_write_object(IoContext& context, IoPropertyTree& tree, const Ref<RttiObject>& object);
        static Status     archive_read_object(IoContext& context, IoStream& stream, Ref<RttiObject>& object);
        static Status     archive_write_object(IoContext& context, IoStream& stream, const Ref<RttiObject>& object);
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

        return WG_OK;
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
    Status tree_read(IoContext& context, IoPropertyTree& tree, Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object;
        WG_CHECKED(RttiObject::tree_read_object(context, tree, object));
        ref = object.template cast<T>();
        return WG_OK;
    }

    template<typename T>
    Status tree_write(IoContext& context, IoPropertyTree& tree, const Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object = ref.template as<RttiObject>();
        WG_CHECKED(RttiObject::tree_write_object(context, tree, object));
        return WG_OK;
    }

    template<typename T>
    Status stream_read(IoContext& context, IoStream& stream, Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object;
        WG_CHECKED(RttiObject::archive_read_object(context, stream, object));
        ref = object.template cast<T>();
        return WG_OK;
    }

    template<typename T>
    Status stream_write(IoContext& context, IoStream& stream, const Ref<T>& ref, typename std::enable_if_t<std::is_convertible_v<T*, RttiObject*>>* = 0) {
        Ref<RttiObject> object = ref.template as<RttiObject>();
        WG_CHECKED(RttiObject::archive_write_object(context, stream, object));
        return WG_OK;
    }

}// namespace wmoge