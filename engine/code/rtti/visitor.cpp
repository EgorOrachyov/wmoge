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

#include "visitor.hpp"

#include "rtti/object.hpp"

namespace wmoge {

    Status RttiVisitor::visit(const RttiType* rtti, std::uint8_t* src) {
        switch (rtti->get_archetype()) {
            case RttiArchetype::Fundamental:
                return visit_fundamental(rtti, src);
            case RttiArchetype::Optional:
                return visit_optional((const RttiTypeOptional*) rtti, src);
            case RttiArchetype::Pair:
                return visit_pair((const RttiTypePair*) rtti, src);
            case RttiArchetype::Ref:
                return visit_ref((const RttiTypeRef*) rtti, src);
            case RttiArchetype::AssetRef:
                return visit_asset_ref((const RttiTypeAssetRef*) rtti, src);
            case RttiArchetype::Vector:
                return visit_vector((const RttiTypeVector*) rtti, src);
            case RttiArchetype::Set:
                return visit_set((const RttiTypeSet*) rtti, src);
            case RttiArchetype::Map:
                return visit_map((const RttiTypeMap*) rtti, src);
            case RttiArchetype::Struct:
                return visit_struct((const RttiStruct*) rtti, src);
            case RttiArchetype::Class:
                return visit_class((const RttiClass*) rtti, src);
            case RttiArchetype::Vec:
                return visit_vec((const RttiTypeVec*) rtti, src);
            case RttiArchetype::Mask:
                return visit_mask((const RttiTypeMask*) rtti, src);
            case RttiArchetype::Bitset:
                return visit_bitset((const RttiTypeBitset*) rtti, src);
            case RttiArchetype::Function:
                return visit_function((const RttiTypeFunction*) rtti, src);
            case RttiArchetype::Enum:
                return visit_enum((const RttiEnum*) rtti, src);
            default:
                return WG_OK;
        }
    }

    Status RttiVisitor::visit_optional(const RttiTypeOptional* rtti, std::uint8_t* src) {
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->visit(src, [&](const void* value) {
            return visit(value_type, (std::uint8_t*) value);
        }));
        return WG_OK;
    }

    Status RttiVisitor::visit_pair(const RttiTypePair* rtti, std::uint8_t* src) {
        const RttiType* key_type   = rtti->get_key_type();
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->visit(src, [&](const void* key, const void* value) {
            WG_CHECKED(visit(key_type, (std::uint8_t*) key));
            WG_CHECKED(visit(value_type, (std::uint8_t*) value));
            return WG_OK;
        }));
        return WG_OK;
    }

    Status RttiVisitor::visit_struct(const RttiStruct* rtti, std::uint8_t* src) {
        for (const RttiField& field : rtti->get_fields()) {
            const RttiType* field_type   = field.get_type();
            std::size_t     field_offset = field.get_byte_offset();

            WG_CHECKED(visit(field_type, src + field_offset));
        }
        return WG_OK;
    }

    Status RttiVisitor::visit_class(const RttiClass* rtti, std::uint8_t* src) {
        for (const RttiField& field : rtti->get_fields()) {
            const RttiType* field_type   = field.get_type();
            std::size_t     field_offset = field.get_byte_offset();

            WG_CHECKED(visit(field_type, src + field_offset));
        }
        return WG_OK;
    }

    Status RttiVisitor::visit_vector(const RttiTypeVector* rtti, std::uint8_t* src) {
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->iterate(src, [&](const void* elem) {
            return visit(value_type, (std::uint8_t*) elem);
        }));
        return WG_OK;
    }

    Status RttiVisitor::visit_set(const RttiTypeSet* rtti, std::uint8_t* src) {
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->iterate(src, [&](const void* elem) {
            return visit(value_type, (std::uint8_t*) elem);
        }));
        return WG_OK;
    }

    Status RttiVisitor::visit_map(const RttiTypeMap* rtti, std::uint8_t* src) {
        const RttiType* key_type   = rtti->get_key_type();
        const RttiType* value_type = rtti->get_value_type();
        WG_CHECKED(rtti->iterate(src, [&](const void* key, const void* value) {
            WG_CHECKED(visit(key_type, (std::uint8_t*) key));
            WG_CHECKED(visit(value_type, (std::uint8_t*) value));
            return WG_OK;
        }));
        return WG_OK;
    }

    Status RttiVisitor::visit_ref(const RttiTypeRef* rtti, std::uint8_t* src) {
        const Ref<RttiObject>& as_ptr = *((Ref<RttiObject>*) src);
        if (!as_ptr) {
            return WG_OK;
        }
        return visit(as_ptr->get_class(), (std::uint8_t*) as_ptr.get());
    }

    Status RttiVisitor::visit_asset_ref(const RttiTypeAssetRef* rtti, std::uint8_t* src) {
        const Ref<RttiObject>& as_ptr = *((Ref<RttiObject>*) src);
        if (!as_ptr) {
            return WG_OK;
        }
        return visit(as_ptr->get_class(), (std::uint8_t*) as_ptr.get());
    }

    Status RttiVisitor::visit_vec(const RttiTypeVec* rtti, std::uint8_t* src) {
        return WG_OK;
    }

    Status RttiVisitor::visit_mask(const RttiTypeMask* rtti, std::uint8_t* src) {
        return WG_OK;
    }

    Status RttiVisitor::visit_bitset(const RttiTypeBitset* rtti, std::uint8_t* src) {
        return WG_OK;
    }

    Status RttiVisitor::visit_function(const RttiTypeFunction* rtti, std::uint8_t* src) {
        return WG_OK;
    }

    Status RttiVisitor::visit_enum(const RttiEnum* rtti, std::uint8_t* src) {
        return WG_OK;
    }

    Status RttiVisitor::visit_fundamental(const RttiType* rtti, std::uint8_t* src) {
        return WG_OK;
    }

}// namespace wmoge