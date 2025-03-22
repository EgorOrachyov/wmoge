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

#include "ui_markup_xml_parser.hpp"

namespace wmoge {

    UiMarkupParser::UiMarkupParser(Strid name, UiMarkupDecs& desc, array_view<const std::uint8_t> xml_buffer, RttiTypeStorage* type_storage)
        : m_name(name), m_desc(desc), m_xml_buffer(xml_buffer), m_type_storage(type_storage) {
    }

    Status UiMarkupParser::parse() {
        if (m_document.Parse(reinterpret_cast<const char*>(m_xml_buffer.data()), static_cast<int>(m_xml_buffer.size()))) {
            WG_LOG_ERROR("failed to parse xml file " << m_name);
            return StatusCode::FailedParse;
        }

        auto* xml_root = m_document.FirstChildElement();
        if (!xml_root) {
            WG_LOG_ERROR("no root element in " << m_name);
            return StatusCode::InvalidData;
        }

        auto* xml_bindable = xml_root->FindAttribute("meta:bindable");
        if (!xml_bindable) {
            WG_LOG_ERROR("no bindable class specified " << m_name);
            return StatusCode::InvalidData;
        }

        m_desc.bindable = m_type_storage->find_class(SID(xml_bindable->Value()));
        if (!m_desc.bindable) {
            WG_LOG_ERROR("no such bindable class " << xml_bindable->Value());
            return StatusCode::InvalidData;
        }
        if (!m_desc.bindable->is_subtype_of(UiBindable::get_class_static())) {
            WG_LOG_ERROR("must be sub-class of bindable " << m_desc.bindable->get_name());
            return StatusCode::InvalidData;
        }

        UiMarkupElement root;
        WG_CHECKED(parse_element(xml_root, root));

        m_desc.root_element = static_cast<int>(m_desc.elements.size());
        m_desc.elements.push_back(std::move(root));

        return WG_OK;
    }

    Status UiMarkupParser::parse_element(tinyxml2::XMLElement* xml_node, UiMarkupElement& out) {
        out.cls = m_type_storage->find_class(SID(xml_node->Value()));
        if (!out.cls) {
            WG_LOG_ERROR("no such element class " << xml_node->Value());
            return StatusCode::InvalidData;
        }
        if (!out.cls->can_instantiate()) {
            WG_LOG_ERROR("such class cannot be instantiated " << xml_node->Value());
            return StatusCode::InvalidData;
        }

        for (auto xml_atribute = xml_node->FirstAttribute(); xml_atribute; xml_atribute = xml_atribute->Next()) {
            if (StringUtils::is_starts_with(xml_atribute->Name(), "meta:")) {
                continue;
            }

            UiMarkupAttribute attribute;
            WG_CHECKED(parse_attribute(xml_atribute, attribute, out.cls));

            out.attributes.push_back(static_cast<int>(m_desc.attributes.size()));
            m_desc.attributes.push_back(std::move(attribute));
        }

        const std::string ns_prefix    = std::string(xml_node->Value()) + ".";
        const bool        has_content  = out.cls->has_field(SID("content"));
        const bool        has_children = out.cls->has_field(SID("children"));

        if (has_content && has_children) {
            WG_LOG_ERROR("ambigues fields setup for " << out.cls->get_name());
            return StatusCode::InvalidData;
        }

        for (auto xml_slot = xml_node->FirstChildElement(); xml_slot; xml_slot = xml_slot->NextSiblingElement()) {
            std::string slot_name = xml_slot->Value();

            auto append_slot = [&](UiMarkupSlot& slot) {
                out.slots.push_back(static_cast<int>(m_desc.slots.size()));
                m_desc.slots.push_back(std::move(slot));
            };

            if (StringUtils::is_starts_with(slot_name, ns_prefix)) {
                slot_name = slot_name.substr(ns_prefix.length());
                for (auto xml_child = xml_slot->FirstChildElement(); xml_child; xml_child = xml_child->NextSiblingElement()) {
                    UiMarkupSlot slot;
                    WG_CHECKED(parse_slot(xml_child, slot_name, slot, out.cls));
                    append_slot(slot);
                }
            } else if (has_content) {
                slot_name = "content";
                UiMarkupSlot slot;
                WG_CHECKED(parse_slot(xml_slot, slot_name, slot, out.cls));
                append_slot(slot);
            } else if (has_children) {
                slot_name = "children";
                UiMarkupSlot slot;
                WG_CHECKED(parse_slot(xml_slot, slot_name, slot, out.cls));
                append_slot(slot);
            } else {
                WG_LOG_ERROR("unknown slot attribute for " << out.cls->get_name() << " while parsing " << xml_slot->Value());
                return StatusCode::InvalidState;
            }
        }

        return WG_OK;
    }

