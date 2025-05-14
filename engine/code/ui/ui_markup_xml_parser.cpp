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

    template<typename T>
    struct UiMarkupValueParser {
        T                     value;
        const Ref<UiElement>& element;
        const RttiField*      field;
        const RttiType*       field_type;

        UiMarkupValueParser(const Ref<UiElement>& element, const RttiField* field)
            : element(element), field(field), field_type(field->get_type()) {
        }

        template<typename XmlAttributeParser>
        Status parse(XmlAttributeParser&& attribute_parser) {
            if (attribute_parser(value)) {
                return StatusCode::InvalidData;
            }

            std::size_t   offset = field->get_byte_offset();
            std::uint8_t* dst    = reinterpret_cast<std::uint8_t*>(element.get()) + offset;

            if (field_type->archetype_is(RttiArchetype::Optional)) {
                static_cast<const RttiTypeOptional*>(field_type)->set_value(dst, &value);
            } else {
                field_type->copy(dst, &value);
            }

            return WG_OK;
        }
    };

    UiMarkupParser::UiMarkupParser(std::string name, UiMarkupDecs& desc, array_view<const std::uint8_t> xml_buffer, RttiTypeStorage* type_storage)
        : m_name(std::move(name)), m_desc(desc), m_xml_buffer(xml_buffer), m_type_storage(type_storage) {
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

        Ref<UiElement> root;
        WG_CHECKED(parse_element(xml_root, root));
        m_desc.root = root;

        return WG_OK;
    }

    Status UiMarkupParser::parse_element(tinyxml2::XMLElement* xml_node, Ref<UiElement>& out) {
        RttiClass* cls = m_type_storage->find_class(SID(xml_node->Value()));
        if (!cls) {
            WG_LOG_ERROR("no such element class " << xml_node->Value());
            return StatusCode::InvalidData;
        }
        if (!cls->can_instantiate()) {
            WG_LOG_ERROR("such class cannot be instantiated " << xml_node->Value());
            return StatusCode::InvalidData;
        }

        out = cls->instantiate().cast<UiElement>();
        if (!out) {
            WG_LOG_ERROR("failed to instantiate element " << xml_node->Value());
            return StatusCode::InvalidData;
        }

        for (auto xml_atribute = xml_node->FirstAttribute(); xml_atribute; xml_atribute = xml_atribute->Next()) {
            WG_CHECKED(parse_attribute(xml_atribute, out, cls));
        }

        for (auto xml_slot = xml_node->FirstChildElement(); xml_slot; xml_slot = xml_slot->NextSiblingElement()) {
            std::string slot_name = xml_slot->Value();

            if (slot_name == "bindings") {
                for (auto xml_binding = xml_slot->FirstChildElement(); xml_binding; xml_binding = xml_binding->NextSiblingElement()) {
                    WG_CHECKED(parse_binding(xml_binding, out));
                }
            } else if (auto field = cls->find_field(SID(slot_name)).value_or(nullptr)) {
                tinyxml2::XMLElement* xml_node = xml_slot->FirstChildElement();
                if (!xml_node) {
                    WG_LOG_ERROR("no node to parse " << slot_name << " for " << m_name);
                    return StatusCode::InvalidData;
                }

                const RttiType* type = field->get_type();
                if (!type->archetype_is(RttiArchetype::Ref)) {
                    WG_LOG_ERROR("unexpected type of slot " << slot_name << " for " << m_name);
                    return StatusCode::InvalidData;
                }

                const RttiType* type_ref = static_cast<const RttiTypeRef*>(type)->get_value_type();
                if (!type_ref->archetype_is(RttiArchetype::Class)) {
                    WG_LOG_ERROR("unexpected type of slot " << slot_name << " for " << m_name);
                    return StatusCode::InvalidData;
                }

                if (!reinterpret_cast<const RttiClass*>(type_ref)->is_subtype_of(UiElement::get_class_static())) {
                    WG_LOG_ERROR("expecting UiElement type of slot " << slot_name << " for " << m_name);
                    return StatusCode::InvalidData;
                }

                Ref<UiElement> slot;
                WG_CHECKED(parse_element(xml_node, slot));

                std::size_t   offset = field->get_byte_offset();
                std::uint8_t* dst    = reinterpret_cast<std::uint8_t*>(out.get()) + offset;

                type->copy(dst, &slot);
            } else {
                Ref<UiElement> child;
                WG_CHECKED(parse_element(xml_slot, child));
                out->children.push_back(child);
            }
        }

        return WG_OK;
    }

    Status UiMarkupParser::parse_binding(const tinyxml2::XMLElement* xml_binding, Ref<UiElement>& out) {
        if (std::string("UiBinding") != xml_binding->Value()) {
            WG_LOG_ERROR("unexpected binding name for " << m_name);
            return StatusCode::InvalidData;
        }

        auto xml_property  = xml_binding->FindAttribute("property");
        auto xml_data_path = xml_binding->FindAttribute("data_path");
        auto xml_type      = xml_binding->FindAttribute("type");

        if (!xml_property) {
            WG_LOG_ERROR("no property name in binding for " << m_name);
            return StatusCode::InvalidData;
        }
        if (!xml_data_path) {
            WG_LOG_ERROR("no data_path name in binding for " << m_name);
            return StatusCode::InvalidData;
        }

        UiBinding binding;
        binding.property  = SID(xml_property->Value());
        binding.data_path = SID(xml_data_path->Value());

        if (xml_type) {
            binding.type = Enum::parse<UiBindingType>(xml_type->Value());
        }

        out->bindings.push_back(std::move(binding));

        return WG_OK;
    }

    Status UiMarkupParser::parse_attribute(const tinyxml2::XMLAttribute* xml_attribute, Ref<UiElement>& out, RttiClass* cls) {
        std::string attribute_name = xml_attribute->Name();

        auto field = cls->find_field(SID(attribute_name)).value_or(nullptr);
        if (!field) {
            WG_LOG_ERROR("no such field " << attribute_name << " in " << cls->get_name() << " for " << m_name);
            return StatusCode::InvalidData;
        }

        const RttiType* field_type = field->get_type();

        if (field_type->archetype_is(RttiArchetype::Optional)) {
            field_type = static_cast<const RttiTypeOptional*>(field_type)->get_value_type();
        }

        const RttiType* type_bool   = rtti_type<bool>();
        const RttiType* type_int    = rtti_type<int>();
        const RttiType* type_float  = rtti_type<float>();
        const RttiType* type_strid  = rtti_type<Strid>();
        const RttiType* type_string = rtti_type<std::string>();

        Status status;

        if (field_type == type_bool) {
            status = UiMarkupValueParser<bool>(out, field).parse([&](bool& value) {
                return xml_attribute->QueryBoolValue(&value);
            });
        } else if (field_type == type_int) {
            status = UiMarkupValueParser<int>(out, field).parse([&](int& value) {
                return xml_attribute->QueryIntValue(&value);
            });
        } else if (field_type == type_float) {
            status = UiMarkupValueParser<float>(out, field).parse([&](float& value) {
                return xml_attribute->QueryFloatValue(&value);
            });
        } else if (field_type == type_strid) {
            status = UiMarkupValueParser<Strid>(out, field).parse([&](Strid& value) {
                value = SID(std::string(xml_attribute->Value()));
                return tinyxml2::XML_SUCCESS;
            });
        } else if (field_type == type_string) {
            status = UiMarkupValueParser<std::string>(out, field).parse([&](std::string& value) {
                value = xml_attribute->Value();
                return tinyxml2::XML_SUCCESS;
            });
        } else {
            WG_LOG_ERROR("unknown attribute type " << attribute_name << " " << field_type->get_name());
            return StatusCode::InvalidData;
        }

        if (!status) {
            WG_LOG_ERROR("failed to parse attribute value " << attribute_name << " of " << field_type->get_name());
            return StatusCode::InvalidData;
        }

        return WG_OK;
    }

}// namespace wmoge