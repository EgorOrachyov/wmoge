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

    Status RttiObject::clone(Ref<RttiObject>& object) const {
        if (!object) {
            return StatusCode::InvalidParameter;
        }
        RttiClass* tclass = get_class();
        return tclass->copy(object.get(), this);
    }

    Ref<RttiObject> RttiObject::duplicate() const {
        RttiClass*      tclass = get_class();
        Ref<RttiObject> object(tclass->instantiate());
        if (!tclass->copy(object.get(), this)) {
            object.reset();
        }
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

    Status RttiObject::yaml_read_object(YamlConstNodeRef node, Ref<Object>& object) {
        return WG_OK;
    }
    Status RttiObject::yaml_write_object(YamlNodeRef node, const Ref<Object>& object) {
        return WG_OK;
    }
    Status RttiObject::archive_read_object(Archive& archive, Ref<Object>& object) {
        return WG_OK;
    }
    Status RttiObject::archive_write_object(Archive& archive, const Ref<Object>& object) {
        return WG_OK;
    }

}// namespace wmoge