    Status UiMarkupParser::parse_slot(tinyxml2::XMLElement* xml_slot, const std::string& slot_name, UiMarkupSlot& out, RttiClass* cls) {
        auto field = cls->find_field(SID(slot_name));
        if (!field) {
            WG_LOG_ERROR("no such field " << slot_name << " in " << cls->get_name() << " for " << m_name);
            return StatusCode::InvalidData;
        }

        out.field = *field;

        const RttiType* field_type = out.field->get_type();
        const bool      is_ref     = field_type->archetype_is(RttiArchetype::Ref);
        const bool      is_vector  = field_type->archetype_is(RttiArchetype::Vector);

        if (!is_ref && !is_vector) {
            WG_LOG_ERROR("invalid slot type " << field_type->get_name() << " for " << cls->get_name());
            return StatusCode::InvalidData;
        }

        assert(is_ref || is_vector);

        const RttiTypeRef* element_type_ref = nullptr;

        if (is_ref) {
            element_type_ref = static_cast<const RttiTypeRef*>(field_type);
        }
        if (is_vector) {
            const RttiTypeVector* element_type_vector = static_cast<const RttiTypeVector*>(field_type);
            if (!element_type_vector->get_value_type()->archetype_is(RttiArchetype::Ref)) {
                WG_LOG_ERROR("expecting slot type vector of Ref types " << element_type_vector->get_name());
                return StatusCode::InvalidData;
            }
            element_type_ref = static_cast<const RttiTypeRef*>(element_type_vector->get_value_type());
        }

        const RttiClass* element_type = static_cast<const RttiClass*>(element_type_ref->get_value_type());

        if (!element_type->is_subtype_of(UiElement::get_class_static())) {
            WG_LOG_ERROR("slot type must be sub-class of UiElement " << element_type->get_name());
            return StatusCode::InvalidData;
        }

        UiMarkupElement child_element;
        WG_CHECKED(parse_element(xml_slot, child_element));

        out.child_element = static_cast<int>(m_desc.elements.size());
        m_desc.elements.push_back(std::move(child_element));

        return WG_OK;
    }

    Status UiMarkupParser::parse_attribute(const tinyxml2::XMLAttribute* xml_attribute, UiMarkupAttribute& out, RttiClass* cls) {
        std::string attribute_name = xml_attribute->Name();
        bool        is_bind        = false;

        if (StringUtils::is_starts_with(attribute_name, "bind:")) {
            attribute_name = attribute_name.substr(5);
            is_bind        = true;
        }

        auto field = cls->find_field(SID(attribute_name));
        if (!field) {
            WG_LOG_ERROR("no such field " << attribute_name << " in " << cls->get_name() << " for " << m_name);
            return StatusCode::InvalidData;
        }

        out.field                  = *field;
        const RttiType* field_type = out.field->get_type();

        if (!is_bind) {
            if (field_type->archetype_is(RttiArchetype::Optional)) {
                field_type = static_cast<const RttiTypeOptional*>(field_type)->get_value_type();
            }

            const RttiType* type_bool   = rtti_type<bool>();
            const RttiType* type_int    = rtti_type<int>();
            const RttiType* type_float  = rtti_type<float>();
            const RttiType* type_strid  = rtti_type<Strid>();
            const RttiType* type_string = rtti_type<std::string>();

            if (field_type == type_bool) {
                bool value;
                if (xml_attribute->QueryBoolValue(&value)) {
                    WG_LOG_ERROR("failed to parse attribute value " << attribute_name << " of " << field_type->get_name());
                    return StatusCode::InvalidData;
                }
                out.value = std::move(Var(value));
            } else if (field_type == type_int) {
                int value;
                if (xml_attribute->QueryIntValue(&value)) {
                    WG_LOG_ERROR("failed to parse attribute value " << attribute_name << " of " << field_type->get_name());
                    return StatusCode::InvalidData;
                }
                out.value = std::move(Var(value));
            } else if (field_type == type_float) {
                float value;
                if (xml_attribute->QueryFloatValue(&value)) {
                    WG_LOG_ERROR("failed to parse attribute value " << attribute_name << " of " << field_type->get_name());
                    return StatusCode::InvalidData;
                }
                out.value = std::move(Var(value));
            } else if (field_type == type_strid) {
                out.value = std::move(Var(Strid(xml_attribute->Value())));
            } else if (field_type == type_string) {
                out.value = std::move(Var(std::string(xml_attribute->Value())));
            } else {
                WG_LOG_ERROR("unknown attribute type " << attribute_name << " " << field_type->get_name());
                return StatusCode::InvalidData;
            }
        } else {
            auto method = m_desc.bindable->find_method(SID(xml_attribute->Value()));
            if (!method) {
                WG_LOG_ERROR("no such method " << xml_attribute->Value() << " in " << cls->get_name() << " for " << m_name);
                return StatusCode::InvalidData;
            }

            out.bind_method = *method;

            const RttiTypeFunction* function = out.bind_method->get_function();

            if (function->get_ret().type != nullptr) {
                WG_LOG_ERROR("invalid function for bind " << function->get_name());
                return StatusCode::InvalidData;
            }
            if (function->get_args().size() != 1) {
                WG_LOG_ERROR("invalid function for bind " << function->get_name());
                return StatusCode::InvalidData;
            }
            if (!function->get_args()[0].type->archetype_is(RttiArchetype::Class)) {
                WG_LOG_ERROR("invalid function for bind " << function->get_name());
                return StatusCode::InvalidData;
            }
        }

        return WG_OK;
    }

}// namespace wmoge