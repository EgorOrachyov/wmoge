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
#include "rtti/class.hpp"

namespace wmoge {

    /**
     * @class RttiObject
     * @brief Base class for any engine object which has class information and full rtti support.
    */
    class RttiObject : public RefCnt {
    public:
        ~RttiObject() override = default;

        virtual Status clone(Ref<RttiObject>& object) const;

        virtual Ref<RttiObject> duplicate() const;
        virtual Strid           get_class_name() const;
        virtual Strid           get_parent_class_name() const;
        virtual RttiClass*      get_class() const;
        virtual RttiClass*      get_parent_class() const;

        static Strid      get_class_name_static();
        static Strid      get_parent_class_name_static();
        static RttiClass* get_class_static();
        static RttiClass* get_parent_class_static();

        static Status yaml_read_object(YamlConstNodeRef node, Ref<Object>& object);
        static Status yaml_write_object(YamlNodeRef node, const Ref<Object>& object);
        static Status archive_read_object(Archive& archive, Ref<Object>& object);
        static Status archive_write_object(Archive& archive, const Ref<Object>& object);
    };

}// namespace wmoge