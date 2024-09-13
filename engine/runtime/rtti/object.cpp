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

#include "rtti/type_storage.hpp"

namespace wmoge {

    std::string RttiObject::to_string() const {
        RttiClass*        rtti_class = get_class();
        std::stringstream stream;
        rtti_class->to_string(this, stream);
        return stream.str();
    }
    Status RttiObject::clone(Ref<RttiObject>& object) const {
        RttiClass* rtti_class = get_class();
        object                = rtti_class->instantiate();
        return rtti_class->copy(object.get(), this);
    }
    Status RttiObject::read_from_yaml(IoContext& context, YamlConstNodeRef node) {
        return get_class()->read_from_yaml(this, node, context);
    }
    Status RttiObject::write_to_yaml(IoContext& context, YamlNodeRef node) const {
        return get_class()->write_to_yaml(this, node, context);
    }
    Status RttiObject::read_from_stream(IoContext& context, IoStream& stream) {
        return get_class()->read_from_stream(this, stream, context);
    }
    Status RttiObject::write_to_stream(IoContext& context, IoStream& stream) const {
        return get_class()->write_to_stream(this, stream, context);
    }
    Ref<RttiObject> RttiObject::duplicate() const {
        Ref<RttiObject> object;
        clone(object);
        return object;
    }
    Strid RttiObject::get_class_name() const {
        return get_class_name_static();
    }
    Strid RttiObject::get_parent_class_name() const {
        return get_parent_class_name_static();
    }
    RttiClass* RttiObject::get_class() const {
        return get_class_static();
    }
    RttiClass* RttiObject::get_parent_class() const {
        return get_parent_class_static();
    }

    Strid RttiObject::get_class_name_static() {
        static Strid g_class_name("RttiObject");
        return g_class_name;
    }
    Strid RttiObject::get_parent_class_name_static() {
        static Strid g_class_name;
        return g_class_name;
    }
    RttiClass* RttiObject::get_class_static() {
        static RttiClass* g_class = RttiTypeStorage::instance()->find_class(get_class_name_static());
        return g_class;
    }
    RttiClass* RttiObject::get_parent_class_static() {
        static RttiClass* g_class = nullptr;
        return g_class;
    }
    Status RttiObject::yaml_read_object(IoContext& context, YamlConstNodeRef node, Ref<RttiObject>& object) {
        assert(!object);

        if (node.empty()) {
            return WG_OK;
        }

        Strid rtti_name;
        WG_YAML_READ_AS(context, node, "rtti", rtti_name);

        RttiClass* rtti_class = context.get_type_storage()->find_class(rtti_name);
        if (!rtti_class) {
            WG_LOG_ERROR("no such class to read from yaml " << rtti_name);
            return StatusCode::NoClass;
        }

        object = rtti_class->instantiate();
        if (!object) {
            WG_LOG_ERROR("failed to instantiate class " << rtti_name);
            return StatusCode::FailedInstantiate;
        }

        return object->read_from_yaml(context, node);
    }
    Status RttiObject::yaml_write_object(IoContext& context, YamlNodeRef node, const Ref<RttiObject>& object) {
        if (!object) {
            return WG_OK;
        }
        WG_YAML_MAP(node);
        WG_YAML_WRITE_AS(context, node, "rtti", object->get_class_name());
        return object->write_to_yaml(context, node);
    }
    Status RttiObject::archive_read_object(IoContext& context, IoStream& stream, Ref<RttiObject>& object) {
        assert(!object);

        bool has_value;
        WG_ARCHIVE_READ(context, stream, has_value);

        if (!has_value) {
            return WG_OK;
        }

        Strid rtti_name;
        WG_ARCHIVE_READ(context, stream, rtti_name);

        RttiClass* rtti_class = context.get_type_storage()->find_class(rtti_name);
        if (!rtti_class) {
            WG_LOG_ERROR("no such class to read from stream " << rtti_name);
            return StatusCode::NoClass;
        }

        object = rtti_class->instantiate();
        if (!object) {
            WG_LOG_ERROR("failed to instantiate class " << rtti_name);
            return StatusCode::FailedInstantiate;
        }

        return object->read_from_stream(context, stream);
    }
    Status RttiObject::archive_write_object(IoContext& context, IoStream& stream, const Ref<RttiObject>& object) {
        const bool has_value = object;
        WG_ARCHIVE_WRITE(context, stream, has_value);
        if (!has_value) {
            return WG_OK;
        }
        WG_ARCHIVE_WRITE(context, stream, object->get_class_name());
        return object->write_to_stream(context, stream);
    }

}// namespace wmoge